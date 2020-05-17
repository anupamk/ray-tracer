/*
 * this program uses multiple threads for phong-illumination of a sphere.
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
#include "color.hpp"
#include "tuple.hpp"
#include "sphere.hpp"
#include "ray.hpp"
#include "logging.h"
#include "canvas.hpp"
#include "matrix_transformations.hpp"
#include "phong_illumination.hpp"
#include "material.hpp"
#include "point_light.hpp"
#include "assert_utils.h"
#include "intersection_record.hpp"
#include "scene_params.hpp" /// for, well, scene-parameters

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT = raytracer;
namespace CQ = moodycamel;

/// ----------------------------------------------------------------------------
/// each render thread works on a fixed number of pixels describing the
/// scene. this 'fixed-number-of-pixels' is the 'quanta' of work.
struct render_work {
	std::vector<size_t> point_list;
};

/// --------------------------------------------------------------------------------
/// file specific function declarations
static RT::color color_pixel(uint32_t, uint32_t, RT::scene_params const&);
static void coloring_worker(int, CQ::ConcurrentQueue<render_work>&, RT::scene_params const&, RT::canvas&);

int main(int argc, char** argv)
{
	const auto scene_params = RT::create_scene_params();
	RT::canvas dst_canvas	= RT::canvas::create_binary(scene_params.canvas_dim_x,	/// x-resolution
							    scene_params.canvas_dim_y); /// y-resolution
	LOG_INFO("canvas information: '%s'", dst_canvas.stringify().c_str());

	/// --------------------------------------------------------------------
	/// concurrent queue contains multiple instances of render_work defined
	/// above.
	int const MAX_COLOR_THREADS = std::thread::hardware_concurrency();
	CQ::ConcurrentQueue<render_work> work_queue(scene_params.canvas_dim_y * MAX_COLOR_THREADS);

	/// --------------------------------------------------------------------
	/// starting pushing work out on the queue.
	///
	/// for now, we divide each 'render_work' to have a total of
	/// scene_params.canvas_dim_x/MAX_COLOR_THREADS pixels of coloring
	/// work.
	auto const RENDER_WORK_PIXELS = scene_params.canvas_dim_x / MAX_COLOR_THREADS;

	for (uint32_t y = 0; y < scene_params.canvas_dim_y; y++) {
		for (uint32_t x = 0; x < scene_params.canvas_dim_x; x += RENDER_WORK_PIXELS) {
			/// ----------------------------------------------------
			/// '2 * ...' ∵ 2 values (x, y) per pixel
			render_work rw;
			rw.point_list.reserve(2 * RENDER_WORK_PIXELS);

			for (uint32_t i = 0; i < RENDER_WORK_PIXELS; i++) {
				rw.point_list.push_back(x + i);
				rw.point_list.push_back(y);
			}

			work_queue.enqueue(rw);
		}
	}

	/// --------------------------------------------------------------------------------
	/// start the coloring threads
	std::vector<std::thread> coloring_threads(MAX_COLOR_THREADS);

	for (auto thread_id = 0; thread_id < MAX_COLOR_THREADS; thread_id++) {
		coloring_threads[thread_id] = std::thread(coloring_worker,
							  thread_id,		 /// thread-id
							  std::ref(work_queue),	 /// work-queue
							  scene_params,		 /// scene-params
							  std::ref(dst_canvas)); /// canvas
	}

	/// convenience
	namespace CHRONO	     = std::chrono;
	using HR_CLOCK		     = CHRONO::high_resolution_clock;
	auto const render_start_time = HR_CLOCK::now();

	/// wait for all of them to terminate
	std::for_each(coloring_threads.begin(), coloring_threads.end(), std::mem_fn(&std::thread::join));

	/// some stats
	using CHRONO_MS	     = std::chrono::milliseconds;
	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: %ld (ms)", render_time_ms);

	/// let's see what we did here
	dst_canvas.show();

	return 0;
}

/*
 * file specific functions
**/

/// --------------------------------------------------------------------------------
/// this function is called to color n-rows in an image.
static void coloring_worker(int thread_id, CQ::ConcurrentQueue<render_work>& work_queue,
			    RT::scene_params const& params, RT::canvas& dst_canvas)
{
	bool all_done	      = false;
	size_t items_rendered = 0;

	do {
		/// do some work
		render_work rw;
		if (work_queue.try_dequeue(rw)) {
			for (size_t i = 0; i < rw.point_list.size(); i += 2) {
				auto const x = rw.point_list[i];
				auto const y = rw.point_list[i + 1];

				auto xy_color = color_pixel(x, y, params);
				if (xy_color != RT::color_black()) {
					dst_canvas.write_pixel(x, y, xy_color);
				}
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

	return;
}

/// --------------------------------------------------------------------------------
/// this function is called to color a pixel (using phong-illumination model) on
/// the canvas
static RT::color color_pixel(uint32_t x_coord,		     /// x-coordinate
			     uint32_t y_coord,		     /// y-coordinate
			     RT::scene_params const& params) /// scene-params
{
	auto wall_x_coord   = -params.wall_half_xsize() + params.wall_xpixel_size() * x_coord;
	auto wall_y_coord   = params.wall_half_ysize() - params.wall_ypixel_size() * y_coord;
	auto wall_xyz_coord = RT::create_point(wall_x_coord, wall_y_coord, params.wall_zpos);

	/// parametric equation of ray towards the wall
	auto normalized_ray_dir = RT::normalize(wall_xyz_coord - params.camera_position);
	auto ray_to_wall	= RT::ray_t(params.camera_position, normalized_ray_dir);

	// clang-format off
        /// find intersection point of ray with the sphere
        auto xs_record = ray_to_wall.intersect(params.shape);
        auto xs_point  = (xs_record ?
                          RT::visible_intersection(xs_record.value()) :
                          std::nullopt);
	// clang-format on

	if (!xs_point) {
		/// ray never intersected the sphere...
		return RT::color_black();
	}

	/// ray did hit 'something', that cannot be null
	auto hit_record = xs_point.value();
	ASSERT((hit_record.what_object() != nullptr) && "ray hit null objekt !");

	/*
         * ok, so, if we are here, ray did intersect the sphere.
         *
         * lets now figure out various properties associated
         * with this intersection f.e. hit-position,
         * surface-normal, surface-color etc.
         **/

	/// where is the viewer
	auto viewer_at = -ray_to_wall.direction();

	/// where on the ray did this intersection happen ? and
	/// what is the surface-normal at that position
	auto hit_position   = ray_to_wall.position(hit_record.where());
	auto surface_normal = hit_record.what_object()->normal_at_world(hit_position);

	return RT::phong_illumination(hit_position,		    /// hit-point
				      params.shape->get_material(), /// shape's material
				      params.light,		    /// light
				      viewer_at,		    /// viewer
				      surface_normal);		    /// normal at hit-point
}
