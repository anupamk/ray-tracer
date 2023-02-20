/*
 * this program uses sphere and ray intersections to draw the silhouette of a
 * sphere on the canvas.
 **/

/// c++ includes
#include <cstdint>
#include <memory>
#include <string>

/// our includes
#include "common/include/logging.h"
#include "io/canvas.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/sphere.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

int main(int argc, char** argv)
{
        ///
        /// define some scene specific constants first
        ///
        uint32_t const canvas_dim_x = 1280;
        uint32_t const canvas_dim_y = 1024;
        auto const CAMERA_POSITION  = RT::create_point(0.0, 0.0, -3.0);
        float const WALL_ZPOS       = 6.0;
        float const WALL_XSIZE      = 12.0;
        float const WALL_YSIZE      = 9.0;

        /// compute the rest based on these values
        float const WALL_HALF_XSIZE  = WALL_XSIZE * 0.5;
        float const WALL_HALF_YSIZE  = WALL_YSIZE * 0.5;
        float const WALL_XPIXEL_SIZE = WALL_XSIZE / canvas_dim_x;
        float const WALL_YPIXEL_SIZE = WALL_YSIZE / canvas_dim_y;

        // clang-format off
        auto const scene_constants = std::string(std::string("{") + /// first
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
        LOG_DEBUG("scene parameters: %s", scene_constants.c_str());

        auto const RED_COLOR = RT::color_red();
        auto sphere          = std::make_shared<RT::sphere>();
        auto canvas          = RT::canvas::create_binary(canvas_dim_x, canvas_dim_y);

        ///
        /// some transformations on the sphere
        ///

        /// shrink along y-axis
        sphere->transform(RT_XFORM::create_3d_scaling_matrix(1.0, 0.5, 1.0));

        /// shrink along x+y-axis
        sphere->transform(RT_XFORM::create_3d_scaling_matrix(0.5, 0.75, 1.0));

        /// shrink + rotate-along-z
        sphere->transform(RT_XFORM::create_rotz_matrix(RT::PI_BY_4F) *
                          RT_XFORM::create_3d_scaling_matrix(0.5, 1.0, 1.0));

        /// shrink + skew
        sphere->transform(RT_XFORM::create_shearing_matrix(1.0, 0.0, /// x
                                                           0.0, 0.0, /// y
                                                           0.0, 0.0) /// z
                          * RT_XFORM::create_3d_scaling_matrix(0.5, 1.0, 1.0));

        /// for each pixel do
        for (uint32_t y = 0; y < canvas_dim_y; y++) {
                auto const wall_y = WALL_HALF_YSIZE - WALL_YPIXEL_SIZE * y;

                for (uint32_t x = 0; x < canvas_dim_x; x++) {
                        auto const wall_x      = -WALL_HALF_XSIZE + WALL_XPIXEL_SIZE * x;
                        auto const wall_xyz    = RT::create_point(wall_x, wall_y, WALL_ZPOS);
                        auto const ray_to_wall = RT::ray_t(CAMERA_POSITION, (wall_xyz - CAMERA_POSITION));

                        auto const xs_record = ray_to_wall.intersect(sphere);
                        if (xs_record && RT::visible_intersection(xs_record.value())) {
                                canvas.write_pixel(x, y, RED_COLOR);
                        }
                }
        }

        /// write canvas data out
        auto const img_fname = "sphere-silhouette.ppm";
        canvas.write(img_fname);
        LOG_INFO("image-file: '%s' generated", img_fname);

        return 0;
}
