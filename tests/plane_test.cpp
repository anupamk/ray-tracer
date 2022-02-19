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
#include "constants.hpp"
#include "intersection_record.hpp"
#include "common/include/logging.h"
#include "material.hpp"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "plane.hpp"
#include "ray.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("plane::plane(...) test")
{
        auto const the_xz_plane = std::make_shared<RT::plane>();

        struct {
                RT::tuple xz_point;
                RT::tuple exp_xz_normal;
        } const all_tc[] = {
                /// [0]
                {
                        RT::create_point(0.0, 0.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },

                /// [1]
                {
                        RT::create_point(10.0, 0.0, -10.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },

                /// [2]
                {
                        RT::create_point(-5.0, 0.0, 150.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                },
        };

        for (auto tc : all_tc) {
                auto got_xz_normal = the_xz_plane->normal_at_local(tc.xz_point);
                CHECK(got_xz_normal == tc.exp_xz_normal);
        }
}

TEST_CASE("plane::intersect(...) test")
{
        auto const the_xz_plane = std::make_shared<RT::plane>();

        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                std::optional<RT::intersection_records> expected_xs;
        } const all_tc[] = {
                /// [0] : ray is parallel to the plane
                {
                        RT::create_point(0.0, 10.0, 0.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        {},
                },

                /// [1] : ray is coplanar
                {
                        RT::create_point(0.0, 0.0, 0.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        {},
                },

                /// [2] : ray intersects the plane from above
                {
                        RT::create_point(0.0, 1.0, 0.0),
                        RT::create_vector(0.0, -1.0, 0.0),
                        RT::create_intersections(RT::intersection_record(1.0, the_xz_plane)),
                },

                /// [3] : ray intersects the plane from below
                {
                        RT::create_point(0.0, -1.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                        RT::create_intersections(RT::intersection_record(1.0, the_xz_plane)),
                },
        };

        for (auto tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, tc.ray_direction);
                auto const got_xs = r.intersect(the_xz_plane);

                CHECK(got_xs == tc.expected_xs);
        }
}
