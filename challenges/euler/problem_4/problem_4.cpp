#include "problem_4.h"

#include <iostream>
#include <ostream>
#include <string>


namespace  euler {

	int int_vector_to_int(std::vector<int> int_vector) {
		std::stringstream ss;
		for (int digit : int_vector) {
			ss << digit;
		}
		int out_in = std::stoi(ss.str());
		return out_in;
	}

	std::vector<int> int_to_int_vector(int int_in) {
		std::string int_str = std::to_string(int_in);
		std::vector<int> int_vector;
		for (auto loop_char: int_str ) {
			if (loop_char >= '0' and loop_char <= '9') {
				int_vector.push_back(loop_char - '0');
			} else {
				std::runtime_error("Character out of range.");
			}
		}
		return int_vector;
	}

	bool is_palindrome(std::vector<int> vec_in) {
		const int length = vec_in.size()/2;

		for (int i = 0; i < length; i++) {
			if (vec_in[i] != vec_in[vec_in.size()-i-1]) {
				return false;
			}
		}
		return true;
	}

	bool is_palindrome(const int int_in) {
		const std::vector<int> int_vector = int_to_int_vector(int_in);
		return is_palindrome(int_vector);
	}

	int max_palindrome_produced_from_multiplication(int max_num) {
		int max_palindrome {1};

		for (int i = 1; i <= max_num; i++) {
			for (int j = 1; j <= i; j++) {
				int test_num = i * j;
				if (test_num > max_palindrome & is_palindrome(test_num)) {
					std::cout << "New largest palindrome is: " << test_num
					<< " from multiplying: " << i << " and " << j <<  std::endl;
					max_palindrome = test_num;
			}
			}
		}
		return max_palindrome;
	}

	int next_smallest_palindrome(int digit) {
		return 1;
	}

}
