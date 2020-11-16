/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
**/

/// c++ includes
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/// 3rd-party libraries
#include "concurrentqueue.h"

/// our includes
#include "camera.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "point_light.hpp"
#include "tuple.hpp"
#include "world.hpp"
#include "sphere.hpp"
#include "plane.hpp"
#include "shape_interface.hpp"
#include "logging.h"
#include "solid_pattern.hpp"
#include "striped_pattern.hpp"
#include "gradient_pattern.hpp"
#include "ring_pattern.hpp"
#include "checkers_pattern.hpp"
#include "gradient_ring_pattern.hpp"
#include "blended_pattern.hpp"
#include "perlin_noise_pattern.hpp"
#include "texture_2d_pattern.hpp"
#include "uv_checkers.hpp"
#include "uv_noise_texture.hpp"
#include "gradient_perlin_noise_pattern.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT     = raytracer;
using RT_XFORM   = RT::matrix_transformations_t;
namespace CQ     = moodycamel;
namespace CHRONO = std::chrono;
using HR_CLOCK   = std::chrono::high_resolution_clock;
using CHRONO_MS  = std::chrono::milliseconds;

/// --------------------------------------------------------------------
/// - a render-work item is a ray at a specific place on the canvas
///
/// - a bunch of render-work items is what is what gets handled by a
/// single rendering thread.
struct render_work_item {
	uint32_t x;
	uint32_t y;
	RT::ray_t r;
};

struct render_work {
	std::vector<render_work_item> work_list;
};

/// file specific functions
static RT::world create_world();
static RT::camera create_camera();
static void st_render_world(RT::world const&, RT::camera const&, std::string const& dst_fname = "");
static void mt_render_world(RT::world const&, RT::camera const&, std::string const& dst_fname = "");
static void coloring_worker(int, CQ::ConcurrentQueue<render_work>&, RT::world const&, RT::canvas&);

int main(int argc, char** argv)
{
	auto const w = create_world();
	auto const c = create_camera();
	mt_render_world(w, c, "render-with-textures.ppm");

	return 0;
}

/*
 * only file specific functions from this point onwards
**/

/// ----------------------------------------------------------------------------
/// this function is called to render the world. this is the single threaded renderer.
static inline void st_render_world(RT::world const& w, RT::camera const& c, std::string const& dst_fname)
{
	/// convenience
	auto const render_start_time = HR_CLOCK::now();

	/// render the scene
	auto rendered_canvas = c.render(w);

	// clang-format off
        auto render_end_time = HR_CLOCK::now();
        long render_time_ms =  CHRONO::duration_cast<CHRONO_MS>(render_end_time -
                                                                render_start_time).count();
	// clang-format on
	LOG_INFO("total render time: %ld (ms)", render_time_ms);

	if (!dst_fname.empty()) {
		rendered_canvas.write(dst_fname);
		LOG_INFO("saved canvas in: '%s'", dst_fname.c_str());
	} else {
		rendered_canvas.show();
	}

	return;
}

/// ----------------------------------------------------------------------------
/// this function is called to render the world. this is the multi-threaded renderer.
static inline void mt_render_world(RT::world const& w, RT::camera const& c, std::string const& dst_fname)
{
	RT::canvas rendered_canvas = RT::canvas::create_ascii(c.hsize(), c.vsize());
	LOG_INFO("canvas information: '%s'", rendered_canvas.stringify().c_str());

	/// --------------------------------------------------------------------
	/// concurrent queue contains multiple instances of render_work defined
	/// above.
	int const MAX_RENDERING_THREADS   = std::thread::hardware_concurrency();
	int const TOTAL_PIXELS_PER_THREAD = c.hsize() / MAX_RENDERING_THREADS;
	CQ::ConcurrentQueue<render_work> work_queue(TOTAL_PIXELS_PER_THREAD * c.vsize());

	/// start pushing work out into the queue
	for (uint32_t y = 0; y < c.vsize(); y++) {
		for (uint32_t x = 0; x < c.hsize(); x += TOTAL_PIXELS_PER_THREAD) {
			render_work work_item;
			work_item.work_list.reserve(TOTAL_PIXELS_PER_THREAD);

			for (int i = 0; i < TOTAL_PIXELS_PER_THREAD; i++) {
				render_work_item tmp{
					x + i,                    /// x-pixel
					y,                        /// y-pixel
					c.ray_for_pixel(x + i, y) /// ray-at-(x,y)
				};

				work_item.work_list.push_back(tmp);
			}

			work_queue.enqueue(work_item);
		}
	}

	/// threads are about to start, start counting...
	auto const render_start_time = HR_CLOCK::now();

	/// --------------------------------------------------------------------
	/// start the coloring threads
	std::vector<std::thread> rendering_threads(MAX_RENDERING_THREADS);
	for (auto i = 0; i < MAX_RENDERING_THREADS; i++) {
		rendering_threads[i] = std::thread(coloring_worker,            /// rendering-function
		                                   i,                          /// thread-id
		                                   std::ref(work_queue),       /// work-queue
		                                   w,                          /// the world
		                                   std::ref(rendered_canvas)); /// canvas
	}

	/// wait for all of them to terminate
	std::for_each(rendering_threads.begin(), rendering_threads.end(), std::mem_fn(&std::thread::join));

	/// all threads have terminated by now...
	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: %ld (ms)", render_time_ms);

	if (!dst_fname.empty()) {
		rendered_canvas.write(dst_fname);
		LOG_INFO("saved canvas in: '%s'", dst_fname.c_str());
	} else {
		rendered_canvas.show();
	}

	return;
}

/// ----------------------------------------------------------------------------
/// this is coloring-worker i.e. the thread function responsible for rendering a
/// bunch of rays on the canvas
static void coloring_worker(int thread_id,                                /// for logging
                            CQ::ConcurrentQueue<render_work>& work_queue, /// work-item-queue
                            RT::world const& w,                           /// world
                            RT::canvas& dst_canvas)                       /// destination-canvas
{
	bool all_done          = false;
	size_t pixels_rendered = 0;
	size_t jobs_completed  = 0;
	auto render_start_time = HR_CLOCK::now();

	/// do some work
	do {
		render_work rw;
		if (work_queue.try_dequeue(rw)) {
			for (auto work : rw.work_list) {
				auto r_color = w.color_at(work.r);
				dst_canvas.write_pixel(work.x, work.y, r_color);

				pixels_rendered += 1;
			}

			jobs_completed += 1;
		}

		/// ------------------------------------------------------------
		/// are there more items in work-queue ?
		auto items_in_q = work_queue.size_approx();
		if (items_in_q == 0) {
			all_done = true;
		}
	} while (!all_done);

	/// this thread is done. dump some stats...
	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	// clang-format off
	LOG_DEBUG("thread: %d done, pixels rendered: %ld, total jobs: %ld, time: %ld (ms)"
                  ,
                  thread_id,
                  pixels_rendered,
                  jobs_completed,
                  render_time_ms);
	// clang-format on

	return;
}

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
			std::make_shared<RT::texture_2d_pattern>(sp_01_noisy_texture, sphere_01);

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

		auto sp_03_texture = std::make_shared<RT::texture_2d_pattern>(sp_03_uv_texture, sphere_03);

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
