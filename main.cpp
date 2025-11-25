#include <iostream>
#include "utils/utils.h"
#include "challenges/euler/problem_6/problem_6.h"

int main()
{
    long long result_1 = euler::diff_of_sum_of_squares_vs_square_sum(100);
    long long result_2 = euler::diff_of_sum_of_squares_vs_square_sum(200);

    std::cout << "Result 1: " << result_1 << std::endl;
    std::cout << "Result 2: " << result_2 << std::endl;
    std::cout << "Done." << std::endl;
    return 0;
}