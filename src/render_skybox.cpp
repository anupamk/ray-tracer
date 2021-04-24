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
#include "align_check_pattern.hpp"
#include "benchmark.hpp"
#include "blended_pattern.hpp"
#include "camera.hpp"
#include "checkers_pattern.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "cube.hpp"
#include "cube_map_texture.hpp"
#include "logging.h"
#include "matrix_transformations.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "raytracer_renderer.hpp"
#include "sphere.hpp"
#include "texture_2d_pattern.hpp"
#include "uv_image_pattern.hpp"
#include "uv_mapper.hpp"

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
	auto dst_fname = "skybox.ppm";

	LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
	         "destination: '%s'}",
	         camera.hsize(), camera.vsize(), dst_fname);

	/// --------------------------------------------------------------------
	/// benchmark the render with 'num_iterations' renders performed, and
	/// throwing away the results from 'num_discards' of them
	auto const num_iterations = 1;
	auto const num_discards   = 0;
	Benchmark<> render_bm(num_iterations, num_discards);
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
	auto world = RT::world();

	/// --------------------------------------------------------------------
	/// light up the world
	auto const world_light_01 =
		RT::point_light(RT::create_point(0.0, 100.0, -100.0), RT::color(1.0, 1.0, 1.0));
	world.add(world_light_01);

	/// --------------------------------------------------------------------
	/// cube material and textures
	auto cube_surface_properties = RT::material().set_diffuse(0.0).set_specular(0.0).set_ambient(1.0);

	auto const textureize = [](std::string const& fname) -> std::shared_ptr<RT::uv_image> {
		LOG_INFO("texturizing '%s'", fname.c_str());

		auto canvas_maybe = RT::canvas::load_from_file(fname);
		ASSERT(canvas_maybe.has_value());

		auto canvas_value   = canvas_maybe.value();
		auto canvas_texture = std::make_shared<RT::uv_image>(canvas_value);

		return canvas_texture;
	};

	auto const left_face_texture  = textureize("../textures/skybox/negx.ppm");
	auto const front_face_texture = textureize("../textures/skybox/posz.ppm");
	auto const right_face_texture = textureize("../textures/skybox/posx.ppm");
	auto const back_face_texture  = textureize("../textures/skybox/negz.ppm");
	auto const up_face_texture    = textureize("../textures/skybox/posy.ppm");
	auto const down_face_texture  = textureize("../textures/skybox/negy.ppm");

	// clang-format off
        auto const cube_pattern = std::make_shared<RT::cube_texture>(left_face_texture,
                                                                     front_face_texture,
                                                                     right_face_texture,
                                                                     back_face_texture,
                                                                     up_face_texture,
                                                                     down_face_texture);
        
        auto cube_material = cube_surface_properties.set_pattern(cube_pattern);
	// clang-format on

	/// --------------------------------------------------------------------
	auto const cube_01 = std::make_shared<RT::cube>();
	{
		cube_01->transform(RT_XFORM::create_3d_scaling_matrix(5.0, 5.0, 5.0) *
		                   RT_XFORM::create_rotx_matrix(0.7854) *
		                   RT_XFORM::create_roty_matrix(5.4978));

		cube_01->set_material(cube_material);

		world.add(cube_01);
	}

	return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
	auto camera_01     = RT::camera(1280, 1024, 1.2);
	auto look_from     = RT::create_point(0.0, 0.0, -20.0);
	auto look_to       = RT::create_point(0.0, 0.0, 0.0);
	auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
	auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

	camera_01.transform(xform);

	return camera_01;
}
