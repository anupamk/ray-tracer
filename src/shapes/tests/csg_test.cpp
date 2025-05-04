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
#include "primitives/intersection_record.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/csg.hpp"
#include "shapes/cube.hpp"
#include "shapes/shape_interface.hpp"
#include "shapes/sphere.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("csg is created with an operation and two shapes")
{
        auto s1 = std::make_shared<RT::sphere>();
        auto s2 = std::make_shared<RT::cube>();

        auto csg_op = std::make_shared<RT::csg_union>();
        auto csg1   = RT::csg_shape::create_csg(s1, csg_op, s2);

        CHECK(csg1->operation()->stringify() == "union");
        CHECK(csg1->left() == s1);
        CHECK(csg1->right() == s2);
        CHECK(csg1->left()->get_parent() == csg1);
        CHECK(csg1->right()->get_parent() == csg1);
}

TEST_CASE("evaluating csg-union")
{
        auto csg_union_op = std::make_shared<RT::csg_union>();

        CHECK(csg_union_op->intersection_allowed(true, true, true) == false);
        CHECK(csg_union_op->intersection_allowed(true, true, false) == true);
        CHECK(csg_union_op->intersection_allowed(true, false, true) == false);
        CHECK(csg_union_op->intersection_allowed(true, false, false) == true);
        CHECK(csg_union_op->intersection_allowed(false, true, true) == false);
        CHECK(csg_union_op->intersection_allowed(false, true, false) == false);
        CHECK(csg_union_op->intersection_allowed(false, false, true) == true);
        CHECK(csg_union_op->intersection_allowed(false, false, false) == true);
}

TEST_CASE("evaluating csg-intersection")
{
        auto csg_union_op = std::make_shared<RT::csg_intersection>();

        CHECK(csg_union_op->intersection_allowed(true, true, true) == true);
        CHECK(csg_union_op->intersection_allowed(true, true, false) == false);
        CHECK(csg_union_op->intersection_allowed(true, false, true) == true);
        CHECK(csg_union_op->intersection_allowed(true, false, false) == false);
        CHECK(csg_union_op->intersection_allowed(false, true, true) == true);
        CHECK(csg_union_op->intersection_allowed(false, true, false) == true);
        CHECK(csg_union_op->intersection_allowed(false, false, true) == false);
        CHECK(csg_union_op->intersection_allowed(false, false, false) == false);
}

TEST_CASE("evaluating csg-difference")
{
        auto csg_union_op = std::make_shared<RT::csg_difference>();

        CHECK(csg_union_op->intersection_allowed(true, true, true) == false);
        CHECK(csg_union_op->intersection_allowed(true, true, false) == true);
        CHECK(csg_union_op->intersection_allowed(true, false, true) == false);
        CHECK(csg_union_op->intersection_allowed(true, false, false) == true);
        CHECK(csg_union_op->intersection_allowed(false, true, true) == true);
        CHECK(csg_union_op->intersection_allowed(false, true, false) == true);
        CHECK(csg_union_op->intersection_allowed(false, false, true) == false);
        CHECK(csg_union_op->intersection_allowed(false, false, false) == false);
}

