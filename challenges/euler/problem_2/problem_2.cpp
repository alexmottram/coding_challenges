#include "problem_2.h"

#include <set>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <ostream>

namespace  euler {

	int even_fibonacci_below_limit(int limit) {
		int n_minus_2 = 0;
		int n_minus_1 = 1;
		int n_current = 1;
		int running_tot = 0;
		int iteration_count = 0;

		while (n_current <= limit) {
			iteration_count++;

			std::cout << "Iteration number: " << iteration_count
			<< ". Current numbers: " << n_current << ", " << n_minus_1
			<< ", " << n_minus_2 << "." << std::endl;

			if (n_current%2 == 0) {
				running_tot += n_current;
			}

			n_minus_2 = n_minus_1;
			n_minus_1 = n_current;
			n_current = n_minus_1 + n_minus_2;
		}
		return running_tot;
	}

}
