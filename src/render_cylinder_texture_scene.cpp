/*
 * render a scene using cylinders
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
#include "cube.hpp"
#include "cylinder.hpp"
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
RT::world create_cylinder_world();
RT::camera create_cylinder_world_camera();

int main(int argc, char** argv)
{
	auto world     = create_cylinder_world();
	auto camera    = create_cylinder_world_camera();
	auto dst_fname = "cylinder-texture-scene.ppm";

	LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
	         "destination: '%s'}",
	         camera.hsize(), camera.vsize(), dst_fname);

	/// --------------------------------------------------------------------
	/// benchmark the render with 'num_iterations' renders performed, and
	/// throwing away the results from 'num_discards' of them
	auto const num_iterations = 1;
	auto const num_discards   = 0;
	Benchmark<> render_bm(num_iterations, num_discards);
	LOG_INFO("render benchmark info: '%s'", render_bm.stringify().c_str());

	/// --------------------------------------------------------------------
	/// just use the first [0] result only please
	auto rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
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

/// ----------------------------------------------------------------------------
/// this function is called to create a world which contains just cylinders as
/// primitive shapes
RT::world create_cylinder_world()
{
	auto world = RT::world();

	/// --------------------------------------------------------------------
	/// the world light
	auto world_light_01 = RT::point_light(RT::create_point(-10.0, 10.0, -10.0), RT::color_white());
	world.add(world_light_01);

	/// --------------------------------------------------------------------
	/// add the cylinder
	{
		auto cs = std::make_shared<RT::cylinder>(false, /// cast-a-shadow
		                                         0.0,   /// min-y
		                                         1.0,   /// max-y
		                                         true); /// capped ?
		world.add(cs);

		cs->transform(RT_XFORM::create_3d_translation_matrix(0.0, -0.5, 0.0) *
		              RT_XFORM::create_3d_scaling_matrix(1.0, RT::PI, 1.0) *
		              RT_XFORM::create_rotx_matrix(RT::PI / 15.0));

		auto cs_uv_pattern = std::make_shared<RT::uv_checkers>(16,                       /// u-squares
		                                                       RT::color(0.0, 0.5, 0.0), /// u-color
		                                                       8,                        /// v-squares
		                                                       RT::color(1.0, 1.0, 1.0)); /// v-color
		auto cs_texture =
			std::make_shared<RT::texture_2d_pattern>(cs_uv_pattern, RT::cylindrical_map);

		auto cs_material = RT::material()
		                           .set_ambient(0.1)         /// ambient
		                           .set_specular(0.6)        /// specular
		                           .set_shininess(15)        /// shininess
		                           .set_diffuse(0.8)         /// diffuse
		                           .set_pattern(cs_texture); /// pattern
		cs->set_material(cs_material);
	}

	return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_cylinder_world(...)
RT::camera create_cylinder_world_camera()
{
	auto camera_01     = RT::camera(1280, 1024, 0.5);
	auto look_from     = RT::create_point(0.0, 0.0, -20.0);
	auto look_to       = RT::create_point(0.0, 0.0, 0.0);
	auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
	auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

	camera_01.transform(xform);

	return camera_01;
}