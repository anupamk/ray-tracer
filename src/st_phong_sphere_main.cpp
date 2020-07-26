/*
 * this program implements a single-threaded phong-illumination of a sphere.
**/

/// c++ includes
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

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
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

static RT::color color_pixel(uint32_t X, uint32_t Y, RT::scene_params const& params);

int main(int argc, char** argv)
{
	auto const scene_params = RT::create_scene_params();
	RT::canvas dst_canvas	= RT::canvas::create_binary(scene_params.canvas_dim_x,	/// x-resolution
							    scene_params.canvas_dim_y); /// y-resolution
	LOG_INFO("canvas information: '%s'", dst_canvas.stringify().c_str());

	/// convenience
	namespace CHRONO	     = std::chrono;
	using HR_CLOCK		     = std::chrono::high_resolution_clock;
	using CHRONO_MS		     = std::chrono::milliseconds;
	auto const render_start_time = HR_CLOCK::now();

	/// for each pixel on the wall do
	for (uint32_t y = 0; y < scene_params.canvas_dim_y; y++) {
		for (uint32_t x = 0; x < scene_params.canvas_dim_x; x++) {
			auto xy_color = color_pixel(x, y, scene_params);
			dst_canvas.write_pixel(x, y, xy_color);
		}
	}

	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: %ld (ms)", render_time_ms);

	/// let's see what we did here
	dst_canvas.show();

	return 0;
}

/*
 * this function is called to color a specific pixel on the canvas
**/
static inline RT::color color_pixel(uint32_t x_coord,		    /// x-coordinate
				    uint32_t y_coord,		    /// y-coordinate
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

	return RT::phong_illumination(params.shape,    /// shape
				      hit_position,    /// hit-point
				      params.light,    /// light
				      viewer_at,       /// viewer
				      surface_normal); /// normal at hit-point
}
