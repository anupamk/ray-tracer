/// c++ includes
#include <stdexcept>

/// 3rdparty testing library
#include "doctest.h"

/// our own thing
#include "ray.hpp"
#include "tuple.hpp"

/// ray creation test
TEST_CASE("testing ray creation")
{
        auto const r_origin    = raytracer::create_point(1.0, 2.0, 3.0);
        auto const r_direction = raytracer::create_vector(4.0, 5.0, 6.0);
        auto const r	       = raytracer::ray_t(r_origin, r_direction);

        CHECK(r.origin() == r_origin);
        CHECK(r.direction() == r_direction);
}

/// position on ray test
TEST_CASE("position on ray test")
{
        struct {
                double pos_;
                raytracer::tuple exp_pt_;
        } const tc_list[] = {
                // clang-format off
                { 0.0  , raytracer::create_point(2.0, 3.0, 4.0) },
                { 1.0  , raytracer::create_point(3.0, 3.0, 4.0) },
                { -1.0 , raytracer::create_point(1.0, 3.0, 4.0) },
                { 2.5  , raytracer::create_point(4.5, 3.0, 4.0) },
                // clang-format on
        };

        auto const r_origin    = raytracer::create_point(2.0, 3.0, 4.0);
        auto const r_direction = raytracer::create_vector(1.0, 0.0, 0.0);
        auto const r	       = raytracer::ray_t(r_origin, r_direction);

        for (auto const tc : tc_list) {
                auto const got_pt = r.position(tc.pos_);
                CHECK(got_pt == tc.exp_pt_);
        }
}
