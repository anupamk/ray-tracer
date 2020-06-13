/*
 * this program uses sphere and ray intersections to draw the silhouette of a
 * sphere on the canvas.
**/

/// c++ includes
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>

/// sdl includes
#include <SDL2/SDL.h>

/// our includes
#include "color.hpp"
#include "tuple.hpp"
#include "sphere.hpp"
#include "sdl2_canvas.hpp"
#include "ray.hpp"
#include "logging.h"
#include "canvas.hpp"
#include "matrix_transformations.hpp"
#include "phong_illumination.hpp"
#include "material.hpp"
#include "point_light.hpp"
#include "assert_utils.h"
#include "intersection_record.hpp"

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

static RT::color color_pixel(uint32_t X, uint32_t Y, scene_params const& params);

int main(int argc, char** argv)
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

	const scene_params params {canvas_dim_x,
                                   canvas_dim_y,
                                   WALL_ZPOS,
                                   WALL_XSIZE,
                                   WALL_YSIZE,
                                   CAMERA_POSITION,
                                   sphere,
                                   scene_light};
	// clang-format on
	LOG_INFO("phong-sphere scene description '%s'", scene_descr.c_str());

	/// convenience
	namespace CHRONO	     = std::chrono;
	using HR_CLOCK		     = std::chrono::high_resolution_clock;
	using CHRONO_MS		     = std::chrono::milliseconds;
	auto const render_start_time = HR_CLOCK::now();

	/// for each pixel on the wall do
	auto canvas = RT::sdl2_canvas(canvas_dim_x, canvas_dim_y);
	for (uint32_t y = 0; y < canvas_dim_y; y++) {
		for (uint32_t x = 0; x < canvas_dim_x; x++) {
			auto xy_color = color_pixel(x, y, params);
			canvas.write_pixel(x, y, xy_color);
		}

                /// see progress per 'scanline'
                canvas.show_canvas();
	}

	auto render_end_time = HR_CLOCK::now();
	long render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time).count();

	LOG_INFO("total render time: '%ld' (ms)", render_time_ms);

	return 0;
}

/*
 * this function is called to color a specific pixel on the canvas
**/
static inline RT::color color_pixel(uint32_t x_coord,		/// x-coordinate
				    uint32_t y_coord,		/// y-coordinate
				    scene_params const& params) /// scene-params
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
