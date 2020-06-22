/*
 * this file tests the 'world' class interfaces.
**/

/// c++ includes
#include <cstddef>
#include <iostream>
#include <ostream>
#include <vector>

/// 3rdparty testing library
#include "doctest.h"

/// our includes
#include "constants.hpp"
#include "world.hpp"
#include "color.hpp"
#include "tuple.hpp"
#include "material.hpp"
#include "point_light.hpp"
#include "phong_illumination.hpp"
#include "ray.hpp"

/// convenience
namespace RT = raytracer;

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
        auto const r	       = raytracer::ray_t(r_origin, r_direction);

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
        auto const r	       = raytracer::ray_t(r_origin, r_direction);

        /// first shape in the world
        auto const shape_01   = w.shapes()[0];
        auto const xs_01      = RT::intersection_record(4.0, shape_01);
        auto const xs_01_info = r.prepare_computations(xs_01);

        /// compute + validate the color
        auto const got_color = w.shade_hit(xs_01_info);
        auto const exp_color = RT::color(0.38066, 0.47583, 0.2855);

        CHECK(got_color == exp_color);
}

/// ----------------------------------------------------------------------------
/// shading an intersection from outside
TEST_CASE("world::shade_hit(...) test")
{
        /// a default world with modified light
        auto w		     = RT::world::create_default_world();
        w.modify_lights()[0] = RT::point_light(RT::create_point(0.0, 0.25, 0.0), RT::color_white());

        /// and a ray
        auto const r_origin    = raytracer::create_point(0.0, 0.0, 0.0);
        auto const r_direction = raytracer::create_vector(0.0, 0.0, 1.0);
        auto const r	       = raytracer::ray_t(r_origin, r_direction);

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
        auto const r	       = RT::ray_t(r_origin, r_direction);

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
        auto const r	       = RT::ray_t(r_origin, r_direction);

        /// intersection
        auto const got_color = w.color_at(r);
        auto const exp_color = RT::color(0.38066, 0.47583, 0.2855);

        CHECK(got_color == exp_color);
}

/// ----------------------------------------------------------------------------
/// color with an intersection behind the ray
TEST_CASE("world::color_at(...) test")
{
        /// a default world with modified material
        auto w = RT::world::create_default_world();
        w.modify_shapes()[0]->set_material(RT::material().set_ambient(1.0));
        w.modify_shapes()[1]->set_material(RT::material().set_ambient(1.0));

        /// and a ray
        auto const r_origin    = RT::create_point(0.0, 0.0, 0.75);
        auto const r_direction = RT::create_vector(0.0, 0.0, -1.0);
        auto const r	       = RT::ray_t(r_origin, r_direction);

        /// intersection
        auto const got_color = w.color_at(r);
        auto const exp_color = w.shapes()[1]->get_material().get_color();

        CHECK(got_color == exp_color);
}
