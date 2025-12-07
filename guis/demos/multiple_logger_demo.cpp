#include "../guis.h"

int main(int, char**)
{
    const int loop_delay_ms {100};

    guis::GuiLogger logger_three("mod_3", "Multiples of 3");
    guis::GuiLogger logger_seven("mod_7", "Multiples of 7");
    guis::GuiLogger logger_thirteen("mod_13", "Multiples of 13");

    for (int current = 1; current <= 100; ++current) {
        if (current % 3 == 0) logger_three.append(std::format("Hit {}", current));
        if (current % 7 == 0) logger_seven.append(std::format("Hit {}", current));
        if (current % 13 == 0) logger_thirteen.append(std::format("Hit {}", current));
        std::this_thread::sleep_for(std::chrono::milliseconds(loop_delay_ms));
    }

    // give GUI a moment to display
    std::this_thread::sleep_for(std::chrono::seconds(20));
    return 0;
}
