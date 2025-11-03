#pragma once

namespace  euler {

	int even_fibonacci_below_limit(int);

	inline int problem_2_solution() {
		return even_fibonacci_below_limit(4000000);
	};

}