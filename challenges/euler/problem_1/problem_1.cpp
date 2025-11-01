#include "problem_1.h"

#include <set>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>

namespace  euler {

	int sum_of_multiples_integers_in_range(const int a, const int b, const int multiple) {
		const int a_div_by_multiple = (a-1)/multiple;
		const int b_div_by_multiple = (b-1)/multiple;
		const int sum_of_range = ((b_div_by_multiple*(b_div_by_multiple+1))-(a_div_by_multiple*(a_div_by_multiple+1))) / 2 ;
		return multiple * sum_of_range;
	}

	int sum_of_multiple_below_limit(const int limit) {
		const int sum_of_multiples_3 = sum_of_multiples_integers_in_range(0, limit, 3);
		const int sum_of_multiples_5 = sum_of_multiples_integers_in_range(0, limit, 5);
		const int sum_of_multiples_15 = sum_of_multiples_integers_in_range(0, limit, 15);

		std::cout << "Sum of multiples of 3 to limit " << limit << ": " << sum_of_multiples_3 << std::endl;
		std::cout << "Sum of multiples of 5 to limit " << limit << ": " << sum_of_multiples_5 << std::endl;
		std::cout << "Sum of multiples of 15 to limit " << limit << ": " << sum_of_multiples_15 << std::endl;

		const int total = sum_of_multiples_3 + sum_of_multiples_5 - sum_of_multiples_15;

		return total;
	};

}
