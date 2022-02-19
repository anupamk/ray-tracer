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
#include "align_check_pattern.hpp"
#include "common/include/logging.h"
#include "cone.hpp"
#include "constants.hpp"
#include "intersection_record.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "ray.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// ensure that ray intersects a cone
TEST_CASE("scenario: intersecting a cone with a ray")
{
        auto const the_cone = std::make_shared<RT::cone>();

        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                std::optional<RT::intersection_records> expected_xs;
        } const all_tc[] = {

                /// [0]
                {
                        RT::create_point(0.0, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        RT::create_intersections(RT::intersection_record(5.0, the_cone),
                                                 RT::intersection_record(5.0, the_cone)),
                },

                /// [1]
                {
                        RT::create_point(0.0, 0.0, -4.9999),
                        RT::create_vector(1.0, 1.0, 1.0),
                        RT::create_intersections(RT::intersection_record(8.66008, the_cone),
                                                 RT::intersection_record(8.66008, the_cone)),
                },

                /// [2]
                {
                        RT::create_point(1.0, 1.0, -5.0),
                        RT::create_vector(-0.5, -1.0, 1.0),
                        RT::create_intersections(RT::intersection_record(4.55006, the_cone),
                                                 RT::intersection_record(49.44994, the_cone)),
                },
        };

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs = r.intersect(the_cone);

                CHECK(got_xs.has_value() == true);
                CHECK(got_xs.value().size() == tc.expected_xs.value().size());
                CHECK(got_xs.value() == tc.expected_xs.value());
        }
}

/// ----------------------------------------------------------------------------
/// ensure that ray (parallel to one of the halves) intersects the ray
TEST_CASE("scenario: intersecting a cone with a ray parllel to one of its halves")
{
        auto const the_cone        = std::make_shared<RT::cone>();
        RT::tuple const ray_origin = RT::create_point(0.0, 0.0, -1.0);
        RT::tuple const ray_dir    = RT::create_vector(0.0, 1.0, 1.0);

        auto const r      = RT::ray_t(ray_origin, normalize(ray_dir));
        auto const got_xs = r.intersect(the_cone);

        CHECK(got_xs.has_value() == true);
        CHECK(got_xs.value().size() == 1);
        CHECK(RT::epsilon_equal(got_xs.value()[0].where(), 0.35355));
}

/// ----------------------------------------------------------------------------
/// intersecting a cone's end caps
TEST_CASE("scenario: intersecting a cone's end caps")
{
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                uint32_t num_of_intersections;
        } const all_tc[] = {

                /// [0]
                {
                        RT::create_point(0.0, 0.0, -5.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                        0,
                },

                /// [1]
                {
                        RT::create_point(0.0, 0.0, -0.25),
                        RT::create_vector(0.0, 1.0, 1.0),
                        2,
                },

                /// [2]
                {
                        RT::create_point(0.0, 0.0, -0.25),
                        RT::create_vector(0.0, 1.0, 0.0),
                        4,
                },
        };

        auto const the_cone = std::make_shared<RT::cone>(false, /// cast-shadow
                                                         -0.5,  /// y-min
                                                         0.5,   /// y-max
                                                         true); /// capped ?

        for (auto const& tc : all_tc) {
                auto const r            = RT::ray_t(tc.ray_origin, normalize(tc.ray_direction));
                auto const got_xs       = r.intersect(the_cone);
                auto const got_xs_count = got_xs ? got_xs.value().size() : 0;

                CHECK(got_xs_count == tc.num_of_intersections);
        }
}

/// ----------------------------------------------------------------------------
/// computing the normal vector on a cone
TEST_CASE("scenario: computing the normal vector on a cone")
{
        struct {
                RT::tuple point;
                RT::tuple exp_normal_at_point;
        } const all_tc[] = {

                /// [0]
                {
                        RT::create_point(0.0, 0.0, 0.0),
                        RT::create_vector(0.0, 0.0, 0.0),
                },

                /// [1]
                {
                        RT::create_point(1.0, 1.0, 1.0),
                        RT::create_vector(1.0, -RT::SQRT_2, 1.0),
                },

                /// [2]
                {
                        RT::create_point(-1.0, -1.0, 0.0),
                        RT::create_vector(-1.0, 1.0, 0.0),
                },
        };

        auto const the_cone = std::make_shared<RT::cone>();

        for (auto const& tc : all_tc) {
                auto const got_normal = the_cone->normal_at_local(tc.point);
                CHECK(got_normal == tc.exp_normal_at_point);
        }
}
