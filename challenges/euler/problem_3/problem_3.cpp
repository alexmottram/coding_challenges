#include "problem_3.h"

#include <iostream>
#include <ostream>
#include <set>
#include <algorithm>


namespace  euler {

	std::pair<long long, long long> find_first_factor_pair(const long long target) {
		long long lower_factor = 1;
		const long long half_target = target / 2;

		while (lower_factor <= half_target) {
			lower_factor ++;

			if (lower_factor % 10000 == 0) {
				std::cout << "Testing for factor: " << lower_factor << std::endl;
			}
			if (target % lower_factor == 0) {
				std::pair factor_pair = {lower_factor, target/lower_factor};
				std::cout << "Found factor pair: " << factor_pair << std::endl;
				return factor_pair;
			}
		}
		return {0, 0};
	}

	long long largest_prime_factor(const long long target) {
		std::set<long long> unverified_factors;
		std::set<long long> verified_primes;
		constexpr std::pair<long long, long long> prime_factor_pair {0, 0};
		int loop_count = 0;
		unverified_factors.emplace(target);

		while (!unverified_factors.empty()) {
			loop_count++;
			std::cout << "Loop count: " << loop_count << std::endl;
			std::cout << "Current prime factors: " << verified_primes << std::endl;
			std::cout << "Current unverified factors: " << unverified_factors << std::endl;

			std::set<long long> next_unverified_factors {};

			for (const long long factor : unverified_factors) {

				auto factor_pair = find_first_factor_pair(factor);

				if (factor_pair == prime_factor_pair) {
					verified_primes.insert(factor);
				} else {
					next_unverified_factors.insert(factor_pair.first);
					next_unverified_factors.insert(factor_pair.second);
				}
			}
			unverified_factors = next_unverified_factors;
		}
		const long long max_factor = *std::ranges::max_element(verified_primes);
		return max_factor;
	}
}
