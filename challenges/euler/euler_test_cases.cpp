#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"

#include "../../doctest.h"
#include "all_euler_solutions.h"


TEST_SUITE_BEGIN("Euler Test Case Solution Suite");

TEST_CASE("Test solution")
{
	// SUBCASE("Problem 1:") {
	// 	CHECK(euler::problem_1_solution()==233168);
	// }
	// SUBCASE("Problem 2:") {
	// 	CHECK(euler::problem_2_solution()==4613732);
	// }
	// SUBCASE("Problem 3:") {
	// 	CHECK(euler::problem_3_solution()==6857);
	// }
	// SUBCASE("Problem 4:") {
	// 	CHECK(euler::problem_4_solution()==906609);
	// }
	// SUBCASE("Problem 5:") {
	// 	CHECK(euler::problem_5_solution()==232792560);
	// }
	SUBCASE("Problem 6:") {
		CHECK(euler::problem_6_solution(10)==2640);
		CHECK(euler::problem_6_solution(100)==25164150);
	}
}

TEST_SUITE_END;
