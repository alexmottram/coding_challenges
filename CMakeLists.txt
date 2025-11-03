cmake_minimum_required(VERSION 4.0)
project(coding_challenges)

set(CMAKE_CXX_STANDARD 20)
set(test_tools doctest.h doctest.cpp)
set(euler_problems
    challenges/euler/problem_1/problem_1.cpp
    challenges/euler/problem_1/problem_1.h
    challenges/euler/problem_2/problem_2.cpp
    challenges/euler/problem_2/problem_2.h
    challenges/euler/euler_test_cases.cpp
    challenges/euler/all_euler_solutions.h
)

add_executable(
    Euler-Solutions
    "${test_tools}"
    "${euler_problems}"
)
