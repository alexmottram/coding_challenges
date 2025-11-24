#include "problem_6.h"
#include <chrono>
#include <thread>

namespace  euler {
	long long diff_of_sum_of_squares_vs_square_sum(const long long n) {
		long long sum_of_diff {0};
		long long counter {n};

		utils::progress_bar* progress_a = new utils::progress_bar(std::clog, 70u, "Progress bar 1");

		while (counter > 1) {
			sum_of_diff += counter * counter * (counter-1);
			counter--;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			double pct_complete= (static_cast<double>(n)-static_cast<double>(counter))/static_cast<double>(n);
			progress_a->write(pct_complete);
		}
		delete progress_a;
		return sum_of_diff;
	}
}
