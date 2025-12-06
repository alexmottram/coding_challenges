// High-level: This header implements a small GUI window that shows
//  - a progress bar (0..100%) and
//  - a scrolling text log.
//
// It is designed so your long-running code can:
//  - set per-instance progress values to update the bar, and
//  - write to std::cout (or another std::ostream) to have messages appear in the log.
//
// Internally it uses:
//  - a background UI thread running a GLFW + ImGui event/render loop,
//  - a worker thread (optional) that executes a user-provided function,
//  - std::atomic to safely share simple values (progress, running flag) between threads,
//  - std::mutex to protect the vector of log lines from concurrent access,
//  - GLFW to create an OS window and OpenGL context for ImGui to draw into.
//
// You typically just construct utils::progress_log_window from your worker code
// (optionally passing &std::cout), and then call reset()/set_speed()/append_log(),
// and write to std::cout as usual; the details below are mostly implementation
// mechanics.

#pragma once
#include "../precompile_header.h"
#include "imgui.h"
#include "imgui_glfw_setup.h"

namespace utils {

    // Forward declare the free-function wrappers implemented in ui_manager.cpp
    // so this header can register/deregister windows and broadcast log lines
    // without including the full ui_manager definition.
    class progress_log_window; // forward declare self for the function prototypes
    void ui_register_window(progress_log_window* window);
    void ui_deregister_window(progress_log_window* window);
    void ui_broadcast_log_line(const std::string& line);

    // Forward declaration for the log sink API used by the streambuf.
    void log_to_progress_bar(const std::string& line);

    // progress_log_streambuf
    // ----------------------
    // Custom std::streambuf that "tees" output:
    //  - every character is forwarded to an underlying streambuf (e.g. std::cout's buffer),
    //  - complete lines (ending with '\n') are also sent to log_to_progress_bar(...), which
    //    appends them to the GUI log in the active progress_log_window, if any.
    //
    // You normally don't use this directly; it is wrapped by scoped_progress_ostream_redirect.
    class progress_log_streambuf : public std::streambuf {
    public:
        explicit progress_log_streambuf(std::streambuf* wrapped)
            : wrapped_(wrapped) {}

    protected:
        int_type overflow(int_type ch) override {
            if (traits_type::eq_int_type(ch, traits_type::eof())) {
                return traits_type::not_eof(ch);
            }
            const char c = static_cast<char>(ch);
            const auto res = wrapped_->sputc(c);
            if (traits_type::eq_int_type(res, traits_type::eof())) {
                return res;
            }
            line_buffer_.push_back(c);
            if (c == '\n') {
                if (!line_buffer_.empty()) {
                    if (line_buffer_.back() == '\n') {
                        line_buffer_.pop_back();
                    }
                    if (!line_buffer_.empty()) {
                        log_to_progress_bar(line_buffer_);
                    }
                    line_buffer_.clear();
                }
            }
            return ch;
        }

        int sync() override {
            return wrapped_->pubsync();
        }

    private:
        std::streambuf* wrapped_;
        std::string line_buffer_;
    };

    // scoped_progress_ostream_redirect
    // --------------------------------
    // RAII helper that temporarily replaces an std::ostream's internal buffer with a
    // progress_log_streambuf. This causes all writes to that stream (e.g. std::cout)
    // to be both printed normally AND forwarded into the GUI log.
    //
    // When this object is destroyed, it restores the original buffer, so the stream
    // goes back to normal behavior.
    class scoped_progress_ostream_redirect {
    public:
        explicit scoped_progress_ostream_redirect(std::ostream& os)
            : os_(os), old_buf_(os.rdbuf()), tee_buf_(old_buf_) {
            os_.rdbuf(&tee_buf_);
        }

        ~scoped_progress_ostream_redirect() {
            os_.rdbuf(old_buf_);
        }

        scoped_progress_ostream_redirect(const scoped_progress_ostream_redirect&) = delete;
        scoped_progress_ostream_redirect& operator=(const scoped_progress_ostream_redirect&) = delete;

    private:
        std::ostream& os_;
        std::streambuf* old_buf_;
        progress_log_streambuf tee_buf_;
    };

    // progress_log_window
    // -------------------
    // A logical ImGui "widget" that shows a progress bar and a text log inside
    // whatever ImGui context/window is active. It does NOT own a GLFW window or
    // its own event loop; those are provided by ui_manager (or your application).
    class progress_log_window {
        std::atomic<float> progress_{0.0f};
        std::atomic<bool> running_{true};
        float speed_{0.5f};
        std::string title_;
        // Desired size for the window's content area; only used when creating
        // the GLFW window. Height is mostly determined automatically.
        std::pair<float, float> bar_size_{300.0f, 0.0f};

