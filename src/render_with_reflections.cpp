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
#include "uv_image_pattern.hpp"
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
	auto dst_fname = "render-red-planet.ppm";

	LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
	         "destination: '%s'}",
	         camera.hsize(), camera.vsize(), dst_fname);

	/// --------------------------------------------------------------------
	/// benchmark the render as follows:
	///   - record timings for 'num_iterations' renderings
	///   - from these records, discard 'num_discards' samples
	/// and then compute mean and standard-deviation
	auto const num_iterations = 10;
	auto const num_discards   = 1;
	Benchmark<> render_bm(num_iterations, num_discards);
	LOG_INFO("render benchmark info: '%s'", render_bm.stringify().c_str());

	/// --------------------------------------------------------------------
	/// just use the first [0] result only please (they are all identical)
	auto rendered_canvas = render_bm.benchmark(RT::single_threaded_renderer, world, camera)[0];
	rendered_canvas.write(dst_fname);

	/// --------------------------------------------------------------------
	/// show what we got
	LOG_INFO("render benchmark results : {mean (ms): '%05zu', standard-deviation (ms): '%05zu'}",
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
		auto floor_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
			RT::color(RT::color::RGB(56, 167, 252)),  /// color-a
			RT::color(RT::color::RGB(56, 128, 252)),  /// color-b
			std::default_random_engine::default_seed, /// seed
			16);                                      /// octaves

		floor->set_material(RT::material()
		                            .set_pattern(floor_pattern)
		                            .set_ambient(0.02)
		                            .set_specular(0.0)
		                            .set_shininess(0.05)
		                            .set_reflective(0.5));
	}

	/// --------------------------------------------------------------------
	/// red-planet without the shadow ('false')
	auto red_planet = std::make_shared<RT::sphere>(false);
	{
		auto fuzzy_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
			RT::color(0.2, 0.0, 0.0),                 /// u
			RT::color(0.8, 0.4, 0.4),                 /// v
			std::default_random_engine::default_seed, /// seed
			16);                                      /// octaves

		red_planet->transform(RT_XFORM::create_3d_scaling_matrix(3.3, 3.3, 3.3) *
		                      RT_XFORM::create_3d_translation_matrix(2.0, 1.5, 3.0) *
		                      RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
		                      RT_XFORM::create_rotz_matrix(-RT::PI));

		red_planet->set_material(RT::material()
		                                 .set_pattern(fuzzy_pattern)
		                                 .set_ambient(0.1)
		                                 .set_diffuse(0.9)
		                                 .set_shininess(10.0)
		                                 .set_specular(0.2));
	}

	/// --------------------------------------------------------------------
	/// the world light
	auto world_light_01 = RT::point_light(RT::create_point(-2.0, 3.0, -10.0), RT::color_white());

	/// --------------------------------------------------------------------
	/// now create the world...
	auto world = RT::world();
	world.add(world_light_01);
	world.add(floor);
	world.add(red_planet);

	return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
	auto camera_01     = RT::camera(1280, 1024, 0.8);
	auto look_from     = RT::create_point(-2.0, 3.0, -25.0);
	auto look_to       = RT::create_point(0.0, 1.1, 0.0);
	auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
	auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

	camera_01.transform(xform);

	return camera_01;
}
