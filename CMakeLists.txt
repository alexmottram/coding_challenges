cmake_minimum_required(VERSION 4.0)
project(coding_challenges)

set(CMAKE_CXX_STANDARD 20)
set(test_tools doctest.h doctest.cpp)

set(utils
    utils/utils.h
    utils/std_extensions.h
    utils/precompile_header.cpp utils/precompile_header.h
        utils/prime_utils.cpp
        utils/prime_utils.h
)

set(utils_unittests
    doctest.h doctest.cpp
    utils_tests/prime_utils_tests.cpp)

set(euler_problems
    challenges/euler/problem_1/problem_1.cpp
    challenges/euler/problem_1/problem_1.h
    challenges/euler/problem_2/problem_2.cpp
    challenges/euler/problem_2/problem_2.h
    challenges/euler/problem_3/problem_3.cpp
    challenges/euler/problem_3/problem_3.h
    challenges/euler/problem_4/problem_4.cpp
    challenges/euler/problem_4/problem_4.h
    challenges/euler/problem_5/problem_5.cpp
    challenges/euler/problem_5/problem_5.h
    challenges/euler/euler_test_cases.cpp
    challenges/euler/all_euler_solutions.h
)

add_executable(
    Euler-Solutions
    "${test_tools}"
    "${utils}"
    "${euler_problems}"
)
add_executable(
    Util-Tests
    "${test_tools}"
    "${utils}"
    "${utils_unittests}"
)
