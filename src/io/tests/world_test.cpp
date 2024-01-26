/// c++ includes
#include <cstddef>
#include <memory>
#include <vector>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "io/world.hpp"
#include "primitives/color.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// ----------------------------------------------------------------------------
///
TEST_CASE("world::world(...) test")
{
        auto const w = RT::world();
        CHECK(w.lights().size() == 0);
        CHECK(w.shapes().size() == 0);
}

/// ----------------------------------------------------------------------------
///
TEST_CASE("world::create_default_world(...) test")
{
        auto const w = RT::world::create_default_world();

        /// simple checks
        CHECK(w.lights().size() == 1);
        CHECK(w.shapes().size() == 2);
}

/// ----------------------------------------------------------------------------
///
TEST_CASE("world::create_default_world(...) intersection test")
{
        /// a default world
        auto const w = RT::world::create_default_world();

        /// and a ray
        auto const r_origin    = raytracer::create_point(0.0, 0.0, -5.0);
        auto const r_direction = raytracer::create_vector(0.0, 0.0, 1.0);
        auto const r           = raytracer::ray_t(r_origin, r_direction);

        /// and their intersections
        auto const got_xs      = w.intersect(r);
        auto const expected_xs = std::vector<double>{4.0, 4.5, 5.5, 6.0};

        /// verify
        CHECK(got_xs.size() == expected_xs.size());
        for (size_t i = 0; i < got_xs.size(); i++) {
                CHECK(got_xs[i].where() == expected_xs[i]);
        }
}

/// ----------------------------------------------------------------------------
/// shading an intersection from inside
TEST_CASE("world::shade_hit(...) test")
{
        /// a default world
        auto const w = RT::world::create_default_world();

        /// and a ray
        auto const r_origin    = raytracer::create_point(0.0, 0.0, -5.0);
        auto const r_direction = raytracer::create_vector(0.0, 0.0, 1.0);
        auto const r           = raytracer::ray_t(r_origin, r_direction);

        /// first shape in the world
        auto const shape_01   = w.shapes()[0];
        auto const xs_01      = RT::intersection_record(4.0, shape_01);
        auto const xs_list    = RT::intersection_records{xs_01};
        auto const xs_01_info = r.prepare_computations(xs_list);

        /// compute + validate the color
        auto const got_color = w.shade_hit(xs_01_info);
        auto const exp_color = RT::color(0.38066, 0.47583, 0.2855);

        CHECK(got_color == exp_color);
}

#if 0
/// ----------------------------------------------------------------------------
/// shading an intersection from outside
TEST_CASE("world::shade_hit(...) test")
{
	/// a default world with modified light
	auto w               = RT::world::create_default_world();
	w.modify_lights()[0] = RT::point_light(RT::create_point(0.0, 0.25, 0.0), RT::color_white());

	/// and a ray
	auto const r_origin    = raytracer::create_point(0.0, 0.0, 0.0);
	auto const r_direction = raytracer::create_vector(0.0, 0.0, 1.0);
	auto const r           = raytracer::ray_t(r_origin, r_direction);

	/// first shape in the world
	auto const shape_02   = w.shapes()[1];
	auto const xs_02      = RT::intersection_record(0.5, shape_02);
	auto const xs_02_info = r.prepare_computations(xs_02);

	/// compute + validate the color
	auto const got_color = w.shade_hit(xs_02_info);
	auto const exp_color = RT::color(0.90498, 0.90498, 0.90498);

	CHECK(got_color == exp_color);
}

/// ----------------------------------------------------------------------------
/// color when ray misses
TEST_CASE("world::color_at(...) test")
{
	/// a default world
	auto w = RT::world::create_default_world();

	/// and a ray
	auto const r_origin    = RT::create_point(0.0, 0.0, -5.0);
	auto const r_direction = RT::create_vector(0.0, 1.0, 0.0);
	auto const r           = RT::ray_t(r_origin, r_direction);

	auto const got_color = w.color_at(r);
	CHECK(got_color == RT::color_black());
}

/// ----------------------------------------------------------------------------
/// color when ray hits
TEST_CASE("world::color_at(...) test")
{
	/// a default world
	auto w = RT::world::create_default_world();

	/// and a ray
	auto const r_origin    = RT::create_point(0.0, 0.0, -5.0);
	auto const r_direction = RT::create_vector(0.0, 0.0, 1.0);
	auto const r           = RT::ray_t(r_origin, r_direction);

	/// intersection
	auto const got_color = w.color_at(r);
	auto const exp_color = RT::color(0.38066, 0.47583, 0.2855);

	CHECK(got_color == exp_color);
}

/// ----------------------------------------------------------------------------
/// is a point in shadow ? case-1 : when nothing is co-linear between point and
/// light.
TEST_CASE("world::is_shadowed(...) test")
{
	auto w        = RT::world::create_default_world();
	auto xs_point = RT::create_point(0, 10, 0);

	auto got_pt_shadowed = w.is_shadowed(xs_point);
	auto exp_pt_shadowed = false;

	CHECK(got_pt_shadowed == exp_pt_shadowed);
}

/// ----------------------------------------------------------------------------
/// is a point in shadow ? case-2 : when object is between point and light
TEST_CASE("world::is_shadowed(...) test")
{
	auto w        = RT::world::create_default_world();
	auto xs_point = RT::create_point(10, -10, 10);

	auto got_pt_shadowed = w.is_shadowed(xs_point);
	auto exp_pt_shadowed = true;

	CHECK(got_pt_shadowed == exp_pt_shadowed);
}

/// ----------------------------------------------------------------------------
/// is a point in shadow ? case-3 : when object is behind the light
TEST_CASE("world::is_shadowed(...) test")
{
	auto w        = RT::world::create_default_world();
	auto xs_point = RT::create_point(-20, 20, -20);

	auto got_pt_shadowed = w.is_shadowed(xs_point);
	auto exp_pt_shadowed = false;

	CHECK(got_pt_shadowed == exp_pt_shadowed);
}

/// ----------------------------------------------------------------------------
/// is a point in shadow ? case-4 : when object is behind the point
TEST_CASE("world::is_shadowed(...) test")
{
	auto w        = RT::world::create_default_world();
	auto xs_point = RT::create_point(-2, 2, -2);

	auto got_pt_shadowed = w.is_shadowed(xs_point);
	auto exp_pt_shadowed = false;

	CHECK(got_pt_shadowed == exp_pt_shadowed);
}

/// ----------------------------------------------------------------------------
/// shade_hit with point in shade
TEST_CASE("world::shade_hit(...) point is in shadow")
{
	/// create a 'custom' world
	auto w               = RT::world();
	auto const pt_light  = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());
	auto const sphere_01 = std::make_shared<RT::sphere>();
	auto sphere_02       = std::make_shared<RT::sphere>();
	sphere_02->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 10.0));

	/// add the objects
	w.add(pt_light);
	w.add(sphere_01);
	w.add(sphere_02);

	/// and a ray
	auto const r_origin    = raytracer::create_point(0.0, 0.0, 5.0);
	auto const r_direction = raytracer::create_vector(0.0, 0.0, 1.0);
	auto const r           = raytracer::ray_t(r_origin, r_direction);

	/// an intersection record
	auto const xs_01 = RT::intersection_record(4.0, sphere_02);

	/// check the computed vs expected color
	auto const comps     = r.prepare_computations(xs_01);
	auto const got_color = w.shade_hit(comps);
	auto const exp_color = RT::color(0.1, 0.1, 0.1);

	CHECK(got_color == exp_color);
}
#endif
