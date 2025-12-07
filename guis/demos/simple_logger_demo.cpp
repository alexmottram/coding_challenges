#include "../guis.h"
#include <thread>
#include <chrono>

int main() {
    guis::GuiLogger logger("test_logger", "Test Logger");
    logger.append("Hello");
    logger.append("World");

    // Let the GUI manager run for a short while
    std::this_thread::sleep_for(std::chrono::seconds(10));
    guis::GUIManager::instance().stop();
    return 0;
}

