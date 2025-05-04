/// c++ includes
#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "shapes/aabb.hpp"
#include "shapes/csg.hpp"
#include "shapes/cylinder.hpp"
#include "shapes/group.hpp"
#include "shapes/sphere.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT             = raytracer;
using rt_matrix_xforms_t = raytracer::matrix_transformations_t;

TEST_CASE("aabb:create an empty bounding box")
{
        const auto empty_bb = RT::aabb();

        /*
         * we are resorting to 'by-hand' equality comparison because, default
         * tuple comparison is an 'epsilon' equality comparison.
         *
         * to make that work, we need to 'operate' on infinities. which is
         * generally not a good idea.
         *
         * so...here we are.
         **/
        const auto pt_at_pos_infinity = RT::point_at_positive_infinity();
        CHECK(empty_bb.min().x() == pt_at_pos_infinity.x());
        CHECK(empty_bb.min().y() == pt_at_pos_infinity.y());
        CHECK(empty_bb.min().z() == pt_at_pos_infinity.z());

        const auto pt_at_neg_infinity = RT::point_at_negative_infinity();
        CHECK(empty_bb.max().x() == pt_at_neg_infinity.x());
        CHECK(empty_bb.max().y() == pt_at_neg_infinity.y());
        CHECK(empty_bb.max().z() == pt_at_neg_infinity.z());
}

TEST_CASE("aabb:create bounding box with volume")
{
        const auto min_pt = RT::create_point(-1, -2, -3);
        const auto max_pt = RT::create_point(3, 2, 1);
        const auto a_bb   = RT::aabb(min_pt, max_pt);

        CHECK(a_bb.min() == min_pt);
        CHECK(a_bb.max() == max_pt);
}

TEST_CASE("aabb:create bounding box with volume")
{
        const auto min_pt = RT::create_point(-1, -2, -3);
        const auto max_pt = RT::create_point(3, 2, 1);
        const auto a_bb   = RT::aabb(min_pt, max_pt);

        CHECK(a_bb.min() == min_pt);
        CHECK(a_bb.max() == max_pt);
}

TEST_CASE("aabb:adding points to an empty bounding box")
{
        auto empty_bb   = RT::aabb();
        const auto pt_1 = RT::create_point(-5, 2, 0);
        const auto pt_2 = RT::create_point(7, 0, -3);

        /// --------------------------------------------------------------------
        /// add the points 'pt_1' and 'pt_2' to the empty box, and see if things
        /// are OK.
        empty_bb.add_point(pt_1);
        empty_bb.add_point(pt_2);

        const auto exp_min_pt = RT::create_point(-5, 0, -3);
        const auto exp_max_pt = RT::create_point(7, 2, 0);

        CHECK(empty_bb.min() == exp_min_pt);
        CHECK(empty_bb.max() == exp_max_pt);
}

TEST_CASE("aabb:checking to see if a box contains a given point")
{
        auto const a_box = RT::aabb{RT::create_point(5, -2, 0), RT::create_point(11, 4, 7)};

        struct {
                RT::tuple pt;
                bool exp_result;
        } const all_tc[] = {

                /// [0]
                {
                        RT::create_point(5, -2, 0),
                        true,
                },

                /// [1]
                {
                        RT::create_point(11, 4, 7),
                        true,
                },

                /// [2]
                {
                        RT::create_point(8, 1, 3),
                        true,
                },

                /// [3]
                {
                        RT::create_point(3, 0, 3),
                        false,
                },

                /// [4]
                {
                        RT::create_point(8, -4, 3),
                        false,
                },

                /// [5]
                {
                        RT::create_point(8, 1, -1),
                        false,
                },

                /// [6]
                {
                        RT::create_point(13, 1, 3),
                        false,
                },

                /// [7]
                {
                        RT::create_point(8, 5, 3),
                        false,
                },

                /// [8]
                {
                        RT::create_point(8, 1, 8),
                        false,
                },
        };

        for (auto tc : all_tc) {
                auto got_result = a_box.contains(tc.pt);
                CHECK(tc.exp_result == got_result);
        }
}

TEST_CASE("aabb:checking to see if a box contains a given box")
{
        auto const a_box = RT::aabb{RT::create_point(5, -2, 0), RT::create_point(11, 4, 7)};

        struct {
                RT::aabb other_box;
                bool exp_result;
        } const all_tc[] = {

                /// [0]
                {
                        RT::aabb(RT::create_point(5, -2, 0), RT::create_point(11, 4, 7)),
                        true,
                },

                /// [1]
                {
                        RT::aabb(RT::create_point(6, -1, 1), RT::create_point(10, 3, 6)),
                        true,
                },

                /// [2]
                {
                        RT::aabb(RT::create_point(4, -3, -1), RT::create_point(10, 3, 6)),
                        false,
                },

                /// [3]
                {
                        RT::aabb(RT::create_point(6, -1, 1), RT::create_point(12, 5, 8)),
                        false,
                },
        };

        for (auto tc : all_tc) {
                auto got_result = a_box.contains(tc.other_box);
                CHECK(tc.exp_result == got_result);
        }
}

