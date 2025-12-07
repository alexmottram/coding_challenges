#include "../guis.h"

int main() {
    guis::GuiLogger logger("test_logger", "Test Logger");
    logger.log("Hello");
    std::this_thread::sleep_for(std::chrono::seconds(4));
    logger.log("World");

    // Let the GUI manager run for a short while
    std::this_thread::sleep_for(std::chrono::seconds(10));
    guis::GUIManager::instance().stop();
    return 0;
}
