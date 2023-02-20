/// c++ includes
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest/doctest.h"

/// our includes
#include "patterns/align_check_pattern.hpp"
#include "patterns/material.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cylinder.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// ensure that ray-cylinder intersections are good
TEST_CASE("scenario: a ray misses the cylinder")
{
        auto const the_cylinder = std::make_shared<RT::cylinder>();
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
        } const all_tc[] = {

                /// [0] : on surface along +y
                {
                        RT::create_point(1.0, 0.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },

                /// [1] : inside cylined along +y
                {
                        RT::create_point(0.0, 0.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },

                /// [2] : outside, but oriented askew from all axes
                {
                        RT::create_point(0.0, 0.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },
        };

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs = r.intersect(the_cylinder);

                CHECK(got_xs.has_value() == false);
        }
}

/// ----------------------------------------------------------------------------
/// ensure that ray-cylinder intersections are good
TEST_CASE("scenario: a ray hits the cylinder")
{
        auto const the_cylinder = std::make_shared<RT::cylinder>();
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                std::optional<RT::intersection_records> expected_xs;
        } const all_tc[] = {

                /// [0] : tangent intersection
                {
                        RT::create_point(1.0, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        RT::create_intersections(RT::intersection_record(5.0, the_cylinder),
                                                 RT::intersection_record(5.0, the_cylinder)),
                },

                /// [1] : perpendicular through the middle
                {
                        RT::create_point(0.0, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cylinder),
                                                 RT::intersection_record(6.0, the_cylinder)),
                },

                /// [2] : strikes at an angle
                {
                        RT::create_point(0.5, 0.0, -5.0),
                        RT::create_vector(0.1, 1.0, 1.0),
                        RT::create_intersections(RT::intersection_record(6.80798, the_cylinder),
                                                 RT::intersection_record(7.08872, the_cylinder)),
                },
        };

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs = r.intersect(the_cylinder);

                CHECK(got_xs.has_value() == true);
                CHECK(got_xs.value().size() == 2);
                CHECK(got_xs.value() == tc.expected_xs.value());
        }
}

/// ----------------------------------------------------------------------------
/// ensure that normal vector at various points on the cylinder are as expected
TEST_CASE("scenario: normal vector on the cylinder")
{
        auto const the_cylinder = std::make_shared<RT::cylinder>();
        struct {
                RT::tuple pt;
                RT::tuple exp_normal;
        } const all_tc[] = {
                {RT::create_point(1.0, 0.0, 0.0), RT::create_vector(1.0, 0.0, 0.0)},
                {RT::create_point(0.0, 5.0, -1.0), RT::create_vector(0.0, 0.0, -1.0)},
                {RT::create_point(0.0, -2.0, 1.0), RT::create_vector(0.0, 0.0, 1.0)},
                {RT::create_point(-1.0, 1.0, 0.0), RT::create_vector(-1.0, 0.0, 0.0)},
        };

        for (auto const& tc : all_tc) {
                auto const got_normal = the_cylinder->normal_at(tc.pt);
                CHECK(got_normal == tc.exp_normal);
        }
}

