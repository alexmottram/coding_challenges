#pragma once
#include <utility>
#include "../../../utils/utils.h"

namespace  euler {

	int int_vector_to_int(std::vector<int>);

	std::vector<int> int_to_int_vector(int);

	bool is_palindrome(std::vector<int>);

	bool is_palindrome(int);

	int create_repeated_digit_number(int digit, int n);

	int max_palindrome_produced_from_multiplication(int);

	inline long long problem_4_solution() {
		return max_palindrome_produced_from_multiplication(999);
	};

}