TEST_CASE("aabb:transforming a bounding box")
{
        auto const a_box       = RT::aabb{RT::create_point(-1, -1, -1), RT::create_point(1, 1, 1)};
        auto const xform_mat   = (rt_matrix_xforms_t::create_rotx_matrix(RT::PI_BY_4F) *
                                rt_matrix_xforms_t::create_roty_matrix(RT::PI_BY_4F));
        auto const xformed_box = a_box.transform(xform_mat);

        auto exp_xform_box_min = RT::create_point(-1.414214, -1.707107, -1.707107);
        auto exp_xform_box_max = RT::create_point(1.414214, 1.707107, 1.707107);

        CHECK(xformed_box.min() == exp_xform_box_min);
        CHECK(xformed_box.max() == exp_xform_box_max);
}

TEST_CASE("aabb:a group has a bounding box that contains its children")
{
        auto g_1_s_1 = std::make_shared<RT::sphere>();
        g_1_s_1->transform(rt_matrix_xforms_t::create_3d_translation_matrix(2.0, 5.0, -3.0) *
                           rt_matrix_xforms_t::create_3d_scaling_matrix(2.0, 2.0, 2.0));

        auto g_1_s_2 = std::make_shared<RT::cylinder>(true,   /// cast-shadow ?
                                                      -2.0,   /// min
                                                      2.0,    /// max
                                                      false); /// capped ?
        g_1_s_2->transform(rt_matrix_xforms_t::create_3d_translation_matrix(-4.0, -1.0, 4.0) *
                           rt_matrix_xforms_t::create_3d_scaling_matrix(0.5, 1.0, 0.5));

        /// --------------------------------------------------------------------
        /// add both the shapes to a group
        auto g_1 = std::make_shared<RT::group>();
        g_1->add_child(g_1_s_1);
        g_1->add_child(g_1_s_2);

        auto const g_1_aabb      = g_1->bounds_of();
        auto g_1_exp_aabb_min_pt = RT::create_point(-4.5, -3.0, -5.0);
        auto g_1_exp_aabb_max_pt = RT::create_point(4.0, 7.0, 4.5);

        CHECK(g_1_aabb.min() == g_1_exp_aabb_min_pt);
        CHECK(g_1_aabb.max() == g_1_exp_aabb_max_pt);
}

TEST_CASE("aabb: a CSG shape has a bounding box that contains its children")
{
        auto left_shape  = std::make_shared<RT::sphere>();
        auto right_shape = std::make_shared<RT::sphere>();
        right_shape->transform(rt_matrix_xforms_t::create_3d_translation_matrix(2.0, 3.0, 4.0));

        auto csg_shape =
                RT::csg_shape::create_csg(left_shape, std::make_shared<RT::csg_difference>(), right_shape);

        auto const csg_shape_aabb      = csg_shape->bounds_of();
        auto csg_shape_exp_aabb_min_pt = RT::create_point(-1.0, -1.0, -1.0);
        auto csg_shape_exp_aabb_max_pt = RT::create_point(3.0, 4.0, 5.0);

        CHECK(csg_shape_aabb.min() == csg_shape_exp_aabb_min_pt);
        CHECK(csg_shape_aabb.max() == csg_shape_exp_aabb_max_pt);
}

