#include "../guis.h"

int main(int, char**)
{
    // GUIManager::instance().start(); // no longer required; logger registration will start GUI lazily

    // create three GuiLogger objects and keep shared_ptrs to write to them

    guis::GuiLogger logger_three("mod_3", "Multiples of 3");
    guis::GuiLogger logger_seven("mod_7", "Multiples of 7");
    guis::GuiLogger logger_thirteen("mod_13", "Multiples of 13");

    for (int current = 1; current <= 100; ++current) {
        // std::this_thread::sleep_for(std::chrono::milliseconds(200));
        if (current % 3 == 0) logger_three.append(std::format("Hit {}", current));
        if (current % 7 == 0) logger_seven.append(std::format("Hit {}", current));
        if (current % 13 == 0) logger_thirteen.append(std::format("Hit {}", current));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // give GUI a moment to display
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return 0;
}
