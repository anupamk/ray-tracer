/*
 * this file tests the 'camera' class interfaces.
 **/

/// 3rdparty testing library
#include "color.hpp"
#include "doctest.h"

/// our includes
#include "camera.hpp"
#include "constants.hpp"
#include "logging.h"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "ray.hpp"
#include "tuple.hpp"
#include "utils.hpp"
#include "world.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// base camera test
TEST_CASE("camera::camera(...) test")
{
        auto const c_01 = RT::camera(160, 120, RT::PI_BY_2F);

        CHECK(c_01.hsize() == 160);
        CHECK(c_01.vsize() == 120);
        CHECK(c_01.field_of_view() == RT::PI_BY_2F);
        CHECK(c_01.transform() == RT::fsize_dense2d_matrix_t::create_identity_matrix(4));
}

/// ----------------------------------------------------------------------------
/// base camera test
TEST_CASE("camera::pixel_size(...) test")
{
        auto const c_01 = RT::camera(200, 125, RT::PI_BY_2F);

        CHECK(RT::epsilon_equal(c_01.pixel_size(), 0.01));
}

/// ----------------------------------------------------------------------------
/// ray through center of canvas
TEST_CASE("camera::ray_for_pixel(...) test")
{
        auto const c_01 = RT::camera(201, 101, RT::PI_BY_2F);
        auto const r_01 = c_01.ray_for_pixel(100, 50);

        CHECK(r_01.origin() == RT::create_point(0.0, 0.0, 0.0));
        CHECK(r_01.direction() == RT::create_vector(0.0, 0.0, -1.0));
}

/// ----------------------------------------------------------------------------
/// ray through corner of canvas
TEST_CASE("camera::ray_for_pixel(...) test")
{
        auto const c_01 = RT::camera(201, 101, RT::PI_BY_2F);
        auto const r_01 = c_01.ray_for_pixel(0, 0);

        CHECK(r_01.origin() == RT::create_point(0.0, 0.0, 0.0));
        CHECK(r_01.direction() == RT::create_vector(0.66519, 0.33259, -0.66851));
}

/// ----------------------------------------------------------------------------
/// ray when camera is transformed
TEST_CASE("camera::ray_for_pixel(...) test")
{
        auto c_01            = RT::camera(201, 101, RT::PI_BY_2F);
        auto const xform_mat = (RT::matrix_transformations_t::create_roty_matrix(RT::PI_BY_4F) *
                                RT::matrix_transformations_t::create_3d_translation_matrix(0.0, -2.0, 5.0));
        c_01.transform(xform_mat);

        auto const r_01               = c_01.ray_for_pixel(100, 50);
        auto const exp_r_01_origin    = RT::create_point(0.0, 2.0, -5.0);
        auto const exp_r_01_direction = RT::create_vector(RT::SQRT_2_BY_2F, 0.0, -RT::SQRT_2_BY_2F);

        CHECK(r_01.origin() == exp_r_01_origin);
        CHECK(r_01.direction() == exp_r_01_direction);
}

/// ----------------------------------------------------------------------------
/// render a world by camera
TEST_CASE("camera::ray_for_pixel(...) test")
{
        auto w_01       = RT::world::create_default_world();
        auto c_01       = RT::camera(11, 11, RT::PI_BY_2F);
        auto from_point = RT::create_point(0.0, 0.0, -5.0);
        auto to_point   = RT::create_point(0.0, 0.0, 0.0);
        auto up_vector  = RT::create_vector(0.0, 1.0, 0.0);

        c_01.transform(RT::matrix_transformations_t::create_view_transform(from_point, to_point, up_vector));
        auto rendered_img = c_01.render(w_01);

        auto got_color_at_pixel = rendered_img.read_pixel(5, 5);
        auto exp_color_at_pixel = RT::color(0.38066, 0.47583, 0.2855);

        CHECK(got_color_at_pixel == exp_color_at_pixel);
}
