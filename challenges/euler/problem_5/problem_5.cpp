#include "problem_5.h"

#include "../../../utils/prime_utils.h"

namespace  euler {
	int smallest_multiple_up_to_number(const int number) {
		std::map<int, int> repetition_of_prime_factorals {};

		for (int i = 1; i <= number; i++) {
			auto prime_factors_map = utils::prime_count_map(i);
			std::cout << "For number " << i << " updating prime factors with: " << prime_factors_map << std::endl;
			for (auto prime_factor_and_count: prime_factors_map) {
				if (repetition_of_prime_factorals.contains(prime_factor_and_count.first)) {
					repetition_of_prime_factorals[prime_factor_and_count.first] = std::max(
						repetition_of_prime_factorals[prime_factor_and_count.first],
						prime_factor_and_count.second
						);
				} else {
					repetition_of_prime_factorals[prime_factor_and_count.first] = prime_factor_and_count.second;
				}
			}
		}

		int multiple = 1;
		for (auto prime_factor_and_count: repetition_of_prime_factorals) {
			std::cout << "Raising multiple: " << multiple << " by " << prime_factor_and_count.first
			<< " to the power of " << prime_factor_and_count.second << std::endl;
			multiple *= std::pow(prime_factor_and_count.first, prime_factor_and_count.second);
		}

		return multiple;
	}
}
