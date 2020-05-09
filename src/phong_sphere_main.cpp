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

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

int main(int argc, char** argv)
{
	///
	/// define some scene specific constants first
	///

	/// maintain a 3:4 aspect ratio
	uint32_t const canvas_dim_x = 1280;
	uint32_t const canvas_dim_y = (canvas_dim_x * 3) / 4;

	/// other constants
	auto const CAMERA_POSITION = RT::create_point(0.0, 0.0, -3.0);
	float const WALL_ZPOS	   = 6.0;
	float const WALL_XSIZE	   = 12.0;
	float const WALL_YSIZE	   = 9.0;

	/// --------------------------------------------------------------------------------
	/// compute the rest based on these values
	float const WALL_HALF_XSIZE  = WALL_XSIZE * 0.5;
	float const WALL_HALF_YSIZE  = WALL_YSIZE * 0.5;
	float const WALL_XPIXEL_SIZE = WALL_XSIZE / canvas_dim_x;
	float const WALL_YPIXEL_SIZE = WALL_YSIZE / canvas_dim_y;

	/// and the canvas
	auto canvas = RT::canvas(canvas_dim_x, canvas_dim_y);

	// clang-format off
        auto const canvas_constants = std::string(std::string("{") + /// first
                                                  "CANVAS_XPIXELS: "   + std::to_string(canvas_dim_x)     + ", "  +
                                                  "CANVAS_YPIXELS: "   + std::to_string(canvas_dim_y)     + ", "  +
                                                  "WALL_ZPOS: "        + std::to_string(WALL_ZPOS)        + ", "  +
                                                  "WALL_XSIZE: "       + std::to_string(WALL_XSIZE)       + ", "  +
                                                  "WALL_YSIZE: "       + std::to_string(WALL_YSIZE)       + ", "  +
                                                  "WALL_XPIXEL_SIZE: " + std::to_string(WALL_XPIXEL_SIZE) + ", "  +
                                                  "WALL_YPIXEL_SIZE: " + std::to_string(WALL_YPIXEL_SIZE) + ", "  +
                                                  std::string("}") /// last
                                                  );
	// clang-format on

	LOG_INFO("phong-sphere canvas parameters: %s", canvas_constants.c_str());

	/// --------------------------------------------------------------------------------
	/// setup the sphere and its properties first
	auto sphere_material	= RT::material();
	auto const sphere_color = RT::color(1.0, 0.2, 1.0);
	sphere_material.set_color(sphere_color).set_specular(0.7);

	auto sphere = std::make_shared<RT::sphere>();
	sphere->set_material(sphere_material);

	/// --------------------------------------------------------------------------------
	/// next setup the light and viewer on the scene
	auto const light_position = RT::create_point(-20.0, 20.0, -20.0);
	auto const light_color	  = RT::color(sphere_color * 0.64125);
	auto const scene_light	  = RT::point_light(light_position, light_color);

	// clang-format off
        auto const scene_descr = (std::string("sphere: {") + sphere->stringify() + "}, " +
                                  std::string("light: {") + scene_light.stringify() + "}");
	// clang-format on
	LOG_INFO("phong-sphere scene description '%s'", scene_descr.c_str());

	/// convenience
	namespace CHRONO	     = std::chrono;
	using HR_CLOCK		     = std::chrono::high_resolution_clock;
	using CHRONO_MS		     = std::chrono::milliseconds;
	auto const render_start_time = HR_CLOCK::now();

	/// for each pixel on the wall do
	for (uint32_t y = 0; y < canvas_dim_y; y++) {
		auto wall_y_coord = WALL_HALF_YSIZE - WALL_YPIXEL_SIZE * y;

		for (uint32_t x = 0; x < canvas_dim_x; x++) {
			auto wall_x_coord   = -WALL_HALF_XSIZE + WALL_XPIXEL_SIZE * x;
			auto wall_xyz_coord = RT::create_point(wall_x_coord, wall_y_coord, WALL_ZPOS);

			/// parametric equation of ray towards the wall
			auto normalized_ray_dir = normalize(wall_xyz_coord - CAMERA_POSITION);
			auto ray_to_wall	= RT::ray_t(CAMERA_POSITION, normalized_ray_dir);

			// clang-format off
			/// find intersection point of ray with the sphere
			auto xs_record = ray_to_wall.intersect(sphere);
			auto xs_point  = (xs_record ?
                                          RT::visible_intersection(xs_record.value()) :
                                          std::nullopt);
			// clang-format on

			if (!xs_point) {
				/// ray never intersected the sphere...next ray
				continue;
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

			// clang-format off
                        auto surface_color = RT::phong_illumination(hit_position,
                                                                    sphere->get_material(),
                                                                    scene_light,
                                                                    viewer_at,
                                                                    surface_normal);
			// clang-format on

			canvas.write_pixel(x, y, surface_color);
		}
	}

	auto const render_end_time = HR_CLOCK::now();
	auto const render_time_ms  = CHRONO::duration_cast<CHRONO_MS>(render_end_time - render_start_time);

	/// write canvas data out
	auto const img_fname = "phong-sphere.ppm";
	canvas.save_canvas(img_fname);
	LOG_INFO("image-file: '%s' generated, render-time (ms): '%ld'", img_fname, render_time_ms);

	return 0;
}
