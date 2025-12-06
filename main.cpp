#include <iostream>
#include <thread>
#include "utils/utils.h"
#include "challenges/euler/problem_6/problem_6.h"

int main()
{
    // Results captured from worker
    long long result_1 = 0;
    long long result_2 = 0;

    // Start worker thread(s) which will create progress windows and perform work.
    std::thread worker([&](){
        result_1 = euler::diff_of_sum_of_squares_vs_square_sum(100);
        result_2 = euler::diff_of_sum_of_squares_vs_square_sum(200);
    });

    // Run the UI loop on the main thread (blocks here). This ensures glfwInit()
    // and the OpenGL context are created on the main thread (required on Windows).
    utils::ui_manager::instance().run();

    if (worker.joinable())
        worker.join();

    std::cout << "Result 1: " << result_1 << std::endl;
    std::cout << "Result 2: " << result_2 << std::endl;
    std::cout << "Done." << std::endl;
    return 0;
}