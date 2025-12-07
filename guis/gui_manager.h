#pragma once
#include "precompile_header.h"

namespace guis {
    class GUIManager {
        public:
            // Singleton access
            static GUIManager& instance();

            GUIManager(const GUIManager&) = delete;
            GUIManager& operator=(const GUIManager&) = delete;

            void start();
            void stop();

            using RenderFn = std::function<void()>;

            // register/unregister widgets (called by widget factory/destructor)
            void register_widget(const std::string& key, const std::string& title, RenderFn render_fn);
            void unregister_widget(const std::string& key);

        private:
            GUIManager();
            ~GUIManager();
            void threadMain();

            struct WidgetEntry {
                std::string title;
                RenderFn render;
            };

            std::map<std::string, WidgetEntry> widgets_;
            std::mutex widgets_mutex_;

            std::thread thread_;
            std::atomic<bool> running_{false};
    };
}
