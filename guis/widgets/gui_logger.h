#pragma once

#include "../precompile_header.h"
#include "../gui_manager.h"

namespace guis {

    class GuiLogger {
    public:
        explicit GuiLogger(std::string key = "gui_logger", std::string title = "GUI Logger")
        : key_(std::move(key)), title_(std::move(title)) {
            std::cerr << "[GuiLogger] constructed: " << key_ << "\n";
            GUIManager::instance().register_widget(key_, title_, [this]() { render(); });
        }

        ~GuiLogger() {
            std::cerr << "[GuiLogger] destructing: " << key_ << "\n";
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
            // If ImGui isn't initialized in this process/thread, fall back to console output
            // We must only call ImGui from the main thread where the context exists.
            if (!GUIManager::instance().isMainThread() || ImGui::GetCurrentContext() == nullptr) {
                std::cerr << "[GuiLogger] render() - no ImGui context or not main thread, using console.\n";
                // Skeleton render that writes to stdout. Preserve previous behavior for non-GUI runs.
                std::cout << "[" << title_ << "]\n";
                std::cout << snapshot();
                return;
            }
            std::cerr << "[GuiLogger] render() - using ImGui on main thread.\n";

            // Create an ImGui window sized 600x400 (first use)
            ImGui::SetNextWindowSize(ImVec2(600, 400), ImGuiCond_FirstUseEver);

            // We don't use a persistent 'open' pointer here; Close button will unregister the widget.
            if (!ImGui::Begin(title_.c_str(), nullptr, ImGuiWindowFlags_None)) {
                ImGui::End();
                return;
            }

            // Scrolling child that takes remaining space above the buttons
            ImGui::BeginChild("LogScrollingRegion", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);

            // Snapshot the current log and display it as unformatted text to preserve newlines
            std::string text = snapshot();
            if (!text.empty()) {
                ImGui::TextUnformatted(text.c_str());
                // Auto-scroll to bottom so newest entries are visible
                ImGui::SetScrollHereY(1.0f);
            }

            ImGui::EndChild();

            ImGui::Separator();

            // Buttons: right-aligned
            float avail = ImGui::GetContentRegionAvail().x;
            float btn_w = 80.0f;
            float spacing = ImGui::GetStyle().ItemSpacing.x;
            // Move cursor to the X position where the two buttons fit right-aligned
            float cur_x = ImGui::GetCursorPosX();
            ImGui::SetCursorPosX(cur_x + avail - (btn_w*2 + spacing));

            if (ImGui::Button("Close", ImVec2(btn_w, 0))) {
                // Unregister this widget so it will be removed from the GUI
                GUIManager::instance().unregister_widget(key_);
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(btn_w, 0))) {
                // Treat Cancel as clearing the log contents
                clear();
            }

            ImGui::End();
        }

    private:
        std::string key_;
        std::string title_;
        std::vector<std::string> lines_;
        std::mutex mutex_;
    };

} // namespace guis
