/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/// our includes
#include "benchmark.hpp"
#include "blended_pattern.hpp"
#include "camera.hpp"
#include "checkers_pattern.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "gradient_pattern.hpp"
#include "gradient_perlin_noise_pattern.hpp"
#include "gradient_ring_pattern.hpp"
#include "logging.h"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "perlin_noise_pattern.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "raytracer_renderer.hpp"
#include "ring_pattern.hpp"
#include "shape_interface.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "striped_pattern.hpp"
#include "texture_2d_pattern.hpp"
#include "tuple.hpp"
#include "uv_checkers.hpp"
#include "uv_mapper.hpp"
#include "uv_noise_texture.hpp"
#include "world.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// file specific functions
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
	auto world     = create_world();
	auto camera    = create_camera();
	auto dst_fname = "render-with-textures.ppm";

	LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
	         "destination: '%s'}",
	         camera.hsize(), camera.vsize(), dst_fname);

	/// --------------------------------------------------------------------
	/// benchmark the render with '10' renders performed, and throwing away
	/// the results from '1' of them
	Benchmark<> render_bm(10, 1);
	LOG_INFO("benchmark details: '%s'", render_bm.stringify().c_str());

	/// --------------------------------------------------------------------
	/// just use the first [0] result only please
	auto rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
	rendered_canvas.write(dst_fname);

	/// --------------------------------------------------------------------
	/// show what we got
	LOG_INFO("benchmark details : {mean (ms): '%05zu', standard-deviation (ms): '%05zu'}",
	         render_bm.mean(),                /// mean-usec
	         render_bm.standard_deviation()); /// stddev-usec

	return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
	/// --------------------------------------------------------------------
	/// create the floor with a blended pattern
	auto floor = std::make_shared<RT::plane>();
	{
		auto fp_01 = std::make_shared<RT::striped_pattern>(RT::color(0.9, 0.9, 0.9), /// pattern-1
		                                                   RT::color(0.8, 0.8, 0.8));

		fp_01->transform(RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
		                 RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 5.5));

		auto fp_02 = std::make_shared<RT::striped_pattern>(RT::color(0.8, 0.8, 0.8), /// pattern-2
		                                                   RT::color(0.7, 0.7, 0.7));

		auto floor_pattern_01 = std::make_shared<RT::blended_pattern>(fp_01, fp_02);

		floor_pattern_01->transform(RT_XFORM::create_roty_matrix(-RT::PI_BY_2F) *
		                            RT_XFORM::create_3d_scaling_matrix(0.6, 1.0, 1.0));

		floor->set_material(RT::material()
		                            .set_pattern(floor_pattern_01) /// pattern
		                            .set_ambient(0.02)             /// ambient
		                            .set_specular(0.0)             /// specular
		                            .set_shininess(0.05));         /// shininess
	}

	/// --------------------------------------------------------------------
	/// create a 'sun' wall
	auto wall = std::make_shared<RT::plane>();
	{
		wall->transform(RT_XFORM::create_3d_translation_matrix(-10.0, 0.0, 100.0) *
		                RT_XFORM::create_rotx_matrix(RT::PI_BY_4F));

		auto wall_01_pattern =
			std::make_shared<RT::gradient_ring_pattern>(RT::color(1.0, 0.0, 0.0),  /// 'sun'
		                                                    RT::color(1.0, 1.0, 0.0)); /// 'sky'

		wall_01_pattern->transform(RT_XFORM::create_3d_scaling_matrix(30.0, 1.0, 15.0));

		wall->set_material(RT::material()
		                           .set_pattern(wall_01_pattern) /// pattern
		                           .set_ambient(1.0)             /// ambient
		                           .set_specular(0.0)            /// specular
		                           .set_diffuse(0.0));           /// diffuse
	}

	/// --------------------------------------------------------------------
	/// sphere-01 : with some (perlin) noisy pattern
	auto sphere_01 = std::make_shared<RT::sphere>();
	{
		auto sp_01_noisy_texture = std::make_shared<RT::uv_noise>(RT::color(0.0, 0.0, 0.3),  /// u
		                                                          RT::color(1.0, 1.0, 1.0)); /// v

		auto sp_01_noisy_pattern =
			std::make_shared<RT::texture_2d_pattern>(sp_01_noisy_texture, RT::spherical_map);

		sphere_01->transform(RT_XFORM::create_3d_translation_matrix(0.125, 1.0, -1.0));
		sphere_01->set_material(RT::material()
		                                .set_pattern(sp_01_noisy_pattern) /// pattern
		                                .set_ambient(0.1)                 /// ambient
		                                .set_diffuse(0.6)                 /// diffuse
		                                .set_shininess(5.0)               /// shininess
		                                .set_specular(0.4));              /// specular
	}

	/// --------------------------------------------------------------------
	/// sphere-02 : gradient perlin noise
	auto sphere_02 = std::make_shared<RT::sphere>();

	{
		auto sp_02_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
			RT::color(0.1, 0.0, 0.1),  /// color-a
			RT::color(1.0, 1.0, 0.8)); /// color-b

		sphere_02->transform(RT_XFORM::create_3d_scaling_matrix(1.2, 1.2, 1.2) *
		                     RT_XFORM::create_3d_translation_matrix(1.5, 2.0, 8.0) *
		                     RT_XFORM::create_roty_matrix(RT::PI_BY_2F));

		sphere_02->set_material(RT::material()
		                                .set_pattern(sp_02_pattern) /// pattern
		                                .set_ambient(0.1)           /// ambient
		                                .set_diffuse(0.6)           /// diffuse
		                                .set_shininess(10.0)        /// shininess
		                                .set_specular(0.6));        /// specular
	}

	/// --------------------------------------------------------------------
	/// sphere-03 : with checkers
	auto sphere_03 = std::make_shared<RT::sphere>();
	{
		auto sp_03_uv_texture = std::make_shared<RT::uv_checkers>(20, RT::color(0.3, 0.0, 0.0),  /// u
		                                                          10, RT::color(1.0, 1.0, 1.0)); /// v

		auto sp_03_texture =
			std::make_shared<RT::texture_2d_pattern>(sp_03_uv_texture, RT::spherical_map);

		sphere_03->transform(RT_XFORM::create_3d_scaling_matrix(0.9, 0.9, 0.9) *
		                     RT_XFORM::create_3d_translation_matrix(-4.5, 1.0, 2.9));

		sphere_03->set_material(RT::material()
		                                .set_pattern(sp_03_texture) /// pattern
		                                .set_ambient(0.1)           /// ambient
		                                .set_diffuse(0.6)           /// diffuse
		                                .set_shininess(10.0)        /// shininess
		                                .set_specular(0.6));        /// specular
	}

	/// --------------------------------------------------------------------
	/// the world light
	auto world_light_01 = RT::point_light(RT::create_point(-9.0, 9.0, -9.0), RT::color_white());

	/// --------------------------------------------------------------------
	/// now create the world...
	auto world = RT::world();
	world.add(world_light_01);

	world.add(floor);
	world.add(wall);
	world.add(sphere_01);
	world.add(sphere_02);
	world.add(sphere_03);

	LOG_DEBUG("world details:\n%s\n", world.stringify().c_str());

	return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
	auto camera_01     = RT::camera(1280, 1024, 1.047);
	auto look_from     = RT::create_point(1.0, 2.0, -5.0);
	auto look_to       = RT::create_point(0.0, 1.0, 0.0);
	auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
	auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

	camera_01.transform(xform);

	return camera_01;
}
