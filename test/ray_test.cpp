/// c++ includes
#include <stdexcept>

/// 3rdparty testing library
#include "doctest.h"

/// our own thing
#include "ray.hpp"
#include "tuple.hpp"
#include "intersection_record.hpp"
#include "sphere.hpp"

/// convenience
namespace RT = raytracer;

/// ray creation test
TEST_CASE("testing ray creation")
{
        auto const r_origin    = RT::create_point(1.0, 2.0, 3.0);
        auto const r_direction = RT::create_vector(4.0, 5.0, 6.0);
        auto const r	       = RT::ray_t(r_origin, r_direction);

        CHECK(r.origin() == r_origin);
        CHECK(r.direction() == r_direction);
}

/// position on ray test
TEST_CASE("position on ray test")
{
        struct {
                double pos_;
                RT::tuple exp_pt_;
        } const tc_list[] = {
                // clang-format off
                { 0.0  , RT::create_point(2.0, 3.0, 4.0) },
                { 1.0  , RT::create_point(3.0, 3.0, 4.0) },
                { -1.0 , RT::create_point(1.0, 3.0, 4.0) },
                { 2.5  , RT::create_point(4.5, 3.0, 4.0) },
                // clang-format on
        };

        auto const r_origin    = RT::create_point(2.0, 3.0, 4.0);
        auto const r_direction = RT::create_vector(1.0, 0.0, 0.0);
        auto const r	       = RT::ray_t(r_origin, r_direction);

        for (auto const tc : tc_list) {
                auto const got_pt = r.position(tc.pos_);
                CHECK(got_pt == tc.exp_pt_);
        }
}

/// ----------------------------------------------------------------------------
/// with intersection happening outside
TEST_CASE("ray::prepare_computations(...) test")
{
        /// create the ray
        auto const r_origin    = RT::create_point(0.0, 0.0, -5.0);
        auto const r_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const r	       = RT::ray_t(r_origin, r_direction);

        /// create sphere and setup the intersection
        auto const the_sphere  = std::make_shared<RT::sphere>();
        auto const xs_01       = RT::intersection_record(4.0, the_sphere);

        /// --------------------------------------------------------------------
        auto const xs_info = r.prepare_computations(xs_01);

        /// --------------------------------------------------------------------
        /// validate
        CHECK(xs_info.what_object() == xs_01.what_object());
        CHECK(xs_info.point() == xs_01.where());
        CHECK(xs_info.position() == RT::create_point(0.0, 0.0, -1.0));
        CHECK(xs_info.eye_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.normal_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.inside() == false);
}

/// ----------------------------------------------------------------------------
/// with intersection happening inside
TEST_CASE("ray::prepare_computations(...) test")
{
        /// create the ray
        auto const r_origin    = RT::create_point(0.0, 0.0, 0.0);
        auto const r_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const r	       = RT::ray_t(r_origin, r_direction);

        /// create sphere and setup the intersection
        auto const the_sphere  = std::make_shared<RT::sphere>();
        auto const xs_01       = RT::intersection_record(1.0, the_sphere);

        /// --------------------------------------------------------------------
        auto const xs_info = r.prepare_computations(xs_01);

        /// --------------------------------------------------------------------
        /// validate
        CHECK(xs_info.what_object() == xs_01.what_object());
        CHECK(xs_info.point() == xs_01.where());
        CHECK(xs_info.position() == RT::create_point(0.0, 0.0, 1.0));
        CHECK(xs_info.eye_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.normal_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.inside() == true);
}
