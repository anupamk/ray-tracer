/// c++ includes
#include "matrix.hpp"
#include <stdexcept>

/// 3rdparty testing library
#include "doctest.h"

/// our own thing
#include "ray.hpp"
#include "tuple.hpp"
#include "matrix_transformations.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

TEST_CASE("test ray.transform(...) results")
{
	/// convenience
	namespace RT       = raytracer;
	using matrix_xform = RT::matrix_transformations_t;

	struct {
		RT::ray_t ray;
		RT::fsize_dense2d_matrix_t transform_matrix;
		RT::ray_t expected_ray;
	} const tc_list[] = {
		/// [0]
		{
			RT::ray_t(RT::create_point(1.0, 2.0, 3.0), RT::create_vector(0.0, 1.0, 0.0)),
			matrix_xform::create_3d_translation_matrix(3.0, 4.0, 5.0),
			RT::ray_t(RT::create_point(4.0, 6.0, 8.0), RT::create_vector(0.0, 1.0, 0.0)),
		},

		/// [1]
		{
			RT::ray_t(RT::create_point(1.0, 2.0, 3.0), RT::create_vector(0.0, 1.0, 0.0)),
			matrix_xform::create_3d_scaling_matrix(2.0, 3.0, 4.0),
			RT::ray_t(RT::create_point(2.0, 6.0, 12.0), RT::create_vector(0.0, 3.0, 0.0)),
		},
	};

	for (auto const tc : tc_list) {
		auto const got_ray = tc.ray.transform(tc.transform_matrix);
		CHECK(got_ray == tc.expected_ray);
	}
}
