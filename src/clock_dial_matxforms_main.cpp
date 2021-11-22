/*
 * this program uses matrix transformations (rotation, scaling etc) to draw a
 * simple clock-dial face.
 **/

/// c++ includes
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <vector>

/// our includes
#include "canvas.hpp"
#include "common/include/logging.h"
#include "matrix_transformations.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// file specific functions
static std::vector<RT::tuple> compute_dial_positions(size_t total_points);

int main(int argc, char** argv)
{
        auto dial_hour_pos = compute_dial_positions(100);

        /// ----------------------------------------------------------------
        constexpr auto canvas_dim_x = 1280;
        constexpr auto canvas_dim_y = 1024;

        /// maximum size of dial cannot exceed 90% of the half of minimum of the
        /// canvas, so that we can see something
        constexpr auto max_dial_radius = 0.9 * (std::min(canvas_dim_x, canvas_dim_y) / 2);

        /// compute the maximum ratio of canvas-area to clock-dial area
        constexpr auto canvas_area = canvas_dim_x * canvas_dim_y;
        constexpr auto dial_area   = RT::PI * max_dial_radius * max_dial_radius;
        constexpr auto alpha_max   = dial_area / canvas_area;

        /*
         * ok, so we want the AREA of the clock-face to be let's say
         *      0.0 < α ≦ alpha_max
         * of the total canvas.
         *                       _______________________
         * so, scaled-radius == √ (canvas_area * α) / PI
         *
         * moreover, the clock is drawn in the middle of the canvas.
         **/
        constexpr auto area_alpha    = std::min(0.2, alpha_max);
        constexpr auto scaled_radius = std::sqrt((canvas_area * area_alpha) / RT::PI);
        constexpr auto x_midpt       = canvas_dim_x / 2;
        constexpr auto y_midpt       = canvas_dim_y / 2;

        /// the transformation matrices
        const auto dial_scale_mat  = RT_XFORM::create_3d_scaling_matrix(scaled_radius, scaled_radius, 0.0);
        const auto dial_xlate_mat  = RT_XFORM::create_3d_translation_matrix(x_midpt, y_midpt, 0.0);
        const auto final_xform_mat = dial_xlate_mat * dial_scale_mat;

        /// compute final positions
        for (auto& pos : dial_hour_pos) {
                pos = final_xform_mat * pos;
        }

        /// plot dial positions on canvas
        auto canvas = RT::canvas::create_binary(canvas_dim_x, canvas_dim_y);
        for (const auto& pos : dial_hour_pos) {
                size_t const dial_xpos = pos.x();
                size_t const dial_ypos = pos.y();

                canvas.write_pixel(dial_xpos, dial_ypos, RT::color_white());
        }

        /// write canvas data out
        auto const img_fname = "clock-face.ppm";
        canvas.write(img_fname);
        LOG_INFO("image-file: '%s' generated", img_fname);

        return 0;
}

/*
 * ok, so the clock-face will be drawn on the xy-plane in a
 * left-handed-coordinate system (+ve z-axis, inside the screen)
 *
 * therefore, the 12'o clock position will be at (0, 1, 0) i.e '1' unit
 * from the origin on the y-axis (assuming '1' as the radius of the
 * clock)
 *
 * therefore:
 *    - 'dots' representing hours, will be at an angle of π/6 from each
 *      other.
 *
 *    - moreover, if you imagine the hour-hand rotating to say 1'o
 *      clock position, it becomes pretty obvious that this rotation
 *      happens around the z-axis.
 *
 * with the preliminaries out of the way, let's go.
 **/
static std::vector<RT::tuple> compute_dial_positions(size_t total_points)
{
        auto const dial_12o_clock_at = RT::create_point(0, 1, 0);
        auto const zaxis_rot_angle   = (2 * RT::PI) / total_points;
        auto const dial_zrot_mat     = RT_XFORM::create_rotz_matrix(zaxis_rot_angle);

        std::vector<RT::tuple> dial_pos;
        dial_pos.push_back(dial_12o_clock_at);

        ///
        /// compute other dial positions using previously calculated values.
        /// we have a '-1' because, first position i.e. 12'o clock is
        /// pre-computed.
        ///
        for (size_t i = 0; i < total_points - 1; i++) {
                auto const next_hour_dial_pos = dial_zrot_mat * dial_pos[i];
                dial_pos.push_back(next_hour_dial_pos);
        }

        return dial_pos;
}
