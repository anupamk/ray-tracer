/*
 * this program uses sphere and ray intersections to draw the silhouette of a
 * sphere on the canvas.
 **/

/// c++ includes
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

/// 3rd-party libraries
#include "concurrentqueue.h"
#include <SDL2/SDL.h>

/// our includes
#include "assert_utils.h"
#include "canvas.hpp"
#include "color.hpp"
#include "intersection_record.hpp"
#include "logging.h"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "phong_illumination.hpp"
#include "point_light.hpp"
#include "ray.hpp"
#include "sdl2_canvas.hpp"
#include "sphere.hpp"
#include "tuple.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

///
/// this describes the scene parmeters that are used for coloring a particular
/// point on the scene.
///
class scene_params
{
    public:
	/// stuff 'outside' the scene
	uint32_t const canvas_dim_x;
	uint32_t const canvas_dim_y;
	float const wall_zpos;
	float const wall_xsize;
	float const wall_ysize;
	RT::tuple const camera_position;

	/// objects making up the scene
	std::shared_ptr<RT::shape_interface> const shape;
	RT::point_light const light;

	// clang-format off
        float wall_half_xsize()   const { return wall_xsize * 0.5;          }
        float wall_half_ysize()   const { return wall_ysize * 0.5;          }
        float wall_xpixel_size()  const { return wall_xsize / canvas_dim_x; }
        float wall_ypixel_size()  const { return wall_ysize / canvas_dim_y; }
	// clang-format on
};

/// --------------------------------------------------------------------------------
/// this describes the rgb color values for a specific (x,y) pixel
class pixel_color
{
    private:
	uint32_t x_;
	uint32_t y_;
	RT::color color_;

    public:
	pixel_color()
	    : pixel_color(0, 0, RT::color_black())
	{
	}

	pixel_color(uint32_t x, uint32_t y, RT::color color)
	    : x_(x)
	    , y_(y)
	    , color_(color)
	{
	}

    public:
	// clang-format off
        constexpr uint32_t x()      { return this->x_;     }
        constexpr uint32_t y()      { return this->y_;     }
        constexpr RT::color color() { return this->color_; }
        // clang format on
};

/// --------------------------------------------------------------------------------
/// concurrent color queue for getting results from computation threads into the
/// rendering thread
namespace CQ = moodycamel;
constexpr int DEFAULT_COLORQ_SIZE   = 300000;
constexpr int Q_BULK_ENQUEUE_THRESH = 4000; /// enqueue when we have upto these many items 
constexpr int Q_BULK_DEQUEUE_THRESH = 15000; /// try-dequeuing these many items
CQ::ConcurrentQueue<pixel_color> color_queue(DEFAULT_COLORQ_SIZE);

/// --------------------------------------------------------------------------------
/// how many threads do we start ?
static int const MAX_COLOR_THREADS	     = std::thread::hardware_concurrency();
std::atomic<int> num_active_coloring_threads = MAX_COLOR_THREADS;

/// --------------------------------------------------------------------------------
/// file specific function declarations
static scene_params create_scene_params();
static RT::color color_pixel(uint32_t X, uint32_t Y, scene_params const& params);
static void render_scene(scene_params const& params);
static void color_n_rows(uint32_t start_row, uint32_t end_row, scene_params const& params);

int main(int argc, char** argv)
{
	const auto scene_params	       = create_scene_params();
	const auto scanline_per_thread = scene_params.canvas_dim_y / MAX_COLOR_THREADS;

	/// --------------------------------------------------------------------------------
	/// start the renderer + coloring threads
	std::thread renderer_thread(render_scene, scene_params);
	std::vector<std::thread> coloring_threads(MAX_COLOR_THREADS);

	/// --------------------------------------------------------------------------------
	/// an anoymous struct is better than using a tuple or somesuch...
	for (struct {
		     int i;
		     int start_row;
		     uint32_t remainder; /// straggling items
	     } ctxt = {0, 0, scene_params.canvas_dim_y - (scanline_per_thread * MAX_COLOR_THREADS)};
	     ctxt.i < MAX_COLOR_THREADS; ctxt.i++) {
		/// --------------------------------------------------------------------------------
		/// ok, so, in this scheme, depending on MAX_COLOR_THREADS and
		/// number-of-scanlines, some threads might end up doing
		/// _slightly_ more work, specifically by 1 item, than other
		/// threads.
		///
		/// it is fairer distribution than f.e. allocating all
		/// straggling scanline entries to the last thread.
		int end_row = ctxt.start_row + scanline_per_thread;
		if (ctxt.remainder) {
			end_row	       = end_row + 1;
			ctxt.remainder = ctxt.remainder - 1;
		}

		/// create the coloring thread
		coloring_threads[ctxt.i] = std::thread(color_n_rows, ctxt.start_row, end_row, scene_params);

		LOG_INFO("thread: %d, start-row: %d, end-row: %d, total: %d" /// fmt
			 ,						     /// --values--
			 ctxt.i, ctxt.start_row, end_row, end_row - ctxt.start_row);

		ctxt.start_row = end_row;
	}

	/// wait for all of them to terminate
	std::for_each(coloring_threads.begin(), coloring_threads.end(), std::mem_fn(&std::thread::join));
	renderer_thread.join();

	return 0;
}

/*
 * file specific functions
**/

