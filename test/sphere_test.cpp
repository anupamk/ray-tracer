/// c++ includes
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "tuple.hpp"
#include <iostream>
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
                auto const r	  = RT::ray_t(tc.ray_origin, tc.ray_direction);
                auto const got_xs = the_sphere->intersect(r);

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
        auto const the_sphere	= std::make_shared<RT::sphere>();
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
                auto const xs_value = the_sphere->intersect(r).value();

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
                auto const xs = the_sphere->intersect(r);

                CHECK(xs.has_value() == false);
        }
}
