/*
 * this program implements a single-threaded and multi-threaded world-rendering
 * through a camera
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
#include "shape_interface.hpp"
#include "logging.h"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT	 = raytracer;
using RT_XFORM	 = RT::matrix_transformations_t;
namespace CQ	 = moodycamel;
namespace CHRONO = std::chrono;
using HR_CLOCK	 = std::chrono::high_resolution_clock;
using CHRONO_MS	 = std::chrono::milliseconds;

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
static void st_render_world(RT::world const&, RT::camera const&);
static void mt_render_world(RT::world const&, RT::camera const&);
static void coloring_worker(int, CQ::ConcurrentQueue<render_work>&, RT::world const&, RT::canvas&);

int main(int argc, char** argv)
{
	auto const w = create_world();
	auto const c = create_camera();
	mt_render_world(w, c);

	return 0;
}

/*
 * only file specific functions from this point onwards
**/

/// ----------------------------------------------------------------------------
/// this function is called to render the world. this is the single threaded renderer.
static inline void st_render_world(RT::world const& w, RT::camera const& c)
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

	rendered_canvas.show();

	return;
}

/// ----------------------------------------------------------------------------
/// this function is called to render the world. this is the multi-threaded renderer.
static inline void mt_render_world(RT::world const& w, RT::camera const& c)
{
	RT::canvas dst_canvas = RT::canvas::create_binary(c.hsize(), c.vsize());
	LOG_INFO("canvas information: '%s'", dst_canvas.stringify().c_str());

	/// --------------------------------------------------------------------
	/// concurrent queue contains multiple instances of render_work defined
	/// above.
	int const MAX_COLOR_THREADS	= std::thread::hardware_concurrency();
	int const TOTAL_RAYS_PER_THREAD = c.hsize() / MAX_COLOR_THREADS;
	CQ::ConcurrentQueue<render_work> work_queue(TOTAL_RAYS_PER_THREAD * c.vsize());

	/// start pushing work out into the queue
	for (uint32_t y = 0; y < c.vsize(); y++) {
		for (uint32_t x = 0; x < c.hsize(); x += TOTAL_RAYS_PER_THREAD) {
			render_work work_item;
			work_item.work_list.reserve(TOTAL_RAYS_PER_THREAD);

			for (int i = 0; i < TOTAL_RAYS_PER_THREAD; i++) {
				render_work_item tmp{
					x + i,			  /// x-pixel
					y,			  /// y-pixel
					c.ray_for_pixel(x + i, y) /// ray-at-(x,y)
				};

				work_item.work_list.push_back(tmp);
			}

			work_queue.enqueue(work_item);
		}
	}

	/// --------------------------------------------------------------------
	/// start the coloring threads
	std::vector<std::thread> coloring_threads(MAX_COLOR_THREADS);
	for (auto thread_id = 0; thread_id < MAX_COLOR_THREADS; thread_id++) {
		coloring_threads[thread_id] = std::thread(coloring_worker,	 /// rendering-function
							  thread_id,		 /// thread-id
							  std::ref(work_queue),	 /// work-queue
							  w,			 /// the world
							  std::ref(dst_canvas)); /// canvas
	}

	/// convenience
	auto const render_start_time = HR_CLOCK::now();

	/// wait for all of them to terminate
	std::for_each(coloring_threads.begin(), coloring_threads.end(), std::mem_fn(&std::thread::join));

	/// some stats
	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: %ld (ms)", render_time_ms);

	/// let's see what we did here
	dst_canvas.show();

	return;
}

