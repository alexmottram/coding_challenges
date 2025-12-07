#include "../utils/guis/gui_manager.h"
#include "../utils/guis/widgets/gui_logger.h"

int main(int, char**)
{
    // GUIManager::instance().start(); // no longer required; logger registration will start GUI lazily

    // create three GuiLogger objects and keep shared_ptrs to write to them
    auto log3 = GuiLogger::create("div3", "Multiples of 3");
    auto log7 = GuiLogger::create("div7", "Multiples of 7");
    auto log13 = GuiLogger::create("div13", "Multiples of 13");

    for (int current = 1; current <= 100; ++current) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (current % 3 == 0) log3->appendf("Hit %d", current);
        if (current % 7 == 0) log7->appendf("Hit %d", current);
        if (current % 13 == 0) log13->appendf("Hit %d", current);
    }

    // give GUI a moment to display
    std::this_thread::sleep_for(std::chrono::seconds(20));
    GUIManager::instance().stop();
    return 0;
}