TEST_CASE("aabb: Intersecting a ray with a bounding box at the origin")
{
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                bool expected_intersection;
        } const all_tc[] = {
                /// [0]
                {
                        RT::create_point(5.0, 0.5, 0.0),
                        RT::create_vector(-1, 0, 0),
                        true,
                },

                /// [1]
                {
                        RT::create_point(-5.0, 0.5, 0.0),
                        RT::create_vector(1, 0, 0),
                        true,
                },

                /// [2]
                {
                        RT::create_point(0.5, 5.0, 0),
                        RT::create_vector(0, -1.0, 0),
                        true,
                },

                /// [3]
                {
                        RT::create_point(0.5, -5.0, 0.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                        true,
                },

                /// [4]
                {
                        RT::create_point(0.5, 0.0, 5.0),
                        RT::create_vector(0.0, 0.0, -1.0),
                        true,
                },

                /// [5]
                {
                        RT::create_point(0.5, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        true,
                },

                /// [6]
                {
                        RT::create_point(0.0, 0.5, 0.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        true,
                },

                /// [7]
                {
                        RT::create_point(-2.0, 0.0, 0.0),
                        RT::create_vector(2.0, 4.0, 6.0),
                        false,
                },

                /// [8]
                {
                        RT::create_point(0.0, -2.0, 0.0),
                        RT::create_vector(6.0, 2.0, 4.0),
                        false,
                },

                /// [9]
                {
                        RT::create_point(0.0, 0.0, -2.0),
                        RT::create_vector(4.0, 6.0, 2.0),
                        false,
                },

                /// [10]
                {
                        RT::create_point(2.0, 0.0, 2.0),
                        RT::create_vector(0.0, 0.0, -1.0),
                        false,
                },

                /// [11]
                {
                        RT::create_point(0.0, 2.0, 2.0),
                        RT::create_vector(0.0, -1.0, 0.0),
                        false,
                },

                /// [12]
                {
                        RT::create_point(2.0, 2.0, 0.0),
                        RT::create_vector(-1.0, 0.0, 0.0),
                        false,
                },
        };

        auto const aabb_1 = RT::aabb(RT::create_point(-1.0, -1.0, -1.0), /// min
                                     RT::create_point(1.0, 1.0, 1.0));   /// max

        for (auto const& tc : all_tc) {
                auto tc_ray           = RT::ray_t(tc.ray_origin, RT::normalize(tc.ray_direction));
                auto got_intersection = aabb_1.intersects(tc_ray);
                CHECK(tc.expected_intersection == got_intersection);
        }
}

TEST_CASE("aabb: Intersecting a ray with a non-cubic bounding box")
{
        struct {
                RT::tuple ray_origin;
                RT::tuple ray_direction;
                bool expected_intersection;
        } const all_tc[] = {
                /// [0]
                {
                        RT::create_point(15.0, 1.0, 2.0),
                        RT::create_vector(-1, 0, 0),
                        true,
                },

                /// [1]
                {
                        RT::create_point(-5.0, -1.0, 4.0),
                        RT::create_vector(1, 0, 0),
                        true,
                },

                /// [2]
                {
                        RT::create_point(7.0, 6.0, 5.0),
                        RT::create_vector(0, -1.0, 0),
                        true,
                },

                /// [3]
                {
                        RT::create_point(9.0, -5.0, 6.0),
                        RT::create_vector(0.0, 1.0, 0.0),
                        true,
                },

                /// [4]
                {
                        RT::create_point(8.0, 2.0, 12.0),
                        RT::create_vector(0.0, 0.0, -1.0),
                        true,
                },

                /// [5]
                {
                        RT::create_point(6.0, 0.0, -5.0),
                        RT::create_vector(0.0, 0.0, 1.0),
                        true,
                },

                /// [6]
                {
                        RT::create_point(8.0, 1.0, 3.5),
                        RT::create_vector(0.0, 0.0, 1.0),
                        true,
                },

                /// [7]
                {
                        RT::create_point(9.0, -1.0, -8.0),
                        RT::create_vector(2.0, 4.0, 6.0),
                        false,
                },

                /// [8]
                {
                        RT::create_point(8.0, 3.0, -4.0),
                        RT::create_vector(6.0, 2.0, 4.0),
                        false,
                },

                /// [9]
                {
                        RT::create_point(9.0, -1.0, -2.0),
                        RT::create_vector(4.0, 6.0, 2.0),
                        false,
                },

                /// [10]
                {
                        RT::create_point(4.0, 0.0, 9.0),
                        RT::create_vector(0.0, 0.0, -1.0),
                        false,
                },

                /// [11]
                {
                        RT::create_point(8.0, 6.0, -1.0),
                        RT::create_vector(0.0, -1.0, 0.0),
                        false,
                },

                /// [12]
                {
                        RT::create_point(12.0, 5.0, 4.0),
                        RT::create_vector(-1.0, 0.0, 0.0),
                        false,
                },

        };

        auto const aabb_1 = RT::aabb(RT::create_point(5.0, -2.0, 0.0),  /// min
                                     RT::create_point(11.0, 4.0, 7.0)); /// max

        for (auto const& tc : all_tc) {
                auto tc_ray           = RT::ray_t(tc.ray_origin, RT::normalize(tc.ray_direction));
                auto got_intersection = aabb_1.intersects(tc_ray);
                CHECK(tc.expected_intersection == got_intersection);
        }
}
