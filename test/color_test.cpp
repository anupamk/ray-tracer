/// c++ includes
#include <iostream>
#include <stdexcept>

/// 3rdparty testing library
#include "doctest.h"

/// our own thing
#include "constants.hpp"
#include "color.hpp"

/// color creation test
TEST_CASE("color creation")
{
	raytracer::color c1(-0.5, 0.4, 1.7);

	CHECK(c1.R() == -0.5f);
	CHECK(c1.G() == 0.4f);
	CHECK(c1.B() == 1.7f);
}

/// color operation tests
TEST_CASE("color operation tests")
{
	const raytracer::color c1(0.9, 0.6, 0.75);
	const raytracer::color c2(0.7, 0.1, 0.25);

	/// addition
	{
		const raytracer::color c1c2_sum = c1 + c2;

		CHECK(raytracer::epsilon_equal(c1c2_sum.R(), 1.6));
		CHECK(raytracer::epsilon_equal(c1c2_sum.G(), 0.7));
		CHECK(raytracer::epsilon_equal(c1c2_sum.B(), 1.0));
	}

	/// subtraction
	{
		const raytracer::color c1c2_diff = c1 - c2;

		CHECK(raytracer::epsilon_equal(c1c2_diff.R(), 0.2));
		CHECK(raytracer::epsilon_equal(c1c2_diff.G(), 0.5));
		CHECK(raytracer::epsilon_equal(c1c2_diff.B(), 0.5));
	}

	/// hadamard multiplication
	{
		const raytracer::color c1c2_mul = c1 * c2;

		CHECK(raytracer::epsilon_equal(c1c2_mul.R(), 0.63));
		CHECK(raytracer::epsilon_equal(c1c2_mul.G(), 0.06));
		CHECK(raytracer::epsilon_equal(c1c2_mul.B(), 0.1875));
	}

	/// scalar multiplication
	{
		const raytracer::color c0 = c1 * 2.0;

		CHECK(raytracer::epsilon_equal(c0.R(), 1.8));
		CHECK(raytracer::epsilon_equal(c0.G(), 1.2));
		CHECK(raytracer::epsilon_equal(c0.B(), 1.5));
	}

	/// comparison
	{
		CHECK((c1 == c2) == false);
	}
}
