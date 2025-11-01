#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"

#include "../../doctest.h"
#include "all_euler_solutions.h"


TEST_SUITE_BEGIN("Euler Test Case Solution Suite");

TEST_CASE("Test solution")
{
	CHECK(euler::problem_1_solution()==233168);
}

TEST_SUITE_END;
