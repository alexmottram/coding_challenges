#include "gui_logger.h"

namespace guis {

    GuiLogger::GuiLogger(std::string key, std::string title)
    : key_(std::move(key)), title_(std::move(title)) {
        GUIManager::instance().register_widget(key_, title_, [this]() { render(); });
    }

    GuiLogger::~GuiLogger() {
        GUIManager::instance().unregister_widget(key_);
    }

    void GuiLogger::log(const std::string &s) {
        std::lock_guard lock(mutex_);
        lines_.push_back(s);
        if (lines_.size() > 1000) lines_.erase(lines_.begin(), lines_.end()-900);
    }

    std::string GuiLogger::snapshot() {
        std::lock_guard lock(mutex_);
        std::string out;
        for (auto &l : lines_) { out += l + "\n"; }
        return out;
    }

    void GuiLogger::clear() {
        std::lock_guard lock(mutex_);
        lines_.clear();
    }

    void GuiLogger::render() {
        // If ImGui isn't initialized in this process/thread, fall back to console output
        // We must only call ImGui from the main thread where the context exists.
        if (!GUIManager::instance().isMainThread() || ImGui::GetCurrentContext() == nullptr) {
            // Skeleton render that writes to stdout. Preserve previous behavior for non-GUI runs.
            std::cout << "[" << title_ << "]\n";
            std::cout << snapshot();
            return;
        }

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
        if (ImGui::Button("Clear", ImVec2(btn_w, 0))) {
            // Treat Clear as clearing the log contents
            clear();
        }

        ImGui::End();
    }

} // namespace guis