/// ----------------------------------------------------------------------------
/// ensure that ray + uncapped-constrained-cylinder intersections are good
TEST_CASE("scenario: a ray hits the cylinder")
{
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                uint32_t exp_num_xs;
        } const all_tc[] = {

                /// [0] : cast ray diagonally from within the cylinder
                {
                        RT::create_point(0.0, 1.5, 0.0),
                        RT::create_vector(0.1, 1.0, 0.0),
                        0,
                },

                /// [1] : cast ray perpendicular to y-axis but from above the
                /// cylinder
                {
                        RT::create_point(0.0, 3.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        0,
                },

                /// [2] : cast ray perpendicular to y-axis but from below the
                /// cylinder
                {
                        RT::create_point(0.0, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        0,
                },

                /// [3] : edge case => max value is out-of-bounds
                {
                        RT::create_point(0.0, 2.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        0,
                },

                /// [4] : edge case => min value is out-of-bounds
                {
                        RT::create_point(0.0, 1.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        0,
                },

                /// [5] : perpendicular through middle of cylinder causes 2
                /// intersections
                {
                        RT::create_point(0.0, 1.5, -2.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        2,
                },

        };

        auto const the_cylinder = std::make_shared<RT::cylinder>(false,    /// cast-shadow ?
                                                                 1.0, 2.0, /// y min, max
                                                                 false);   /// capped ?

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs = r.intersect(the_cylinder);

                /// ------------------------------------------------------------
                /// no intersections ==> 0 ok ?
                auto got_num_xs = (got_xs.has_value() ? got_xs.value().size() : 0);
                CHECK(got_num_xs == tc.exp_num_xs);
        }
}

/// ----------------------------------------------------------------------------
/// ensure that by default a cylinder is uncapped or open
TEST_CASE("scenario: normal vector on the cylinder")
{
        auto const the_cylinder = std::make_shared<RT::cylinder>();
        CHECK(the_cylinder->capped == false);
}

/// ----------------------------------------------------------------------------
/// ensure that ray + constrained-cylinder intersections are good
TEST_CASE("scenario: intersecting caps of a closed cylinder")
{
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                uint32_t exp_num_xs;
        } const all_tc[] = {

                /// [0] : a ray above a cylinder pointing down through its
                /// middle
                {
                        RT::create_point(0.0, 3.0, 0.0),
                        RT::create_vector(0.0, -1.0, 0.0),
                        2,
                },

                /// [1] : ray above cylinder and intersecting the cap and the
                /// wall
                {
                        RT::create_point(0.0, 3.0, -2.0),
                        RT::create_vector(0.0, -1.0, 2.0),
                        2,
                },

                /// [2] : ray originating above the cylinder and intersecting
                /// the end cap, and emerging where other end cap intersects the
                /// side of the cylinder
                {
                        RT::create_point(0.0, 4.0, -2.0),
                        RT::create_vector(0.0, -1.0, 1.0),
                        2,
                },

                /// [3] : ray below cylinder and intersecting the cap and the
                /// wall
                {
                        RT::create_point(0.0, 0.0, -2.0),
                        RT::create_vector(0.0, 1.0, 2.0),
                        2,
                },

                /// [4] : ray originating below the cylinder and intersecting
                /// the end cap, and emerging where other end cap intersects the
                /// side of the cylinder
                {
                        RT::create_point(0.0, -1.0, -2.0),
                        RT::create_vector(0.0, 1.0, 1.0),
                        2,
                },

                /// ------------------------------------------------------------
                /// next two tests [5], [6] are from:
                ///    https://forum.raytracerchallenge.com/post/448/thread

                /// [5]
                ///
                {
                        RT::create_point(0.0, 0.5, 0.999),
                        RT::create_vector(0.0, 1.0, 0.0),
                        2,
                },

                /// [6]
                {
                        RT::create_point(0.0, 0.5, 1.001),
                        RT::create_vector(0.0, 1, 0),
                        0,
                },

        };

        auto const the_cylinder = std::make_shared<RT::cylinder>(false, /// no shadow
                                                                 1.0,   /// y-min
                                                                 2.0,   /// y-max
                                                                 true); /// capped

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs = r.intersect(the_cylinder);

                /// ------------------------------------------------------------
                /// no intersections ==> 0 ok ?
                auto got_num_xs = (got_xs.has_value() ? got_xs.value().size() : 0);
                CHECK(got_num_xs == tc.exp_num_xs);
        }
}

/// ----------------------------------------------------------------------------
/// ensure that normal vector at cylinders caps are all ok.
TEST_CASE("scenario: normal vector cylinder's end caps")
{
        struct {
                RT::tuple pt;
                RT::tuple exp_normal;
        } const all_tc[] = {
                {RT::create_point(0.0, 1.0, 0.0), RT::create_vector(0.0, -1.0, 0.0)},
                {RT::create_point(0.5, 1.0, 0.0), RT::create_vector(0.0, -1.0, 0.0)},
                {RT::create_point(0.0, 1.0, 0.5), RT::create_vector(0.0, -1.0, 0.0)},
                {RT::create_point(0.0, 2.0, 0.0), RT::create_vector(0.0, 1.0, 0.0)},
                {RT::create_point(0.5, 2.0, 0.0), RT::create_vector(0.0, 1.0, 0.0)},
                {RT::create_point(0.0, 2.0, 0.5), RT::create_vector(0.0, 1.0, 0.0)},
        };

        auto const the_cylinder = std::make_shared<RT::cylinder>(false,    /// cast-shadow ?
                                                                 1.0, 2.0, /// y min, max
                                                                 true);    /// capped ?

        for (auto const& tc : all_tc) {
                auto const got_normal = the_cylinder->normal_at(tc.pt);
                CHECK(got_normal == tc.exp_normal);
        }
}
