/// c++ includes
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest.h"

/// our includes
#include "sphere.hpp"
#include "ray.hpp"
#include "intersection_record.hpp"
#include "constants.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("test that sphere.intersect(...) gives right results")
{
	auto const the_sphere = std::make_shared<RT::sphere>();

	struct {
		RT::tuple ray_origin;
		RT::tuple ray_direction;
		std::optional<RT::intersection_records> expected_xs;
	} const all_tc[] = {
		/// [0] : ray actually intersects
		{
			RT::create_point(0.0, 0.0, -5.0),
			RT::create_vector(0.0, 0.0, 1.0),
			RT::create_intersections(RT::intersection_record(4.0, the_sphere),
	                                         RT::intersection_record(6.0, the_sphere)),
		},

		/// [1] : ray is tangent to the sphere
		{
			RT::create_point(0.0, 1.0, -5.0),
			RT::create_vector(0.0, 0.0, 1.0),
			RT::create_intersections(RT::intersection_record(5.0, the_sphere),
	                                         RT::intersection_record(5.0, the_sphere)),

		},

		/// [2] : ray misses the sphere
		{
			RT::create_point(0.0, 2.0, -5.0),
			RT::create_vector(0.0, 0.0, 1.0),
			{},
		},

		/// [3] : ray originates inside the sphere
		{
			RT::create_point(0.0, 0.0, 0.0),
			RT::create_vector(0.0, 0.0, 1.0),
			RT::create_intersections(RT::intersection_record(-1.0, the_sphere),
	                                         RT::intersection_record(1.0, the_sphere)),
		},

		/// [4] : sphere is behind the ray
		{
			RT::create_point(0.0, 0.0, 5.0),
			RT::create_vector(0.0, 0.0, 1.0),
			RT::create_intersections(RT::intersection_record(-6.0, the_sphere),
	                                         RT::intersection_record(-4.0, the_sphere)),
		},
	};

	for (auto tc : all_tc) {
		auto const r      = RT::ray_t(tc.ray_origin, tc.ray_direction);
		auto const got_xs = r.intersect(the_sphere);

		CHECK(got_xs == tc.expected_xs);
	}
}

TEST_CASE("test visible_intersection(...) results")
{
	auto const the_sphere = std::make_shared<RT::sphere>();

	/*
         * there is slight duplication here. specifically, the ixns_list as well
         * as exp_vis_xs contain 1 record which is common to both
        **/
	struct {
		RT::intersection_records ixns_list;
		std::optional<RT::intersection_record> exp_vis_xs;
	} const all_tc[] = {

		/// [0]
		{
			RT::create_intersections(RT::intersection_record(1.0, the_sphere),
	                                         RT::intersection_record(2.0, the_sphere)),
			RT::intersection_record(1.0, the_sphere),
		},

		/// [1]
		{
			RT::create_intersections(RT::intersection_record(-1.0, the_sphere),
	                                         RT::intersection_record(1.0, the_sphere)),
			RT::intersection_record(1.0, the_sphere),
		},

		/// [2]
		{
			RT::create_intersections(RT::intersection_record(-2.0, the_sphere),
	                                         RT::intersection_record(-1.0, the_sphere)),
			{},
		},

		/// [3]
		{
			RT::create_intersections(RT::intersection_record(5.0, the_sphere),
	                                         RT::intersection_record(7.0, the_sphere),
	                                         RT::intersection_record(-3.0, the_sphere),
	                                         RT::intersection_record(2.0, the_sphere)),
			RT::intersection_record(2.0, the_sphere),
		},
	};

	for (auto tc : all_tc) {
		auto const got_vis_xs = RT::visible_intersection(tc.ixns_list);
		CHECK(got_vis_xs == tc.exp_vis_xs);
	}
}

TEST_CASE("test sphere.transform(...) interface")
{
	auto const the_sphere   = std::make_shared<RT::sphere>();
	auto const ident_44_mat = RT::fsize_dense2d_matrix_t::create_identity_matrix(4);

	/// default transformation is a 44 identity matrix
	CHECK(the_sphere->transform() == ident_44_mat);

	/// create+set+validate translation matrix
	auto const xlate_xform = RT::matrix_transformations_t::create_3d_translation_matrix(2.0, 3.0, 4.0);
	the_sphere->transform(xlate_xform);
	CHECK(the_sphere->transform() == xlate_xform);
}

