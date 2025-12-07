#pragma once

#include "../precompile_header.h"
#include "../gui_manager.h"

namespace guis {

    class GuiLogger {
    public:
        GuiLogger(const std::string &key = "gui_logger", const std::string &title = "GUI Logger")
        : key_(key), title_(title) {
            GUIManager::instance().register_widget(key_, title_, [this]() { render(); });
        }

        ~GuiLogger() {
            GUIManager::instance().unregister_widget(key_);
        }

        void append(const std::string &s) {
            std::lock_guard lock(mutex_);
            lines_.push_back(s);
            if (lines_.size() > 1000) lines_.erase(lines_.begin(), lines_.end()-900);
        }

        std::string snapshot() {
            std::lock_guard lock(mutex_);
            std::string out;
            for (auto &l : lines_) { out += l + "\n"; }
            return out;
        }

        void clear() {
            std::lock_guard lock(mutex_);
            lines_.clear();
        }

        void render() {
            // Skeleton render that writes to stdout. Replace with ImGui calls in real GUI.
            std::cout << "[" << title_ << "]\n";
            std::cout << snapshot();
        }

    private:
        std::string key_;
        std::string title_;
        std::vector<std::string> lines_;
        std::mutex mutex_;
    };

} // namespace guis
