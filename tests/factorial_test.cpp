#include "doctest.h"

///
/// trivial factorial(...) implementation
///
int factorial(int N)
{
	if (N > 1) {
		return N * factorial(N - 1);
	}

	return 1;
}

/// and now for the test cases
TEST_CASE("testing factorial(...) function")
{
	CHECK(factorial(0) == 1);
	CHECK(factorial(1) == 1);
	CHECK(factorial(2) == 2);
	CHECK(factorial(3) == 6);
	CHECK(factorial(10) == 3628800);
}
