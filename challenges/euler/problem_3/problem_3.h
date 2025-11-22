#pragma once
#include <utility>
#include "../../../utils/utils.h"

namespace  euler {

	std::pair<long long, long long> find_first_factor(long long);

	long long largest_prime_factor(long long);


	inline long long problem_3_solution() {
		return largest_prime_factor(600851475143);
	};

}