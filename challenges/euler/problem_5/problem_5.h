#pragma once
#include "../../../utils/utils.h"

namespace  euler {

	std::set<int> get_factors(int number);

	int smallest_multiple_up_to_number(int number);

	inline long long problem_5_solution() {
		return smallest_multiple_up_to_number(20);
	};

}