TEST_CASE("test transformed sphere.intersect(...) result")
{
	using RT_XFORM = RT::matrix_transformations_t;

	/// intersect a scaled sphere
	{
		auto const the_sphere = std::make_shared<RT::sphere>();
		auto const r = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
		auto const scale_mat = RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0);

		the_sphere->transform(scale_mat);
		auto const xs_value = r.intersect(the_sphere).value();

		CHECK(xs_value.size() == 2);
		CHECK(xs_value[0].where() == 3.0);
		CHECK(xs_value[1].where() == 7.0);
	}

	/// intersect a translated sphere
	{
		auto const the_sphere = std::make_shared<RT::sphere>();
		auto const r = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
		auto const xlate_mat = RT_XFORM::create_3d_translation_matrix(5.0, 0.0, 0.0);

		the_sphere->transform(xlate_mat);
		auto const xs = r.intersect(the_sphere);

		CHECK(xs.has_value() == false);
	}
}

TEST_CASE("test sphere.normal_at_local(...) returns correct results")
{
	// clang-format off
        struct {
                RT::tuple point;                  /// point in object-space
                RT::tuple exp_normal;             /// expected normal
        } const all_tc[] = {
                /// [0]
                {
                        RT::create_point(1.0, 0.0, 0.0),
                        RT::create_vector(1.0, 0.0, 0.0)
                },

                /// [1]
                {
                        RT::create_point(0.0, 1.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0)
                },

                /// [2]
                {
                        RT::create_point(0.0, 0.0, 1.0),
                        RT::create_vector(0.0, 0.0, 1.0)
                },

                /// [3]
                {
                        RT::create_point(RT::SQRT_3/3.0, RT::SQRT_3/3.0, RT::SQRT_3/3.0),
                        RT::create_vector(RT::SQRT_3/3.0, RT::SQRT_3/3.0, RT::SQRT_3/3.0)
                },
        };
	// clang-format on

	auto const sphere = std::make_shared<RT::sphere>();

	for (auto tc : all_tc) {
		auto const got_normal = sphere->normal_at_local(tc.point);
		CHECK(got_normal == tc.exp_normal);

		/// normal is 'normalized' i.e. magnitude == 1.0
		CHECK(RT::epsilon_equal(magnitude(got_normal), 1.0) == true);
	}
}

TEST_CASE("test shape.normal_at_world(...) returns correct results")
{
	using RT_XFORM = RT::matrix_transformations_t;

	/// normal on a translated sphere
	{
		auto sphere = std::make_shared<RT::sphere>();
		sphere->transform(RT_XFORM::create_3d_translation_matrix(0.0, 1.0, 0.0));

		auto const world_pt         = RT::create_point(0.0, 1.70711, -0.70711);
		auto const got_world_normal = sphere->normal_at_world(world_pt);
		auto const exp_world_normal = RT::create_vector(0.0, 0.70711, -0.70711);

		CHECK(got_world_normal == exp_world_normal);
	}

	/// normal on a scaled + rotated phere
	{
		auto sphere             = std::make_shared<RT::sphere>();
		auto const xform_matrix = (RT_XFORM::create_3d_scaling_matrix(1.0, 0.5, 1.0) *
		                           RT_XFORM::create_rotz_matrix(RT::PI_BY_5F));
		sphere->transform(xform_matrix);

		auto const world_pt         = RT::create_point(0.0, RT::SQRT_2_BY_2F, -RT::SQRT_2_BY_2F);
		auto const got_world_normal = sphere->normal_at_world(world_pt);
		auto const exp_world_normal = RT::create_vector(0.0, 0.97014, -0.24254);

		CHECK(got_world_normal == exp_world_normal);
	}
}

TEST_CASE("test sphere.material(...) interface")
{
	/// default material interface
	auto sphere = std::make_shared<RT::sphere>();
	CHECK(sphere->get_material() == RT::material());

	/// set a different material
	auto new_material = RT::material();
	new_material.set_ambient(0.3).set_specular(0.6).set_shininess(100);
	sphere->set_material(new_material);

	CHECK(sphere->get_material() == new_material);
}
