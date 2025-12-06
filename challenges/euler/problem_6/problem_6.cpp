#include "problem_6.h"
#include <chrono>
#include <thread>

namespace  euler {
	long long diff_of_sum_of_squares_vs_square_sum(const long long n) {
		// Create two independent progress log windows:
		//  - progress_logger_1: updated on every iteration.
		//  - progress_logger_2: updated only when the counter is a multiple of 5.
		utils::progress_log_window progress_logger_1("Problem 6 (every step)", 0.0f, true, &std::cout);
		utils::progress_log_window progress_logger_2("Problem 6 (every 5 steps)", 0.0f, true, &std::cout);

		long long sum_of_diff {0};
		long long counter {n};

		while (counter > 1) {
			// Do the actual computation.
			sum_of_diff += counter * counter * (counter-1);
			counter--;

			// Log to std::cout once; both windows will mirror it because they both
			// installed a redirect on construction.
			std::cout << "Processing count: " << counter << std::endl;

			// Simulate work.
			std::this_thread::sleep_for(std::chrono::milliseconds(50));

			// Compute overall fraction complete in [0, 1].
			const double pct_complete = (static_cast<double>(n) - static_cast<double>(counter)) / static_cast<double>(n);
			const float pct_f = static_cast<float>(pct_complete);

			// Update first logger every iteration.
			progress_logger_1.reset(pct_f);

			// Update second logger only when counter is a multiple of 5.
			if (counter % 5 == 0) {
				progress_logger_2.reset(pct_f);
			}
		}

		// Ensure both loggers reach 100%.
		progress_logger_1.reset(1.0f);
		progress_logger_2.reset(1.0f);

		return sum_of_diff;
 	}
}
