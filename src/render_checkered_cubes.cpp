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
#include "texture_2d_pattern.hpp"
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
	auto dst_fname = "checker-cubes.ppm";

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

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
	auto world = RT::world();

	/// --------------------------------------------------------------------
	/// define colors for the textures and then the textures themselves
	auto const red    = RT::color_red();
	auto const yellow = RT::color(1.0, 1.0, 0.0);
	auto const brown  = RT::color(1.0, 0.5, 0.0);
	auto const green  = RT::color_green();
	auto const cyan   = RT::color(0.0, 1.0, 1.0);
	auto const blue   = RT::color_blue();
	auto const purple = RT::color(1.0, 0.0, 1.0);
	auto const white  = RT::color_white();

	auto const left_face  = std::make_shared<RT::align_check>(yellow, cyan, red, blue, brown);
	auto const front_face = std::make_shared<RT::align_check>(cyan, red, yellow, brown, green);
	auto const right_face = std::make_shared<RT::align_check>(red, yellow, purple, green, white);
	auto const back_face  = std::make_shared<RT::align_check>(green, purple, cyan, white, blue);
	auto const up_face    = std::make_shared<RT::align_check>(brown, cyan, purple, red, yellow);
	auto const down_face  = std::make_shared<RT::align_check>(purple, brown, green, blue, white);

	auto const cube_pattern = std::make_shared<RT::cube_texture>(left_face, front_face, right_face,
	                                                             back_face, up_face, down_face);

	auto const cube_material =
		RT::material().set_pattern(cube_pattern).set_ambient(0.2).set_specular(0.0).set_diffuse(0.8);

	/// --------------------------------------------------------------------
	auto const cube_01 = std::make_shared<RT::cube>();
	{
		cube_01->transform(RT_XFORM::create_3d_translation_matrix(-6, 2, 0) *
		                   RT_XFORM::create_rotx_matrix(0.7854) *
		                   RT_XFORM::create_roty_matrix(0.7854));
		cube_01->set_material(cube_material);

		world.add(cube_01);
	}

	/// --------------------------------------------------------------------
	auto const cube_02 = std::make_shared<RT::cube>();
	{
		cube_02->transform(RT_XFORM::create_3d_translation_matrix(-2, 2, 0) *
		                   RT_XFORM::create_rotx_matrix(0.7854) *
		                   RT_XFORM::create_roty_matrix(2.3562));
		cube_02->set_material(cube_material);

		world.add(cube_02);
	}

	/// --------------------------------------------------------------------
	auto const cube_03 = std::make_shared<RT::cube>();
	{
		cube_03->transform(RT_XFORM::create_3d_translation_matrix(2, 2, 0) *
		                   RT_XFORM::create_rotx_matrix(0.7854) *
		                   RT_XFORM::create_roty_matrix(3.927));
		cube_03->set_material(cube_material);

		world.add(cube_03);
	}

	/// --------------------------------------------------------------------
	auto const cube_04 = std::make_shared<RT::cube>();
	{
		cube_04->transform(RT_XFORM::create_3d_translation_matrix(6, 2, 0) *
		                   RT_XFORM::create_rotx_matrix(0.7854) *
		                   RT_XFORM::create_roty_matrix(5.4978));
		cube_04->set_material(cube_material);

		world.add(cube_04);
	}

	/// --------------------------------------------------------------------
	auto const cube_05 = std::make_shared<RT::cube>();
	{
		cube_05->transform(RT_XFORM::create_3d_translation_matrix(-6, -2, 0) *
		                   RT_XFORM::create_rotx_matrix(-0.7854) *
		                   RT_XFORM::create_roty_matrix(0.7854));
		cube_05->set_material(cube_material);

		world.add(cube_05);
	}

	/// --------------------------------------------------------------------
	auto const cube_06 = std::make_shared<RT::cube>();
	{
		cube_06->transform(RT_XFORM::create_3d_translation_matrix(-2, -2, 0) *
		                   RT_XFORM::create_rotx_matrix(-0.7854) *
		                   RT_XFORM::create_roty_matrix(2.3562));
		cube_06->set_material(cube_material);

		world.add(cube_06);
	}

	/// --------------------------------------------------------------------
	auto const cube_07 = std::make_shared<RT::cube>();
	{
		cube_07->transform(RT_XFORM::create_3d_translation_matrix(2, -2, 0) *
		                   RT_XFORM::create_rotx_matrix(-0.7854) *
		                   RT_XFORM::create_roty_matrix(3.927));
		cube_07->set_material(cube_material);

		world.add(cube_07);
	}

	/// --------------------------------------------------------------------
	auto const cube_08 = std::make_shared<RT::cube>();
	{
		cube_08->transform(RT_XFORM::create_3d_translation_matrix(6, -2, 0) *
		                   RT_XFORM::create_rotx_matrix(-0.7854) *
		                   RT_XFORM::create_roty_matrix(5.4978));
		cube_08->set_material(cube_material);

		world.add(cube_08);
	}

	/// --------------------------------------------------------------------
	/// light up the world
	auto const world_light_01 =
		RT::point_light(RT::create_point(0.0, 100.0, -100.0), RT::color(0.25, 0.25, 0.25));
	world.add(world_light_01);

	auto const world_light_02 =
		RT::point_light(RT::create_point(0.0, -100.0, -100.0), RT::color(0.25, 0.25, 0.25));
	world.add(world_light_02);

	auto const world_light_03 =
		RT::point_light(RT::create_point(-100.0, 0.0, -100.0), RT::color(0.25, 0.25, 0.25));
	world.add(world_light_03);

	auto const world_light_04 =
		RT::point_light(RT::create_point(100.0, 0.0, -100.0), RT::color(0.25, 0.25, 0.25));
	world.add(world_light_04);

	return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
	auto camera_01     = RT::camera(1280, 1024, 0.8);
	auto look_from     = RT::create_point(0.0, 0.0, -20.0);
	auto look_to       = RT::create_point(0.0, 0.0, 0.0);
	auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
	auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

	camera_01.transform(xform);

	return camera_01;
}
