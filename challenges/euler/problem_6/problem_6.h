#pragma once
#include "../../../utils/utils.h"

namespace  euler {
	// P[i] = sum_x = 1 to i for x
	// P[i]^2 = (P[i-1]+i)^2 = i^2 + P[i-1]^2 + 2iP[i-1]
	// Q[i] = Sum_i(x^2) = 1^2 + 2^2 + ... + i^2
	// Q[i-1] = Q[i] - i^2
	// S[i] = P[i]^2 - Q[i] = i^2 + P[i-1]^2 + 2iP[i-1] - Q[i-1] - i^2
	// S[i] = S[i-1] + 2iP[i-1]
	// i = 1 then S[1] = S[0] + 2*1*0 = 0
	// i = 2 then S[2] = S[1] + 2*2*1 = 4
	// i = 3 then S[3] = S[2] + 3*3*2 = 18
	// Compare to (2+1)^2 - (2^2 + 1^2) = 9 - 5 = 4
	// Compare to (3+2+1)^2 - (3^2 + 2^2 + 1^2) = 36 - 14 = 22
	// Finally P[i] = i(i+1)/2
	// SO S[i] = SUM_i=2 to i((x^2)(x-1))

	long long diff_of_sum_of_squares_vs_square_sum(long long n);

	inline long long problem_6_solution(long long n) {
		return diff_of_sum_of_squares_vs_square_sum(n);
	};

}