#include "problem_1.h"

#include <set>
#include <algorithm>
#include <numeric>

namespace  euler {

	int sum_of_multiple_below_limit(int limit) {
		std::set<int> mult_of_three_or_five;
		for (int i = 0; i < limit; i++) {
			if (i%3 == 0 || i%5 == 0) {
				mult_of_three_or_five.insert(i);
			}
		}
		return std::accumulate(mult_of_three_or_five.begin(), mult_of_three_or_five.end(), 0);
	};

}