TEST_CASE("filtering a list of intersections")
{
        auto s1 = std::make_shared<RT::sphere>();
        auto s2 = std::make_shared<RT::cube>();

        auto xs_list = RT::intersection_records{RT::intersection_record(1.0, s1),  /// 1
                                                RT::intersection_record(2.0, s2),  /// 2
                                                RT::intersection_record(3.0, s1),  /// 3
                                                RT::intersection_record(4.0, s2)}; /// 4

        /// --------------------------------------------------------------------
        /// union operation
        {
                auto csg_op = std::make_shared<RT::csg_union>();
                auto csg1   = RT::csg_shape::create_csg(s1, csg_op, s2);

                auto filtered_xs_list = csg1->filter_intersections(xs_list);
                CHECK(filtered_xs_list.has_value() == true);
                auto filtered_xs_list_val = filtered_xs_list.value();

                CHECK(filtered_xs_list_val.size() == 2);
                CHECK(filtered_xs_list_val[0].where() == 1);
                CHECK(filtered_xs_list_val[1].where() == 4);
        }

        /// --------------------------------------------------------------------
        /// intersection operation
        {
                auto csg_op = std::make_shared<RT::csg_intersection>();
                auto csg1   = RT::csg_shape::create_csg(s1, csg_op, s2);

                auto filtered_xs_list = csg1->filter_intersections(xs_list);
                CHECK(filtered_xs_list.has_value() == true);
                auto filtered_xs_list_val = filtered_xs_list.value();

                CHECK(filtered_xs_list_val.size() == 2);
                CHECK(filtered_xs_list_val[0].where() == 2);
                CHECK(filtered_xs_list_val[1].where() == 3);
        }

        /// --------------------------------------------------------------------
        /// difference operation
        {
                auto csg_op = std::make_shared<RT::csg_difference>();
                auto csg1   = RT::csg_shape::create_csg(s1, csg_op, s2);

                auto filtered_xs_list = csg1->filter_intersections(xs_list);
                CHECK(filtered_xs_list.has_value() == true);
                auto filtered_xs_list_val = filtered_xs_list.value();

                CHECK(filtered_xs_list_val.size() == 2);
                CHECK(filtered_xs_list_val[0].where() == 1);
                CHECK(filtered_xs_list_val[1].where() == 2);
        }
}

TEST_CASE("ray misses a csg object")
{
        auto csg1 = RT::csg_shape::create_csg(std::make_shared<RT::sphere>(),    /// left-shape
                                              std::make_shared<RT::csg_union>(), /// operation
                                              std::make_shared<RT::cube>());     /// right-shape

        auto new_ray = RT::ray_t(RT::create_point(0.0, 2.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
        auto csg_xs  = new_ray.intersect(csg1);

        CHECK(csg_xs.has_value() == false);
}

TEST_CASE("ray hits a csg object")
{
        auto s1 = std::make_shared<RT::sphere>();
        auto s2 = std::make_shared<RT::sphere>();
        s2->transform(RT::matrix_transformations_t::create_3d_translation_matrix(0.0, 0.0, 0.5));

        auto csg1 = RT::csg_shape::create_csg(s1, std::make_shared<RT::csg_union>(), s2);

        auto new_ray = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
        auto csg_xs  = new_ray.intersect(csg1);

        CHECK(csg_xs.has_value() == true);

        auto csg_xs_val = csg_xs.value();

        std::for_each(csg_xs_val.begin(), /// start
                      csg_xs_val.end(),   /// end
                      [](auto const& csg_xs_elem) { INFO("%s\n", csg_xs_elem.stringify().c_str()); });

        CHECK(csg_xs_val.size() == 2);
}

TEST_CASE("Intersecting ray+csg doesn't test children if box is missed")
{
        auto csg_1 = RT::csg_shape::create_csg(std::make_shared<RT::sphere>(),         /// left-shape
                                               std::make_shared<RT::csg_difference>(), /// operation
                                               std::make_shared<RT::sphere>());        /// right-shape

        auto new_ray = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 1.0, 0.0));
        auto csg_xs  = new_ray.intersect(csg_1);

        CHECK(csg_xs.has_value() == false);
}

TEST_CASE("Intersecting ray+csg tests children if box is hit")
{
        auto csg_1 = RT::csg_shape::create_csg(std::make_shared<RT::sphere>(),         /// left-shape
                                               std::make_shared<RT::csg_difference>(), /// operation
                                               std::make_shared<RT::sphere>());        /// right-shape

        auto new_ray = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
        auto csg_xs  = new_ray.intersect(csg_1);

        CHECK(csg_xs.has_value() == true);
}