        // Protects log_lines_ and scroll_to_bottom_ against concurrent access.
        std::mutex mtx_;
        // Accumulated log lines shown in the ImGui window; guarded by mtx_.
        std::vector<std::string> log_lines_;
        // When true, render() will scroll the log child window to the bottom.
        bool scroll_to_bottom_{false};

        // Optional RAII redirector that mirrors an ostream (e.g. std::cout)
        // into this window's log for the lifetime of the window instance.
        std::unique_ptr<scoped_progress_ostream_redirect> cout_redirect_;
        bool redirect_enabled_{false};

        // Window open flag controlled by ImGui; when false the window will
        // deregister itself from the ui_manager so it stops being rendered.
        std::atomic<bool> open_{true};
        bool first_render_{true};

        // OS-level window and its ImGui context (per-widget)
        GLFWwindow* os_window_{nullptr};
        ImGuiContext* imgui_ctx_{nullptr};
        bool os_ui_initialized_{false};

    public:
        // Constructor
        // -----------
        //  - title: text shown in the window title and header.
        //  - speed: visual smoothing factor for the bar (how quickly it moves toward
        //           the target progress each frame).
        //  - auto_start: if true, immediately launches the UI thread via run_async().
        //  - redirect_stream: if non-null (e.g. &std::cout), installs a scoped
        //                     redirect so that all writes to that stream are
        //                     duplicated into this window's log.
        explicit progress_log_window(std::string title = "Loading...",
                                     float speed = 0.5f,
                                     bool auto_start = true,
                                     std::ostream* redirect_stream = nullptr)
            : progress_{0.0f}, running_{true}, speed_{speed}, title_{std::move(title)} {
            if (redirect_stream) {
                // Install the ostream redirect and mark this instance as the
                // current global log sink so lines are routed into this window.
                cout_redirect_ = std::make_unique<scoped_progress_ostream_redirect>(*redirect_stream);
                redirect_enabled_ = true;
            }

            // Register with UI manager so we'll be rendered in the global UI loop.
            ui_register_window(this);
        }

        // Non-copyable
        progress_log_window(const progress_log_window&) = delete;
        progress_log_window& operator=(const progress_log_window&) = delete;
        ~progress_log_window() {
            // Ensure we destroy any OS window and its ImGui context on the thread
            // that created the GLFW context (ui_manager runs the loop on main).
            if (os_ui_initialized_) {
                // Must make this context current before shutting down ImGui backends
                if (os_window_) glfwMakeContextCurrent(os_window_);
                ImGui::SetCurrentContext(imgui_ctx_);
                ImGui_ImplOpenGL3_Shutdown();
                ImGui_ImplGlfw_Shutdown();
                ImGui::DestroyContext(imgui_ctx_);
                imgui_ctx_ = nullptr;
            }
            if (os_window_) {
                glfwDestroyWindow(os_window_);
                os_window_ = nullptr;
            }
            ui_deregister_window(this);
        }

        // Called by ui_manager (on main/UI thread) to create a per-widget OS window
        // and initialize a dedicated ImGui context for it.
        void create_os_window_if_needed() {
            if (os_window_) return;
            // Configure GLFW window hints appropriate for our GL backend
            const char* glsl_version = imgui_glfw_setup_for_current_platform();
            // Create the GLFW window
            os_window_ = glfwCreateWindow(static_cast<int>(bar_size_.first + 200), 400, title_.c_str(), nullptr, nullptr);
            if (!os_window_) return;
            // Create a new ImGui context for this window
            glfwMakeContextCurrent(os_window_);
            glfwSwapInterval(1);
            imgui_ctx_ = ImGui::CreateContext();
            ImGui::SetCurrentContext(imgui_ctx_);
            ImGui::StyleColorsDark();
            ImGui_ImplGlfw_InitForOpenGL(os_window_, true);
            ImGui_ImplOpenGL3_Init(glsl_version);
            os_ui_initialized_ = true;
        }

