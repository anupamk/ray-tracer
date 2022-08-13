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
#include "constants.hpp"
#include "cube.hpp"
#include "cube_map_texture.hpp"
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
/// ensure that ray-cube intersections are good
TEST_CASE("scenario: a ray intersects the cube")
{
        auto const the_cube = std::make_shared<RT::cube>();
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                std::optional<RT::intersection_records> expected_xs;
        } const all_tc[] = {

                /// [0] : +x
                {
                        RT::create_point(5.0, 0.5, 0.0),
                        RT::create_vector(-1.0, 0.0, 0.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [1] : -x
                {
                        RT::create_point(-5.0, 0.5, 0.0),
                        RT::create_vector(1.0, 0.0, 0.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [2] : +y
                {
                        RT::create_point(0.5, 5.0, 0.0),
                        RT::create_vector(0.0, -1.0, 0.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [3] : -y
                {
                        RT::create_point(0.5, -5.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [4] : +z
                {
                        RT::create_point(0.5, 0.0, 5.0),
                        RT::create_vector(0.0, 0.0, -1.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [5] : -z
                {
                        RT::create_point(0.5, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        RT::create_intersections(RT::intersection_record(4.0, the_cube),
                                                 RT::intersection_record(6.0, the_cube)),
                },

                /// [6] : inside
                {
                        RT::create_point(0.0, 0.5, 0.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        RT::create_intersections(RT::intersection_record(-1.0, the_cube),
                                                 RT::intersection_record(1.0, the_cube)),
                },

        };

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, tc.ray_direction);
                auto const got_xs = r.intersect(the_cube);

                CHECK(got_xs.value().size() == 2);
                CHECK(got_xs == tc.expected_xs);
        }
}

/// ----------------------------------------------------------------------------
/// ray misses a cube
TEST_CASE("scenario: a ray misses the cube")
{
        auto const the_cube = std::make_shared<RT::cube>();
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
        } const all_tc[] = {
                {RT::create_point(-2.0, 0.0, 0.0), RT::create_vector(0.2673, 0.5345, 0.8018)},
                {RT::create_point(0.0, -2.0, 0.0), RT::create_vector(0.8018, 0.2673, 0.5345)},
                {RT::create_point(0.0, 0.0, -2.0), RT::create_vector(0.5345, 0.8018, 0.2673)},
                {RT::create_point(2.0, 0.0, 2.0), RT::create_vector(0.0, 0.0, -1.0)},
                {RT::create_point(0.0, 2.0, 2.0), RT::create_vector(0.0, -1.0, 0.0)},
                {RT::create_point(2.0, 2.0, 0.0), RT::create_vector(-1.0, 0.0, 0.0)},
        };

        for (auto const& tc : all_tc) {
                auto const r      = RT::ray_t(tc.ray_origin, tc.ray_direction);
                auto const got_xs = r.intersect(the_cube);

                CHECK(got_xs.has_value() == false);
        }
}

/// ----------------------------------------------------------------------------
/// normal on the surface of the cube
TEST_CASE("scenario: the normal on the surface of the cube")
{
        auto const the_cube = std::make_shared<RT::cube>();
        struct {
                RT::tuple pt;
                RT::tuple exp_normal;
        } const all_tc[] = {
                {RT::create_point(1.0, 0.5, -0.8), RT::create_vector(1.0, 0.0, 0.0)},
                {RT::create_point(-1.0, -0.2, 0.9), RT::create_vector(-1.0, 0.0, 0.0)},
                {RT::create_point(-0.4, 1.0, -0.1), RT::create_vector(0.0, 1.0, 0.0)},
                {RT::create_point(0.3, -1.0, -0.7), RT::create_vector(0.0, -1.0, 0.0)},
                {RT::create_point(-0.6, 0.3, 1.0), RT::create_vector(0.0, 0.0, 1.0)},
                {RT::create_point(0.4, 0.4, -1.0), RT::create_vector(0.0, 0.0, -1.0)},
                {RT::create_point(1.0, 1.0, 1.0), RT::create_vector(1.0, 0.0, 0.0)},
                {RT::create_point(-1.0, -1.0, -1.0), RT::create_vector(-1.0, 0.0, 0.0)},
        };

        for (auto const& tc : all_tc) {
                auto const got_normal = the_cube->normal_at(tc.pt);

                CHECK(got_normal == tc.exp_normal);
        }
}

/// ----------------------------------------------------------------------------
/// cube texture tests
TEST_CASE("scenario: Finding the colors on a mapped cube")
{
        auto const the_cube = std::make_shared<RT::cube>();

        /// --------------------------------------------------------------------
        /// define colors for the textures and then the textures themselves
        auto const red    = RT::color_red();
        auto const yellow = RT::color(1.0, 1.0, 0.0);
        auto const brown  = RT::color(1.0, 0.5, 0.0);
        auto const green  = RT::color_green();
        auto const cyan   = RT::color(0.0, 1.0, 1.0);
        auto const blue   = RT::color_blue();
        auto const purple = RT::color(1.0, 0.0, 1.0);
        auto const white  = RT::color_white();

        auto const left_face  = std::make_shared<RT::align_check>(yellow, cyan, red, blue, brown);
        auto const front_face = std::make_shared<RT::align_check>(cyan, red, yellow, brown, green);
        auto const right_face = std::make_shared<RT::align_check>(red, yellow, purple, green, white);
        auto const back_face  = std::make_shared<RT::align_check>(green, purple, cyan, white, blue);
        auto const up_face    = std::make_shared<RT::align_check>(brown, cyan, purple, red, yellow);
        auto const down_face  = std::make_shared<RT::align_check>(purple, brown, green, blue, white);

        auto const cube_test_texture = std::make_shared<RT::cube_texture>(left_face, front_face, right_face,
                                                                          back_face, up_face, down_face);

        struct {
                RT::tuple pt;
                RT::color exp_color;
        } const all_tc[] = {
                /// ------------------------------------------------------------
                /// LEFT-FACE
                {RT::create_point(-1, 0, 0), yellow},
                {RT::create_point(-1, 0.9, -0.9), cyan},
                {RT::create_point(-1, 0.9, 0.9), red},
                {RT::create_point(-1, -0.9, -0.9), blue},
                {RT::create_point(-1, -0.9, 0.9), brown},

                /// ------------------------------------------------------------
                /// FRONT-FACE
                {RT::create_point(0, 0, 1), cyan},
                {RT::create_point(-0.9, 0.9, 1), red},
                {RT::create_point(0.9, 0.9, 1), yellow},
                {RT::create_point(-0.9, -0.9, 1), brown},
                {RT::create_point(0.9, -0.9, 1), green},

                /// ------------------------------------------------------------
                /// RIGHT-FACE*
                {RT::create_point(1, 0, 0), red},
                {RT::create_point(1, 0.9, 0.9), yellow},
                {RT::create_point(1, 0.9, -0.9), purple},
                {RT::create_point(1, -0.9, 0.9), green},
                {RT::create_point(1, -0.9, -0.9), white},

                /// ------------------------------------------------------------
                /// BACK-FACE
                {RT::create_point(0, 0, -1), green},
                {RT::create_point(0.9, 0.9, -1), purple},
                {RT::create_point(-0.9, 0.9, -1), cyan},
                {RT::create_point(0.9, -0.9, -1), white},
                {RT::create_point(-0.9, -0.9, -1), blue},

                /// ------------------------------------------------------------
                /// UP-FACE
                {RT::create_point(0, 1, 0), brown},
                {RT::create_point(-0.9, 1, -0.9), cyan},
                {RT::create_point(0.9, 1, -0.9), purple},
                {RT::create_point(-0.9, 1, 0.9), red},
                {RT::create_point(0.9, 1, 0.9), yellow},

                /// ------------------------------------------------------------
                /// DOWN-FACE
                {RT::create_point(0, -1, 0), purple},
                {RT::create_point(-0.9, -1, 0.9), brown},
                {RT::create_point(0.9, -1, 0.9), green},
                {RT::create_point(-0.9, -1, -0.9), blue},
                {RT::create_point(0.9, -1, -0.9), white},
        };

        for (auto const& tc : all_tc) {
                auto const got_color = cube_test_texture->color_at_point(tc.pt);
                CHECK(got_color == tc.exp_color);
        }
}
