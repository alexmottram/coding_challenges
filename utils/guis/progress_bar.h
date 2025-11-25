#pragma once
#include "../precompile_header.h"

namespace utils {

    void report_progress(float p);
    // Forward declaration so progress_log_streambuf can call it.
    void log_to_progress_bar(const std::string& line);

    // A stream buffer that forwards all output to another buffer, and also sends
    // complete lines to the active progress_bar log (if any). This lets code
    // mirror std::cout output into the loader log without manual duplication.
    class progress_log_streambuf : public std::streambuf {
    public:
        explicit progress_log_streambuf(std::streambuf* wrapped)
            : wrapped_(wrapped) {}

    protected:
        int_type overflow(int_type ch) override {
            if (traits_type::eq_int_type(ch, traits_type::eof())) {
                return wrapped_->sputc(ch);
            }

            const char c = static_cast<char>(ch);
            const auto res = wrapped_->sputc(c);
            if (traits_type::eq_int_type(res, traits_type::eof())) {
                return res;
            }

            line_buffer_.push_back(c);
            if (c == '\n') {
                // Strip trailing newline and forward line into the progress bar log.
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

    // RAII helper that installs a progress_log_streambuf on a std::ostream for
    // its lifetime, automatically restoring the original buffer when destroyed.
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

    class progress_bar
    {
        std::atomic<float> progress_{0.0f};
        std::atomic<bool> running_{true};
        float speed_{0.5f};
        std::string title_;
        std::pair<float, float> bar_size_{300.0f, 0.0f};

        std::thread ui_thread_;
        std::thread worker_thread_;
        std::mutex mtx_;
        GLFWwindow* created_window_{nullptr};

        std::vector<std::string> log_lines_;
        bool scroll_to_bottom_{false};

        static inline std::atomic<progress_bar*> global_reporter_{nullptr};

    public:
        // Allow helpers to access internals
        friend inline void report_progress(float p);
        friend inline void log_to_progress_bar(const std::string& line);

        explicit progress_bar(std::string title = "Loading...", float speed = 0.5f, bool auto_start = true)
            : progress_{0.0f}, running_{true}, speed_{speed}, title_{std::move(title)}
        {
            if (auto_start) {
                run_async();
            }
        }

        // not copyable
        progress_bar(const progress_bar&) = delete;
        progress_bar& operator=(const progress_bar&) = delete;

        // Movability: explicitly delete move operations because members like std::atomic/std::thread
        // are not safely movable with compiler-generated defaults in this simple header.
        progress_bar(progress_bar&&) = delete;
        progress_bar& operator=(progress_bar&&) = delete;

        // Custom destructor: ensure background threads are joined and resources cleaned.
        ~progress_bar() { stop_and_join(); }

        // control
        void start() { running_.store(true); }
        void stop() { running_.store(false); if (created_window_) glfwPostEmptyEvent(); }
        [[nodiscard]] bool is_running() const { return running_.load(); }

        void reset(float v = 0.0f) { progress_.store(std::clamp(v, 0.0f, 1.0f)); }
        void set_speed(float s) { speed_ = s; }
        [[nodiscard]] float progress() const { return progress_.load(); }
        void set_bar_size(float w, float h = 0.0f) { bar_size_.first = w; bar_size_.second = h; }

        // Append a line to the embedded log. Thread-safe; wakes the UI thread.
        void append_log(const std::string& line)
        {
            std::lock_guard<std::mutex> lk(mtx_);
            log_lines_.push_back(line);
            scroll_to_bottom_ = true;
        }

        // Allow the free helper to access global_reporter_
        friend inline void report_progress(float p);

        // Run a non-blocking UI: spawn a UI thread which creates the window & backends and
        // exits once progress reaches 100% (or stop() is called). Worker runs on a separate thread.
        // Returns immediately.
        void run_async(std::function<void(progress_bar&)> worker = {},
                       std::chrono::milliseconds frame_period = std::chrono::milliseconds(16),
                       void* imgui_context = nullptr)
        {
            // If already running an async UI, do nothing
            if (ui_thread_.joinable())
                return;

            global_reporter_.store(this);
            running_.store(true);

            std::atomic<bool> worker_done{false};
            if (worker) {
                worker_thread_ = std::thread([this, &worker_done, worker]() {
                    try { worker(*this); } catch (...) {}
                    worker_done.store(true);
                });
            }

            ui_thread_ = std::thread([this, frame_period, imgui_context, &worker_done]() {
                // Optional external context: if provided, assume caller manages its lifetime.
                ImGuiContext* prev_ctx = nullptr;
                if (imgui_context) {
                    prev_ctx = ImGui::GetCurrentContext();
                    ImGui::SetCurrentContext(reinterpret_cast<ImGuiContext*>(imgui_context));
                }

                bool created_backends = false;
                bool initialized_glfw = false;

                if (ImGui::GetCurrentContext() == nullptr) {
                    if (!glfwInit()) {
                        std::fprintf(stderr, "progress_bar::run_async: glfwInit() failed\n");
                        return;
                    }
                    initialized_glfw = true;

#if defined(IMGUI_IMPL_OPENGL_ES2)
                    const char* glsl_version = "#version 100";
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
                    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
                    const char* glsl_version = "#version 300 es";
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
                    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
                    const char* glsl_version = "#version 150";
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
                    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
                    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
                    const char* glsl_version = "#version 130";
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
                    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif

                    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
                    created_window_ = glfwCreateWindow(static_cast<int>(bar_size_.first + 120), 200,
                                                       title_.c_str(), nullptr, nullptr);
                    if (!created_window_) {
                        std::fprintf(stderr, "progress_bar::run_async: glfwCreateWindow() failed\n");
                        if (initialized_glfw) glfwTerminate();
                        return;
                    }

                    glfwMakeContextCurrent(created_window_);
                    glfwSwapInterval(1);

                    IMGUI_CHECKVERSION();
                    ImGui::CreateContext();
                    ImGui::StyleColorsDark();

                    ImGui_ImplGlfw_InitForOpenGL(created_window_, true);
                    ImGui_ImplOpenGL3_Init(glsl_version);

                    created_backends = true;
                }

                auto used_preframe = [created_backends]() {
                    if (created_backends) {
                        ImGui_ImplOpenGL3_NewFrame();
                        ImGui_ImplGlfw_NewFrame();
                    }
                };

                auto used_backend_render = [this]() {
                    ImGui::Render();
                    int display_w, display_h;
                    glfwGetFramebufferSize(created_window_, &display_w, &display_h);
                    glViewport(0, 0, display_w, display_h);
                    glClearColor(0.10f, 0.10f, 0.10f, 1.00f);
                    glClear(GL_COLOR_BUFFER_BIT);
                    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
                    glfwSwapBuffers(created_window_);
                };

                // Smooth, polling-based loop: run at a steady frame rate instead of waiting on events.
                while (running_.load() && progress_.load() < 1.0f) {
                    glfwPollEvents();
                    if (created_window_ && glfwWindowShouldClose(created_window_)) {
                        running_.store(false);
                        break;
                    }

                    used_preframe();
                    ImGui::NewFrame();
                    render();
                    ImGui::Render();
                    used_backend_render();

                    if (worker_done.load() && progress_.load() >= 1.0f)
                        break;

                    std::this_thread::sleep_for(frame_period);
                }

                if (created_backends) {
                    ImGui_ImplOpenGL3_Shutdown();
                    ImGui_ImplGlfw_Shutdown();
                    ImGui::DestroyContext();
                    if (created_window_) {
                        glfwDestroyWindow(created_window_);
                        created_window_ = nullptr;
                    }
                    if (initialized_glfw) {
                        glfwTerminate();
                    }
                }
                if (imgui_context) {
                    ImGui::SetCurrentContext(prev_ctx);
                }
                running_.store(false);
                global_reporter_.store(nullptr);
            });
        }

        // Blocking wait for the async UI to finish (if run_async was used).
        void wait()
        {
            if (ui_thread_.joinable()) ui_thread_.join();
            if (worker_thread_.joinable()) worker_thread_.join();
        }

        // Stop UI and join threads
        void stop_and_join()
        {
            stop();
            if (ui_thread_.joinable()) ui_thread_.join();
            if (worker_thread_.joinable()) worker_thread_.join();
        }

        // Render the loader window. Call this once per frame after ImGui::NewFrame().
        void render()
        {
            if (!running_.load())
                return;
            if (ImGui::GetCurrentContext() == nullptr)
                return;

            ImGuiIO& io = ImGui::GetIO();
            float cur = progress_.load();
            cur = std::min(1.0f, cur + speed_ * io.DeltaTime);
            progress_.store(cur);

            // Make the ImGui window fill the entire main viewport, so the panel
            // always fits the full GLFW window.
            ImGuiViewport* vp = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(vp->Pos);
            ImGui::SetNextWindowSize(vp->Size);

            ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar;
            ImGui::SetNextWindowBgAlpha(0.85f);

            ImGui::Begin(title_.c_str(), nullptr, flags);
            ImGui::TextUnformatted(title_.c_str());
            ImGui::Spacing();

            // Progress bar spans full width
            ImVec2 avail = ImGui::GetContentRegionAvail();
            ImVec2 bar_sz(avail.x, 0.0f);
            ImGui::ProgressBar(progress_.load(), bar_sz);
            ImGui::SameLine();
            ImGui::Text(" %.0f%%", progress_.load() * 100.0f);
            ImGui::Spacing();

            // Output section + cancel button share the remaining height.
            // Reserve a small fixed height for the button row at the bottom.
            ImGui::Separator();
            ImGui::TextUnformatted("Output:");

            // Remaining space after header/bar within the window
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

            // Button row
            ImGui::Spacing();
            if (ImGui::Button("Cancel"))
                running_.store(false);
            if (progress_.load() >= 1.0f)
                running_.store(false);

            ImGui::End();
        }
    };

    // Helper that allows long-running functions to report progress without needing a
    // progress callback parameter. It forwards to the globally-registered progress_bar
    // instance (if any).
    inline void report_progress(float p)
    {
        auto g = progress_bar::global_reporter_.load();
        if (g) g->reset(p);
    }

    // Helper to mirror std::cout-style output into the active progress bar's log.
    inline void log_to_progress_bar(const std::string& line)
    {
        auto g = progress_bar::global_reporter_.load();
        if (g) g->append_log(line);
    }

}
