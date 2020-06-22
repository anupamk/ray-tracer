/// c++ includes
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <sstream>
#include <string>

/// our includes
#include "camera.hpp"
#include "ray.hpp"
#include "world.hpp"
#include "canvas.hpp"
#include "matrix.hpp"
#include "tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// create a default camera
        camera::camera(uint32_t h_size, uint32_t v_size, double field_of_view)
            : horiz_size_(h_size)
            , vert_size_(v_size)
            , field_of_view_(field_of_view)
            , half_width_(0.0)
            , half_height_(0.0)
            , pixel_size_(0.0)
            , transform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_transform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
        {
                compute_misc_items(h_size, v_size, field_of_view);
        }

        /// --------------------------------------------------------------------
        /// this function returns a ray starting at the camera and passing
        /// through a specific point (x, y) on the canvas
        ray_t camera::ray_for_pixel(uint32_t x, uint32_t y) const
        {
                /// offset from edge of the canvas to the pixel's center
                auto const x_offset = (x + 0.5) * pixel_size_;
                auto const y_offset = (y + 0.5) * pixel_size_;

                /// un-transformed coordinates of the pixel in world-space.
                auto const world_x = half_width_ - x_offset;
                auto const world_y = half_height_ - y_offset;

                /// camera is at unit distance from the canvas. compute the ray
                /// direction
                auto const pixel       = inv_transform_ * create_point(world_x, world_y, -1.0);
                auto const r_origin    = inv_transform_ * create_point(0.0, 0.0, 0.0);
                auto const r_direction = normalize(pixel - r_origin);

                return ray_t(r_origin, r_direction);
        }

        /// --------------------------------------------------------------------
        /// this function is called to set the transformation matrix of for the
        /// camera. we also update the inverse transform matrix here.
        void camera::transform(fsize_dense2d_matrix_t const& m)
        {
                transform_     = m;
                inv_transform_ = inverse(m);

                return;
        }

        /// --------------------------------------------------------------------
        /// this function is called to render a world. this is the
        /// single-threaded renderer
        canvas camera::render(world const& w) const
        {
                auto dst_canvas = canvas::create_binary(horiz_size_, vert_size_);
                LOG_INFO("canvas information: '%s'", dst_canvas.stringify().c_str());

                for (uint32_t y = 0; y < vert_size_; y++) {
                        for (uint32_t x = 0; x < horiz_size_; x++) {
                                auto const ray_at_xy   = ray_for_pixel(x, y);
                                auto const color_at_xy = w.color_at(ray_at_xy);

                                dst_canvas.write_pixel(x, y, color_at_xy);
                        }
                }

                return dst_canvas;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a stringified representation of
        /// the properties of the camera
        std::string camera::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << "horizontal-size: " << hsize()         << " (pixels), "
                   << "vertical-size: "   << vsize()         << " (pixels), "
                   << "field-of-view: "   << field_of_view() << " (radians), "
                   << "transform: "       << std::endl       << transform().stringify()
                   << std::endl;
                // clang-format on

                return ss.str();
        }

        /*
         * only private functions from this point onwards
        **/

        /// --------------------------------------------------------------------
        /// this function is called to compute miscellaneous items that are used
        /// in the camera for futher computations.
        void camera::compute_misc_items(uint32_t h_size, uint32_t v_size, double fov)
        {
                auto const aspect_ratio = (1.0 * h_size) / (v_size);
                auto const half_view	= tan(fov / 2.0);

                if (aspect_ratio >= 1.0) {
                        half_height_ = half_view / aspect_ratio;
                        half_width_  = half_view;
                } else {
                        half_height_ = half_view;
                        half_width_  = half_view * aspect_ratio;
                }

                pixel_size_ = (half_width_ * 2.0) / h_size;

                return;
        }

} // namespace raytracer