/// ----------------------------------------------------------------------------
/// this is coloring-worker i.e. the thread function responsible for rendering a
/// bunch of rays on the canvas
static void coloring_worker(int thread_id,				  /// for logging
			    CQ::ConcurrentQueue<render_work>& work_queue, /// work-item-queue
			    RT::world const& w,				  /// world
			    RT::canvas& dst_canvas)			  /// destination-canvas
{
	bool all_done	      = false;
	size_t items_rendered = 0;

	/// do some work
	do {
		render_work rw;
		if (work_queue.try_dequeue(rw)) {
			for (auto work : rw.work_list) {
				auto r_color = w.color_at(work.r);
				dst_canvas.write_pixel(work.x, work.y, r_color);
			}
			items_rendered += 1;
		}

		/// ------------------------------------------------------------
		/// are there more items in work-queue ?
		auto items_in_q = work_queue.size_approx();
		if (items_in_q == 0) {
			all_done = true;
		}
	} while (!all_done);

	LOG_DEBUG("thread: %d done, total work done: %ld", thread_id, items_rendered);
}

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// --------------------------------------------------------------------
        /// floor material
        auto const wall_material = RT::material().set_color(RT::color(1.0, 0.9, 0.9)).set_specular(0.0);
        
	/// --------------------------------------------------------------------
	/// 01: floor sphere
	auto floor = std::make_shared<RT::sphere>();
	floor->transform(RT_XFORM::create_3d_scaling_matrix(10.0, 0.01, 10.0));
	floor->set_material(wall_material);

	/// --------------------------------------------------------------------
	/// 02: left wall
	auto left_wall = std::make_shared<RT::sphere>();
	left_wall->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 5.0) *
			     RT_XFORM::create_roty_matrix(-RT::PI_BY_4F) *
			     RT_XFORM::create_rotx_matrix(RT::PI_BY_2F) *
			     RT_XFORM::create_3d_scaling_matrix(10.0, 0.01, 10.0));
	left_wall->set_material(wall_material);

	/// --------------------------------------------------------------------
	/// 03: right wall
	auto right_wall = std::make_shared<RT::sphere>();
	right_wall->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 5.0) *
			      RT_XFORM::create_roty_matrix(RT::PI_BY_4F) *
			      RT_XFORM::create_rotx_matrix(RT::PI_BY_2F) *
			      RT_XFORM::create_3d_scaling_matrix(10.0, 0.01, 10.0));
	right_wall->set_material(wall_material);

	/// --------------------------------------------------------------------
	/// 04: middle sphere
	auto middle_sphere = std::make_shared<RT::sphere>();
	middle_sphere->transform(RT_XFORM::create_3d_translation_matrix(-0.5, 1.0, 0.5));

	// clang-format off
	middle_sphere->set_material(RT::material().set_color(RT::color(0.1, 1.0, 0.5))
                                                  .set_diffuse(0.7)
                                                  .set_specular(0.3));
	// clang-format on

	/// --------------------------------------------------------------------
	/// 05: right sphere
	auto right_sphere = std::make_shared<RT::sphere>();
	right_sphere->transform(RT_XFORM::create_3d_translation_matrix(1.5, 0.5, -0.5) *
				RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5));

	// clang-format off
	right_sphere->set_material(RT::material().set_color(RT::color(0.5, 1.0, 0.1))
                                                 .set_diffuse(0.7)
                                                 .set_specular(0.3));
	// clang-format on

	/// --------------------------------------------------------------------
	/// 06: left sphere
	auto left_sphere = std::make_shared<RT::sphere>();
	left_sphere->transform(RT_XFORM::create_3d_translation_matrix(-1.5, 0.33, -0.75) *
			       RT_XFORM::create_3d_scaling_matrix(0.33, 0.33, 0.33));

	// clang-format off
        left_sphere->set_material(RT::material().set_color(RT::color(1.0, 0.8, 0.1))
                                                .set_diffuse(0.7)
                                                .set_specular(0.3));
	// clang-format on

	/// --------------------------------------------------------------------
	/// the world light
	auto world_light_01 = RT::point_light(RT::create_point(-10.0, 10.0, -10.0), RT::color_white());

	/// --------------------------------------------------------------------
	/// now create the world...
	auto world = RT::world();
	world.add(world_light_01);
        
	world.add(floor);
	world.add(left_wall);
	world.add(right_wall);
	world.add(middle_sphere);
	world.add(right_sphere);
	world.add(left_sphere);

	LOG_DEBUG("world details:\n%s\n", world.stringify().c_str());

	return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
	auto c_01	    = RT::camera(1280, 1024, RT::PI_BY_3F);
	auto c_01_look_from = RT::create_point(0.0, 1.0, -5.0);
	auto c_01_look_to   = RT::create_point(0.0, 1.0, 0.0);
	auto c_01_up_vector = RT::create_vector(0.0, 1.0, 0.0);

	auto c_01_xform = RT_XFORM::create_view_transform(c_01_look_from,  /// from
							  c_01_look_to,	   /// to
							  c_01_up_vector); /// up-vector
	c_01.transform(c_01_xform);

	return c_01;
}