#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
#include "../doctest.h"
#include "../utils/prime_utils.h"
#include "../utils/utils.h"

TEST_SUITE_BEGIN("Prime utils test suite.");

TEST_CASE("Test prime_count_map function.") {
	SUBCASE("Simple price cases")
	{
		std::map<int, int> primes_for_2 {{2, 1}};
		CHECK(utils::prime_count_map(2) == primes_for_2);

		std::map<int, int> primes_for_6 {{2, 1}, {3, 1}};
		CHECK(utils::prime_count_map(6) == primes_for_6);

		std::map<int, int> primes_for_8 {{2, 3}};
		CHECK(utils::prime_count_map(8) == primes_for_8);

		std::map<int, int> primes_for_1672056 {{2, 3}, {3,3}, {7741, 1}};
		CHECK(utils::prime_count_map(1672056) == primes_for_1672056);
	}
}