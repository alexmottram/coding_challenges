#pragma once

#include "../precompile_header.h"
#include "imgui_glfw_setup.h"
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

namespace utils {

    // Forward declaration to avoid circular include; progress_log_window will
    // include this header to register/deregister itself.
    class progress_log_window;

    // Simple UI manager that owns a single GLFW window and ImGui context,
    // and can render any number of progress_log_window instances inside it.
    //
    // Key changes:
    // - Singleton access via instance().
    // - register_window / deregister_window are thread-safe and will start a
    //   background UI thread on first registration.
    // - Windows are referenced by raw pointer (they are owned by callers);
    //   progress_log_window will automatically register/deregister itself.
    class ui_manager {
    public:
        // Get the global instance. Constructed on first use.
        static ui_manager& instance();

        ui_manager();
        ~ui_manager();

        // Non-copyable
        ui_manager(const ui_manager&) = delete;
        ui_manager& operator=(const ui_manager&) = delete;

        // Register / deregister a logical window for rendering. These are
        // thread-safe and may be called from worker threads.
        void register_window(progress_log_window* window);
        void deregister_window(progress_log_window* window);

        // Broadcast a log line to any registered progress_log_window that asked
        // for redirected ostream output (i.e. created with redirect_stream).
        void broadcast_log_line(const std::string& line);

        // If you prefer to run the UI loop on the calling thread (blocking),
        // call run() directly. Otherwise, the manager will start a background
        // thread on first register_window() call.
        void run();

    private:
        // Internal loop executed either on the background thread or in run().
        void run_loop();

        // GLFW / ImGui context
        GLFWwindow* main_window_{};
        bool initialized_{false};

        // Thread-safety for windows_
        std::mutex windows_mtx_;
        std::vector<progress_log_window*> windows_;

        // Background thread management
        std::thread ui_thread_;
        std::atomic<bool> loop_running_{false};
        std::atomic<bool> thread_started_{false};
    };

    // Free functions wrappers so callers (progress_log_window.h) don't need to
    // include the ui_manager definition (avoids circular include issues).
    void ui_register_window(progress_log_window* window);
    void ui_deregister_window(progress_log_window* window);
    void ui_broadcast_log_line(const std::string& line);

} // namespace utils
