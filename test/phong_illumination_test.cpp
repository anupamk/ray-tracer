/*
 * this file tests the phong_illumination(...) api
**/

/// 3rdparty testing library
#include "constants.hpp"
#include "doctest.h"

/// our includes
#include "color.hpp"
#include "tuple.hpp"
#include "material.hpp"
#include "point_light.hpp"
#include "phong_illumination.hpp"

/// convenience
namespace RT = raytracer;

/// all tests run with these
auto const SURFACE_MATERIAL = RT::material();
auto const SURFACE_POINT    = RT::create_point(0.0, 0.0, 0.0);

TEST_CASE("phong_illumination(...) test: light on normal, reflection on normal, viewer on normal")
{
        auto const eye_vector	  = RT::create_vector(0.0, 0.0, -1.0);
        auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
        auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());

        auto const exp_reflected_color = RT::color(1.9, 1.9, 1.9);
        auto const got_reflected_color = RT::phong_illumination(SURFACE_POINT,	  /// where
                                                                SURFACE_MATERIAL, /// material
                                                                incident_light,	  /// incoming light
                                                                eye_vector,	  /// viewwer
                                                                surface_normal);  /// normal

        CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light on normal, reflection on normal, viewer at 45° to normal")
{
        auto const eye_vector	  = RT::create_vector(0.0, RT::SQRT_2_BY_2F, -RT::SQRT_2_BY_2F);
        auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
        auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());

        auto const exp_reflected_color = RT::color(1.0, 1.0, 1.0);
        auto const got_reflected_color = RT::phong_illumination(SURFACE_POINT,	  /// where
                                                                SURFACE_MATERIAL, /// material
                                                                incident_light,	  /// incoming light
                                                                eye_vector,	  /// viewwer
                                                                surface_normal);  /// normal

        CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light at 45° to normal, reflection on normal, viewer on normal")
{
        auto const eye_vector	  = RT::create_vector(0.0, 0.0, -1.0);
        auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
        auto const incident_light = RT::point_light(RT::create_point(0.0, 10.0, -10.0), RT::color_white());

        auto const exp_reflected_color = RT::color(0.7364, 0.7364, 0.7364);
        auto const got_reflected_color = RT::phong_illumination(SURFACE_POINT,	  /// where
                                                                SURFACE_MATERIAL, /// material
                                                                incident_light,	  /// incoming light
                                                                eye_vector,	  /// viewwer
                                                                surface_normal);  /// normal

        CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light at 45° to normal, relection at 45° to normal, viewer on reflected light")
{
        auto const eye_vector	  = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, -RT::SQRT_2_BY_2F);
        auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
        auto const incident_light = RT::point_light(RT::create_point(0.0, 10.0, -10.0), RT::color_white());

        auto const exp_reflected_color = RT::color(1.636385, 1.636385, 1.636385);
        auto const got_reflected_color = RT::phong_illumination(SURFACE_POINT,	  /// where
                                                                SURFACE_MATERIAL, /// material
                                                                incident_light,	  /// incoming light
                                                                eye_vector,	  /// viewwer
                                                                surface_normal);  /// normal

        CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light opposite of normal, reflection opposite of normal, viewer on normal")
{
        auto const eye_vector	  = RT::create_vector(0.0, 0.0, -1.0);
        auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
        auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, 10.0), RT::color_white());

        auto const exp_reflected_color = RT::color(0.1, 0.1, 0.1);
        auto const got_reflected_color = RT::phong_illumination(SURFACE_POINT,	  /// where
                                                                SURFACE_MATERIAL, /// material
                                                                incident_light,	  /// incoming light
                                                                eye_vector,	  /// viewwer
                                                                surface_normal);  /// normal

        CHECK(got_reflected_color == exp_reflected_color);
}