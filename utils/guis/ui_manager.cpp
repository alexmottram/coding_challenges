#include "progress_log_window.h"
#include "ui_manager.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include <algorithm>

namespace utils {

    ui_manager& ui_manager::instance() {
        static ui_manager s;
        return s;
    }

    ui_manager::ui_manager() {
        // Defer initializing GLFW and ImGui until run() is called. We'll initialize
        // lazily inside run_loop() to avoid doing it on static initialization.
    }

    ui_manager::~ui_manager() {
        // Signal shutdown
        loop_running_.store(false);

        // If a background thread was started elsewhere (not by register_window),
        // join it here. We no longer start a thread automatically in register_window.
        if (thread_started_.load() && ui_thread_.joinable()) {
            ui_thread_.join();
        }

        // If we initialized the contexts in this process, clean them up.
        if (!initialized_) return;

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        if (main_window_) {
            glfwDestroyWindow(main_window_);
            main_window_ = nullptr;
        }
        glfwTerminate();
    }

    void ui_manager::register_window(progress_log_window* window) {
        if (!window) return;
        std::lock_guard<std::mutex> lk(windows_mtx_);
        // Avoid duplicates
        if (std::find(windows_.begin(), windows_.end(), window) == windows_.end()) {
            windows_.push_back(window);
        }

        // Do NOT start the UI thread here. Caller must call run() on the main
        // thread to initialize GLFW/ImGui and run the UI loop. This prevents
        // glfwInit() failures on platforms where window/context creation must
        // occur on the main thread (e.g., Windows).
    }

    void ui_manager::deregister_window(progress_log_window* window) {
        if (!window) return;
        std::lock_guard<std::mutex> lk(windows_mtx_);
        windows_.erase(std::remove(windows_.begin(), windows_.end(), window), windows_.end());

        // We do not stop the loop automatically here. If run() is being used
        // on the main thread it will exit when appropriate (e.g., main window
        // closed or the application decides to stop). If you prefer automatic
        // stopping when no windows remain, call stop() explicitly from your
        // application code.
    }

    void ui_manager::broadcast_log_line(const std::string& line) {
        std::lock_guard<std::mutex> lk(windows_mtx_);
        for (auto* w : windows_) {
            if (!w) continue;
            // Only append to windows that have requested redirecting std::cout
            // to their logs.
            if (w->wants_redirect()) {
                w->append_log(line);
            }
        }
    }

    // Free function wrappers
    void ui_register_window(progress_log_window* window) {
        ui_manager::instance().register_window(window);
    }
    void ui_deregister_window(progress_log_window* window) {
        ui_manager::instance().deregister_window(window);
    }
    void ui_broadcast_log_line(const std::string& line) {
        ui_manager::instance().broadcast_log_line(line);
    }

    void ui_manager::run() {
        // Run on the calling thread (blocking). This will initialize and run loop
        // until loop_running_ becomes false or the window is closed by the user.
        loop_running_.store(true);
        run_loop();
    }

    void ui_manager::run_loop() {
        if (!glfwInit()) {
            std::fprintf(stderr, "ui_manager: glfwInit() failed\n");
            loop_running_.store(false);
            return;
        }
        initialized_ = true;

        const char* glsl_version = imgui_glfw_setup_for_current_platform();

        main_window_ = glfwCreateWindow(1280, 720, "Progress Windows", nullptr, nullptr);
        if (!main_window_) {
            std::fprintf(stderr, "ui_manager: glfwCreateWindow() failed\n");
            glfwTerminate();
            initialized_ = false;
            loop_running_.store(false);
            return;
        }

        glfwMakeContextCurrent(main_window_);
        glfwSwapInterval(1);

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(main_window_, true);
        ImGui_ImplOpenGL3_Init(glsl_version);

        while (loop_running_.load() && !glfwWindowShouldClose(main_window_)) {
            glfwPollEvents();

            // For the main window we still create a frame but we will render each
            // progress window into its own OS window/context.
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // Snapshot current windows under lock and operate on them outside the lock
            std::vector<progress_log_window*> snapshot;
            {
                std::lock_guard<std::mutex> lk(windows_mtx_);
                snapshot = windows_;
            }

            // Ensure per-widget OS windows exist and render each into their own window/context.
            for (auto* w : snapshot) {
                if (!w) continue;
                w->create_os_window_if_needed();
                w->render_os_window();
            }

            // We still render an (empty) ImGui frame for the main host window so
            // the application has a visible window. This keeps the single main GLFW
            // window active while per-widget windows are separate.
            ImGui::Render();
            int display_w, display_h;
            glfwGetFramebufferSize(main_window_, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.10f, 0.10f, 0.10f, 1.00f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(main_window_);
        }

        // shutdown will be performed in destructor
        loop_running_.store(false);
    }

} // namespace utils
