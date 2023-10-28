#pragma once

/// c++ includes
#include <cstdint>
#include <string>

/// our includes
#include "io/canvas.hpp"
#include "io/render_params.hpp"
#include "io/world.hpp"
#include "io/xcb_display.hpp"
#include "primitives/matrix.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// a render-work item is a ray at a specific place on the canvas
        struct render_work_item final {
                uint32_t const x;
                uint32_t const y;
                ray_t const r;
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

            public:
                camera(uint32_t, uint32_t, double);
                ray_t ray_for_pixel(uint32_t, uint32_t) const;
                void transform(fsize_dense2d_matrix_t const&);

                /*
                 * render the world
                 **/
                canvas render(world const&,
                              config_render_params const& render_params = config_render_params()) const;

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
                canvas perform_rendering(world const&, config_render_params const&) const;

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
                static void pixel_painter(int,                                             /// thread-id
                                          moodycamel::ConcurrentQueue<render_work_items>&, /// queue-of-work
                                          world const&,                                    /// scene-details
                                          canvas&,                                         /// canvas-details
                                          std::unique_ptr<xcb_display>&);                  /// x11-display

                /*
                 * @brief
                 *    generate work in scanline order.
                 *
                 *    for scanline-order, the camera sweeps across the 'world'
                 *    from
                 *          - top-left -> top-right and
                 *          - top -> bottom
                 *
                 *    this is pretty much identical to how crt displays worked.
                 **/
                moodycamel::ConcurrentQueue<render_work_items> scanline_work_queue(uint32_t) const;

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
                moodycamel::ConcurrentQueue<render_work_items> hilbert_work_queue(uint32_t) const;

                /*
                 * @brief
                 *    generate rendering work as 'tiles' i.e. each work item
                 *    is [M x N] pixels of work.
                 **/
                moodycamel::ConcurrentQueue<render_work_items> tile_work_queue(uint32_t) const;
        };

} // namespace raytracer
