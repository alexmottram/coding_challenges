#include "prime_utils.h"

namespace utils {
	std::map<int, int> prime_count_map(const int number) {
		std::map<int, int> result {};
		int current_number = number;
		int factor = 2;

		while (current_number != 1) {
			if (current_number%factor == 0) {
				if (result.contains(factor)) {
					result[factor] = result[factor] + 1;
				} else {
					result[factor] = 1;
				}
				current_number = current_number/factor;
			} else {
				factor++;
			}
		}

		return result;
	}
}
