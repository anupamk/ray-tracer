#pragma once

/// c++ includes
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <random>
#include <string>
#include <vector>

/// our includes
#include "concurrentqueue/concurrentqueue.h"
#include "io/canvas.hpp"
#include "io/render_params.hpp"
#include "primitives/matrix.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class world;
        class xcb_display;

        /// --------------------------------------------------------------------
        /// a render-work item is a ray at a specific place on the canvas
        class render_work_item final
        {
            public:
                double const x;
                double const y;

            public:
                render_work_item(double x, double y)
                    : x(x)
                    , y(y)
                {
                }
        };

        /// --------------------------------------------------------------------
        /// a bunch of render-work items is what is what gets handled by a
        /// single rendering thread
        struct render_work_items final {
                std::vector<render_work_item> work_list;
        };

        /// --------------------------------------------------------------------
        /// this describes a virtual camera which let's us take 'pictures' of a
        /// scene f.e. allowing us easily to zoom in/out, rotate camera etc.
        class camera final
        {
            private:
                /// ------------------------------------------------------------
                /// horizontal and vertical size (in pixels) of the canvas that
                /// the picture will be rendered to.
                uint32_t const horiz_size_;
                uint32_t const vert_size_;

                /// ------------------------------------------------------------
                /// how much can the camera "see", with small values, the view
                /// will be zoomed in, magnifying a small area
                double const field_of_view_;

                /// ------------------------------------------------------------
                /// half-width and half-height of the canvas (in world-space)
                /// units.
                double half_width_;
                double half_height_;

                /// ------------------------------------------------------------
                /// size (in world-space) units of pixels on the canvas
                double pixel_size_;

                /// ------------------------------------------------------------
                /// how the world is oriented relative to the camera.
                fsize_dense2d_matrix_t transform_;
                fsize_dense2d_matrix_t inv_transform_;

                /// ------------------------------------------------------------
                /// render parameters for ease-of-use where needed
                mutable config_render_params render_params_ = {};

            public:
                camera(uint32_t, uint32_t, double);
                ray_t ray_for_pixel(float, float) const;
                void transform(fsize_dense2d_matrix_t const&);

                /*
                 * render the world
                 **/
                canvas render(world const&, config_render_params const& render_params = {}) const;

                std::string stringify() const;

            public:
                // clang-format off
                uint32_t hsize()                       const { return horiz_size_;    }
                uint32_t vsize()                       const { return vert_size_;     }
                double field_of_view()                 const { return field_of_view_; }
                double pixel_size()                    const { return pixel_size_;    }
                fsize_dense2d_matrix_t transform()     const { return transform_;     }
                fsize_dense2d_matrix_t inv_transform() const { return inv_transform_; }
                // clang-format on

            private:
                void compute_misc_items(uint32_t, uint32_t, double);

                /*
                 * @brief
                 *    low-level rendering routine
                 **/
                canvas perform_rendering(world const&) const;

                /*
                 * @brief
                 *    the workhorse of actually coloring a canvas instance
                 *
                 *    multiple threads are spawned each executing this
                 *    function. the work-queue ensures that one pixel_painter
                 *    instance is not stepping over another.
                 *
                 *    also note that this is a 'static' function, as there is no
                 *    need to spawn instance specific painting routines
                 **/
                void pixel_painter(int,                                             /// thread-id
                                   moodycamel::ConcurrentQueue<render_work_items>&, /// queue-of-work
                                   world const&,                                    /// scene-details
                                   canvas&,                                         /// canvas-details
                                   std::unique_ptr<xcb_display>&);                  /// x11-display

                /*
                 * @brief
                 *    when requested, this function performs and adaptive
                 *    sampling of color(s) at a pixel (x, y) for antialiased
                 *    rendering.
                 *
                 *    instead of casting a fixed number of rays randomly within
                 *    each pixel, we proceed as follows:
                 *
                 *       - step-1: cast 5 rays for a pixel, 4 at each corner of
                 *         the pixel, and 1 at its center.
                 *
                 *       - step-2: if any of the corner's color differ from the
                 *         center by more than
                 *         'AA_MAX_VARIATION_IN_COLOR_SAMPLE' then sub-divide
                 *         the quarter of the pixel, and repeat the process
                 *         recursively.
                 *
                 * @return
                 *    color of the pixel.
                 **/
                color adaptively_color_a_pixel_at(world const&, double x, double y, double delta) const;

                /*
                 * @brief
                 *    just color a pixel at a specific point (x, y)
                 **/
                color pixel_color_at(world const&, double x, double y) const;

                /*
                 * @brief
                 *    generate work in scanline order.
                 *
                 *    for scanline-order, the camera sweeps across the 'world'
                 *    from
                 *          - top-left -> top-right and
                 *          - top -> bottom
                 *
                 *    this is pretty much identical to how CRT displays worked.
                 **/
                moodycamel::ConcurrentQueue<render_work_items> scanline_work_queue_1() const;
                moodycamel::ConcurrentQueue<render_work_items> scanline_work_queue() const;

                /*
                 * @brief
                 *    generate work in hilbert-curve order.
                 *
                 *    a hilbert space-filling curve, fills up a 2d space by
                 *    traversing every point in the space in a specific order.
                 *
                 *    for more details, see:
                 *           https://en.wikipedia.org/wiki/Hilbert_curve
                 **/
                moodycamel::ConcurrentQueue<render_work_items> hilbert_work_queue() const;

                /*
                 * @brief
                 *    generate rendering work as 'tiles' i.e. each work item
                 *    is [M x N] pixels of work.
                 **/
                moodycamel::ConcurrentQueue<render_work_items> tile_work_queue() const;
        };

} // namespace raytracer
