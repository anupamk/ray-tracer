/// c++ includes
#include <memory>

#include "common/include/logging.h"
#include "doctest/doctest.h"
#include "primitives/intersection_info.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/sphere.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// ray creation test
TEST_CASE("testing ray creation")
{
        auto const r_origin    = RT::create_point(1.0, 2.0, 3.0);
        auto const r_direction = RT::create_vector(4.0, 5.0, 6.0);
        auto const r           = RT::ray_t(r_origin, r_direction);

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
        auto const r           = RT::ray_t(r_origin, r_direction);

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
        auto const r           = RT::ray_t(r_origin, r_direction);

        /// create sphere and setup the intersection
        auto const the_sphere = std::make_shared<RT::sphere>();
        auto const xs_01      = RT::intersection_record(4.0, the_sphere);
        auto const xs_list    = RT::intersection_records{xs_01};

        /// --------------------------------------------------------------------
        auto const xs_info = r.prepare_computations(xs_list);

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
        auto const r           = RT::ray_t(r_origin, r_direction);

        /// create sphere and setup the intersection
        auto const the_sphere = std::make_shared<RT::sphere>();
        auto const xs_01      = RT::intersection_record(1.0, the_sphere);
        auto const xs_list    = RT::intersection_records{xs_01};

        /// --------------------------------------------------------------------
        auto const xs_info = r.prepare_computations(xs_list);

        /// --------------------------------------------------------------------
        /// validate
        CHECK(xs_info.what_object() == xs_01.what_object());
        CHECK(xs_info.point() == xs_01.where());
        CHECK(xs_info.position() == RT::create_point(0.0, 0.0, 1.0));
        CHECK(xs_info.eye_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.normal_vector() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(xs_info.inside() == true);
}

/// ----------------------------------------------------------------------------
/// hit should offset the point
TEST_CASE("ray::prepare_computations(...) test")
{
        /// create the ray
        auto const r_origin    = RT::create_point(0.0, 0.0, -5.0);
        auto const r_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const r           = RT::ray_t(r_origin, r_direction);

        /// create sphere and setup the intersection
        auto the_sphere = std::make_shared<RT::sphere>();
        the_sphere->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 1.0));

        /// an intersection record
        auto const xs_01   = RT::intersection_record(5.0, the_sphere);
        auto const xs_list = RT::intersection_records{xs_01};

        /// check compute point of intersection
        auto const comps = r.prepare_computations(xs_list);

        CHECK(comps.over_position().z() < -RT::EPSILON / 2.0);
        CHECK(comps.position().z() > comps.over_position().z());
}