        // Render into the per-widget OS window. Must be called from main/UI thread.
        void render_os_window() {
            if (!os_window_ || !os_ui_initialized_) return;
            // Make window's GL context current and set ImGui context
            glfwMakeContextCurrent(os_window_);
            ImGui::SetCurrentContext(imgui_ctx_);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Use existing render() to populate ImGui contents (it uses current context)
            render();

            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(os_window_, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.10f, 0.10f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(os_window_);
        }

        // append_log
        // ----------
        // Adds a new line to the text log shown in the GUI. Thread-safe.
        void append_log(const std::string& line) {
            std::lock_guard<std::mutex> lk(mtx_);
            log_lines_.push_back(line);
            scroll_to_bottom_ = true;
        }

        // reset
        // -----
        // Set the progress value (0..1). Thread-safe.
        void reset(float v) {
            progress_.store(std::clamp(v, 0.0f, 1.0f));
        }

        // Called once per UI frame from the external UI manager.
        void render() {
            if (!running_.load())
                return;
            if (ImGui::GetCurrentContext() == nullptr)
                return;

            ImGuiIO& io = ImGui::GetIO();
            float cur = progress_.load();
            cur = std::min(1.0f, cur + speed_ * io.DeltaTime);
            progress_.store(cur);

            // Attempt to force this ImGui window into its own platform window
            // on the first render when viewports are enabled.
            ImGuiWindowFlags flags = ImGuiWindowFlags_None;
#if defined(ImGuiConfigFlags_ViewportsEnable)
            if (first_render_ && (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)) {
                // Position and size the new window and set the viewport to
                // the main viewport as a starting point. We also disable docking
                // so it becomes a floating top-level window managed by the
                // platform backend.
                ImGuiViewport* main_vp = ImGui::GetMainViewport();
#  if defined(ImGuiViewport) || defined(ImGuiViewportP)
                ImGui::SetNextWindowViewport(main_vp->ID);
#  endif
                ImGui::SetNextWindowPos(ImVec2(main_vp->Pos.x + 200.0f, main_vp->Pos.y + 50.0f), ImGuiCond_Once);
                ImGui::SetNextWindowSize(ImVec2(500.0f, 300.0f), ImGuiCond_Once);
                flags |= ImGuiWindowFlags_NoDocking; // attempt to keep it as a floating window

                first_render_ = false;
            }
#else
            (void)io; // silence unused variable warning when viewports aren't available
#endif

            bool was_open = open_.load();
            ImGui::Begin(title_.c_str(), &was_open, flags);
            if (!was_open) {
                open_.store(false);
            }

            ImGui::TextUnformatted(title_.c_str());
            ImGui::Spacing();

            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 bar_sz(avail.x, 0.0f);
            ImGui::ProgressBar(progress_.load(), bar_sz);
            ImGui::SameLine();
            ImGui::Text(" %.0f%%", progress_.load() * 100.0f);
            ImGui::Spacing();

            ImGui::Separator();
            ImGui::TextUnformatted("Output:");

            ImVec2 full_avail = ImGui::GetContentRegionAvail();
            const float button_row_height = ImGui::GetFrameHeightWithSpacing() * 1.5f;
            float log_height = std::max(40.0f, full_avail.y - button_row_height);

            ImGui::BeginChild("##loader_log", ImVec2(full_avail.x, log_height), true,
                              ImGuiWindowFlags_HorizontalScrollbar);
            {
                std::lock_guard<std::mutex> lk(mtx_);
                for (const auto& line : log_lines_) {
                    ImGui::TextUnformatted(line.c_str());
                }
                if (scroll_to_bottom_) {
                    ImGui::SetScrollHereY(1.0f);
                    scroll_to_bottom_ = false;
                }
            }
            ImGui::EndChild();

            ImGui::Spacing();
            if (ImGui::Button("Cancel"))
                running_.store(false);
            if (progress_.load() >= 1.0f)
                running_.store(false);

            ImGui::End();

            // If the user closed the window, deregister from the manager so
            // it stops being rendered. This is idempotent.
            if (!open_.load()) {
                ui_deregister_window(this);
            }
        }

        [[nodiscard]] bool wants_redirect() const { return redirect_enabled_; }
    };

    // Log helper used by progress_log_streambuf to route completed lines
    // into the GUI logs of any registered progress_log_window that requested
    // redirection.
    inline void log_to_progress_bar(const std::string& line) {
        ui_broadcast_log_line(line);
    }

    // Helper to append a log line to a progress_log_window. Provided as an
    // inline free function to avoid issues with incomplete types when
    // including headers in different orders.
    inline void progress_log_window_append(progress_log_window* w, const std::string& line) {
        if (w) w->append_log(line);
    }

} // namespace utils
