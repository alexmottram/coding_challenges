#pragma once

#include "../precompile_header.h"
#include "../gui_manager.h"

namespace guis {

    class GuiLogger {
    public:
        explicit GuiLogger(std::string key = "gui_logger", std::string title = "GUI Logger");
        ~GuiLogger();

        // Add a log entry
        void log(const std::string &s);

        // Capture current contents as a single string
        std::string snapshot();

        // Clear the log contents
        void clear();

        // Render the logger UI (ImGui) or fallback to console
        void render();

    private:
        std::string key_;
        std::string title_;
        std::vector<std::string> lines_;
        std::mutex mutex_;
    };

} // namespace guis
