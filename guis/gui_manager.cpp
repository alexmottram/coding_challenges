#include "gui_manager.h"

namespace guis {

    GUIManager& GUIManager::instance() {
        static GUIManager inst;
        return inst;
    }

    GUIManager::GUIManager() {
        // noop
    }

    GUIManager::~GUIManager() {
        stop();
    }

    void GUIManager::start() {
        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) return; // already running

        thread_ = std::thread([this]() { threadMain(); });
    }

    void GUIManager::stop() {
        bool expected = true;
        if (!running_.compare_exchange_strong(expected, false)) return; // not running

        if (thread_.joinable()) thread_.join();
    }

    void GUIManager::register_widget(const std::string& key, const std::string& title, RenderFn render_fn) {
        std::lock_guard lock(widgets_mutex_);
        widgets_[key] = WidgetEntry{title, render_fn};

        // ensure loop is running
        start();
    }

    void GUIManager::unregister_widget(const std::string& key) {
        std::lock_guard lock(widgets_mutex_);
        widgets_.erase(key);
    }

    void GUIManager::threadMain() {
        // A simple mock loop that periodically calls the registered render functions.
        // This avoids depending on GLFW/ImGui in the skeleton. Real implementation
        // should initialize GLFW/ImGui here, then in the loop poll events and render.

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

} // namespace guis
