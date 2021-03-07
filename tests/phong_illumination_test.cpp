/*
 * this file tests the phong_illumination(...) api
 **/

/// c++ includes
#include <memory>

/// 3rdparty testing library
#include "constants.hpp"
#include "doctest.h"

/// our includes
#include "color.hpp"
#include "logging.h"
#include "material.hpp"
#include "phong_illumination.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

/// all tests run with these
auto const SURFACE_MATERIAL = RT::material();
auto const SURFACE_POINT    = RT::create_point(0.0, 0.0, 0.0);
auto const PLANE            = std::make_shared<RT::plane>();

TEST_CASE("phong_illumination(...) test: light on normal, reflection on normal, viewer on normal")
{
	auto const eye_vector     = RT::create_vector(0.0, 0.0, -1.0);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(1.9, 1.9, 1.9);
	auto const got_reflected_color = RT::phong_illumination(PLANE,           /// shape
	                                                        SURFACE_POINT,   /// where
	                                                        incident_light,  /// incoming light
	                                                        eye_vector,      /// viewwer
	                                                        surface_normal); /// normal

	CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light on normal, reflection on normal, viewer at 45° to normal")
{
	auto const eye_vector     = RT::create_vector(0.0, RT::SQRT_2_BY_2F, -RT::SQRT_2_BY_2F);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(1.0, 1.0, 1.0);
	auto const got_reflected_color = RT::phong_illumination(PLANE,           /// shape
	                                                        SURFACE_POINT,   /// where
	                                                        incident_light,  /// incoming light
	                                                        eye_vector,      /// viewwer
	                                                        surface_normal); /// normal

	CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE("phong_illumination(...) test: light at 45° to normal, reflection on normal, viewer on normal")
{
	auto const eye_vector     = RT::create_vector(0.0, 0.0, -1.0);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 10.0, -10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(0.7364, 0.7364, 0.7364);
	auto const got_reflected_color = RT::phong_illumination(PLANE,           /// shape
	                                                        SURFACE_POINT,   /// where
	                                                        incident_light,  /// incoming light
	                                                        eye_vector,      /// viewwer
	                                                        surface_normal); /// normal

	CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE(
	"phong_illumination(...) test: light at 45° to normal, relection at 45° to normal, viewer on reflected light")
{
	auto const eye_vector     = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, -RT::SQRT_2_BY_2F);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 10.0, -10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(1.636385, 1.636385, 1.636385);
	auto const got_reflected_color = RT::phong_illumination(PLANE,           /// shape
	                                                        SURFACE_POINT,   /// where
	                                                        incident_light,  /// incoming light
	                                                        eye_vector,      /// viewwer
	                                                        surface_normal); /// normal

	CHECK(got_reflected_color == exp_reflected_color);
}

TEST_CASE(
	"phong_illumination(...) test: light opposite of normal, reflection opposite of normal, viewer on normal")
{
	auto const eye_vector     = RT::create_vector(0.0, 0.0, -1.0);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, 10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(0.1, 0.1, 0.1);
	auto const got_reflected_color = RT::phong_illumination(PLANE,           /// shape
	                                                        SURFACE_POINT,   /// where
	                                                        incident_light,  /// incoming light
	                                                        eye_vector,      /// viewwer
	                                                        surface_normal); /// normal

	CHECK(got_reflected_color == exp_reflected_color);
}

/// ----------------------------------------------------------------------------
/// ensure that points that are in a shadow only have ambient contribution to
/// their overall color.
TEST_CASE("phong_illumination(...) test: point in a shadow")
{
	auto const eye_vector     = RT::create_vector(0.0, 0.0, -1.0);
	auto const surface_normal = RT::create_vector(0.0, 0.0, -1.0);
	auto const incident_light = RT::point_light(RT::create_point(0.0, 0.0, -10.0), RT::color_white());

	PLANE->set_material(SURFACE_MATERIAL);

	auto const exp_reflected_color = RT::color(0.1, 0.1, 0.1);
	auto const got_reflected_color = RT::phong_illumination(PLANE,          /// shape
	                                                        SURFACE_POINT,  /// where
	                                                        incident_light, /// incoming light
	                                                        eye_vector,     /// viewwer
	                                                        surface_normal, /// normal
	                                                        true);          /// in-shadow

	CHECK(got_reflected_color == exp_reflected_color);
}
