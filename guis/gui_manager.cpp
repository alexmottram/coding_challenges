#include "gui_manager.h"
// Include ImGui platform/renderer backends used by the project
#ifndef NO_IMGUI
#include "imgui/backends/imgui_impl_glfw.h"
#include "imgui/backends/imgui_impl_opengl3.h"
// Include helper that sets GLFW hints and returns GLSL version
#include "imgui_glfw_setup.h"
#endif

namespace guis {

    GUIManager& GUIManager::instance() {
        static GUIManager inst;
        return inst;
    }

    GUIManager::GUIManager() = default;

    GUIManager::~GUIManager() {
        stop();
    }

    bool GUIManager::isMainThread() const {
        return std::this_thread::get_id() == main_thread_id_;
    }

    void GUIManager::start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) return; // already running

        std::cerr << "[GUIManager] start()\n";
        thread_ = std::thread([this]() { threadMain(); });
    }

    void GUIManager::stop() {
        bool expected = true;
        if (!running_.compare_exchange_strong(expected, false)) return; // not running

        std::cerr << "[GUIManager] stop()\n";
        if (thread_.joinable()) thread_.join();
    }

    void GUIManager::register_widget(const std::string& key, const std::string& title, RenderFn render_fn) {
        std::lock_guard lock(widgets_mutex_);
        std::cerr << "[GUIManager] register_widget: " << key << "\n";
        widgets_[key] = WidgetEntry{title, std::move(render_fn)};

        // ensure loop is running
        start();
    }

    void GUIManager::unregister_widget(const std::string& key) {
        std::lock_guard lock(widgets_mutex_);
        std::cerr << "[GUIManager] unregister_widget: " << key << "\n";
        widgets_.erase(key);
    }

    void GUIManager::threadMain() {
        std::cerr << "[GUIManager] threadMain() entry\n";

        // Try to initialize GLFW + ImGui and run a real GUI loop on this thread. If initialization fails,
        // fall back to the mock loop that simply calls widget render functions without ImGui.
        bool got_gui = false;
        GLFWwindow* window = nullptr;
        const char* glsl_version = nullptr;

#ifndef NO_IMGUI
        try {
            if (glfwInit()) {
                glsl_version = imgui_glfw_setup_for_current_platform();

                // Create windowed mode window and its OpenGL context
                glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
                window = glfwCreateWindow(1280, 720, "GUI Manager", nullptr, nullptr);
                if (window) {
                    glfwMakeContextCurrent(window);
                    glfwSwapInterval(1); // enable vsync

                    // Setup Dear ImGui context
                    IMGUI_CHECKVERSION();
                    ImGui::CreateContext();
                    ImGui::StyleColorsDark();

                    // Setup Platform/Renderer bindings
                    ImGui_ImplGlfw_InitForOpenGL(window, true);
                    ImGui_ImplOpenGL3_Init(glsl_version);

                    // Mark this thread as the GUI thread
                    main_thread_id_ = std::this_thread::get_id();

                    got_gui = true;
                    std::cerr << "[GUIManager] Initialized GLFW/ImGui on GUI thread." << std::endl;
                } else {
                    std::cerr << "[GUIManager] Failed to create GLFW window. Falling back to mock loop." << std::endl;
                    glfwTerminate();
                }
            } else {
                std::cerr << "[GUIManager] glfwInit() failed. Falling back to mock loop." << std::endl;
            }
        } catch (const std::exception &e) {
            std::cerr << "[GUIManager] Exception during GUI init: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "[GUIManager] Unknown exception during GUI init" << std::endl;
        }

        if (got_gui && window) {
            // Real GUI loop
            while (running_ && !glfwWindowShouldClose(window)) {
                // Poll and handle events (inputs, window resize, etc.)
                glfwPollEvents();

                // Start the ImGui frame
                ImGui_ImplOpenGL3_NewFrame();
                ImGui_ImplGlfw_NewFrame();
                ImGui::NewFrame();

                // Snapshot the callbacks under lock
                std::vector<RenderFn> callbacks;
                {
                    std::lock_guard lock(widgets_mutex_);
                    callbacks.reserve(widgets_.size());
                    for (auto &kv : widgets_) callbacks.push_back(kv.second.render);
                }

                // Call all callbacks (widgets will use ImGui as they are now on the GUI thread)
                for (auto &cb : callbacks) {
                    try { cb(); } catch (...) { /* swallow exceptions from widgets */ }
                }

                // Render ImGui
                ImGui::Render();
                int display_w, display_h;
                glfwGetFramebufferSize(window, &display_w, &display_h);
                glViewport(0, 0, display_w, display_h);
                glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
                glClear(GL_COLOR_BUFFER_BIT);
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

                glfwSwapBuffers(window);

                // Sleep a short while to avoid busy-looping too hard (frame pacing already handled by vsync)
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }

            // Cleanup ImGui and GLFW for the GUI thread
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();

            if (window) {
                glfwDestroyWindow(window);
                window = nullptr;
            }
            glfwTerminate();
            std::cerr << "[GUIManager] Shutdown GLFW/ImGui cleanly." << std::endl;
        } else
#endif
        {
            // Fallback mock loop (pre-existing behavior) - call registered render()s but they should avoid ImGui
            while (running_) {
                // Snapshot the callbacks under lock
                std::vector<RenderFn> callbacks;
                {
                    std::lock_guard lock(widgets_mutex_);
                    callbacks.reserve(widgets_.size());
                    for (auto &kv : widgets_) callbacks.push_back(kv.second.render);
                }

                // Call all callbacks outside the lock
                for (auto &cb : callbacks) {
                    try { cb(); } catch (...) { /* swallow exceptions from widgets */ }
                }

                // Sleep a short while to avoid busy-looping
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }

        std::cerr << "[GUIManager] threadMain() exit\n";
    }

} // namespace guis