/// --------------------------------------------------------------------------------
/// this function is called to create scene parameters. these describe things
/// like position of the wall, the sphere (it's material, color etc) and the
/// size of canvas where things are drawn etc.
static scene_params create_scene_params()
{
	/// --------------------------------------------------------------------------------
	/// define some scene specific constants first.
	auto const CAMERA_POSITION = RT::create_point(0.0, 0.0, -3.0);
	float const WALL_ZPOS	   = 6.0;
	float const WALL_XSIZE	   = 12.0;
	float const WALL_YSIZE	   = 9.0;

	/// maintain a 3:4 aspect ratio for the canvas
	uint32_t const canvas_dim_x = 1280;
	uint32_t const canvas_dim_y = (canvas_dim_x * 3) / 4;

	/// --------------------------------------------------------------------------------
	/// setup sphere
	auto sphere_material	= RT::material();
	auto const sphere_color = RT::color(1.0, 0.2, 1.0);
	sphere_material.set_color(sphere_color).set_specular(0.7);

	auto sphere = std::make_shared<RT::sphere>();
	sphere->set_material(sphere_material);

	/// --------------------------------------------------------------------------------
	/// setup scene-lighting
	auto const light_position = RT::create_point(-20.0, 20.0, -20.0);
	auto const light_color	  = RT::color(sphere_color * 0.64125);
	auto const scene_light	  = RT::point_light(light_position, light_color);

	// clang-format off
        auto const scene_descr = (std::string("sphere: {") + sphere->stringify() + "}, " +
                                  std::string("light: {") + scene_light.stringify() + "}");

        LOG_INFO("phong-sphere scene description '%s'", scene_descr.c_str());
	// clang-format on

	return scene_params{canvas_dim_x, canvas_dim_y,    WALL_ZPOS, WALL_XSIZE,
	                    WALL_YSIZE,   CAMERA_POSITION, sphere,    scene_light};
}

/// --------------------------------------------------------------------------------
/// this function is called to color n-rows in an image.
static void color_n_rows(uint32_t start_row, uint32_t end_row, scene_params const& params)
{
	/// let's enqueue in bulk as well
	size_t pixels_to_flush = 0;
	std::vector<pixel_color> eq_px_list(Q_BULK_ENQUEUE_THRESH);

	for (uint32_t y = start_row; y < end_row; y++) {
		for (uint32_t x = 0; x < params.canvas_dim_x; x++) {
			/// bulk enqueue
			if (pixels_to_flush == Q_BULK_ENQUEUE_THRESH) {
				pixels_to_flush = 0;
				color_queue.enqueue_bulk(eq_px_list.begin(), Q_BULK_ENQUEUE_THRESH);
			}

			/// local add
			auto xy_color = color_pixel(x, y, params);
			if (xy_color != RT::color_black()) {
				eq_px_list[pixels_to_flush++] = pixel_color{x, y, xy_color};
			}
		}
	}

	/// there might be upto 'pixels_to_flush' stragglers left. let's get
	/// those also enqueued
	color_queue.enqueue_bulk(eq_px_list.begin(), pixels_to_flush);

	/// this thread is done.
	std::atomic_fetch_sub_explicit(&num_active_coloring_threads, 1, std::memory_order_relaxed);

	return;
}

/// --------------------------------------------------------------------------------
/// this function is called to render the scene on the sdl-canvas
static void render_scene(scene_params const& params)
{
	bool all_done   = false;
	auto the_canvas = RT::sdl2_canvas(params.canvas_dim_x, params.canvas_dim_y);
	std::vector<pixel_color> dq_px_list(Q_BULK_DEQUEUE_THRESH);

	/// convenience
	namespace CHRONO = std::chrono;
	using HR_CLOCK   = std::chrono::high_resolution_clock;

	auto const render_start_time = HR_CLOCK::now();

	do {
		size_t count = color_queue.try_dequeue_bulk(dq_px_list.begin(), dq_px_list.size());
		for (size_t i = 0; i < count; i++) {
			the_canvas.write_pixel(dq_px_list[i].x(), dq_px_list[i].y(), dq_px_list[i].color());
		}

		/*
		 * rendering thread is done, when there are no active coloring
		 * threads, and when all the messages that they have enqueued
		 * onto the queue are all processed here.
		 **/
		// clang-format off
		auto threads_remaining = std::atomic_load_explicit(&num_active_coloring_threads,
                                                                   std::memory_order_relaxed);
                auto items_in_q = color_queue.size_approx();
                all_done = ((threads_remaining == 0) && (items_in_q == 0));
		// clang-format on
	} while (!all_done);

	/// some stats
	using CHRONO_MS      = std::chrono::milliseconds;
	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: '%ld' (ms)", render_time_ms);

	/// show what we just rendered
	the_canvas.show_canvas();

	return;
}

/// --------------------------------------------------------------------------------
/// this function is called to color a pixel (using phong-illumination model) on
/// the canvas
static RT::color color_pixel(uint32_t x_coord,           /// x-coordinate
                             uint32_t y_coord,           /// y-coordinate
                             scene_params const& params) /// scene-params
{
	auto wall_x_coord   = -params.wall_half_xsize() + params.wall_xpixel_size() * x_coord;
	auto wall_y_coord   = params.wall_half_ysize() - params.wall_ypixel_size() * y_coord;
	auto wall_xyz_coord = RT::create_point(wall_x_coord, wall_y_coord, params.wall_zpos);

	/// parametric equation of ray towards the wall
	auto normalized_ray_dir = RT::normalize(wall_xyz_coord - params.camera_position);
	auto ray_to_wall        = RT::ray_t(params.camera_position, normalized_ray_dir);

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

	return RT::phong_illumination(hit_position,                 /// hit-point
	                              params.shape->get_material(), /// shape's material
	                              params.light,                 /// light
	                              viewer_at,                    /// viewer
	                              surface_normal);              /// normal at hit-point
}
