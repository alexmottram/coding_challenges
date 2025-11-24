#include  <iostream>
#include "challenges/euler/all_euler_solutions.h"

int main()
{
    std::cout << "Running main!" << std::endl;
    auto res {euler::problem_6_solution(100)};
    std::cout << "Result for Euler problem: " << res << std::endl;
    return 0;
}