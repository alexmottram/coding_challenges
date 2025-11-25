#include "problem_6.h"
#include <chrono>
#include <thread>

namespace  euler {
	long long diff_of_sum_of_squares_vs_square_sum(const long long n) {
		// Create a loader specific to this computation; constructing it starts the async GUI
		utils::progress_bar loader("Problem 6 running", 0.0f, true);

		// Automatically mirror std::cout output into the loader log for the duration
		// of this computation, so we don't have to call log_to_progress_bar manually.
		utils::scoped_progress_ostream_redirect cout_redirect(std::cout);

		long long sum_of_diff {0};
		long long counter {n};

		// Report progress via utils::report_progress() so callers don't need to pass callbacks.
		while (counter > 1) {
			sum_of_diff += counter * counter * (counter-1);
			counter--;
			std::cout << "Processing count: " << counter << std::endl;
			// Simulate work and report progress
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			double pct_complete = (static_cast<double>(n) - static_cast<double>(counter)) / static_cast<double>(n);
			utils::report_progress(static_cast<float>(pct_complete));
		}
		// ensure completion
		utils::report_progress(1.0f);
		return sum_of_diff;
	}
}
