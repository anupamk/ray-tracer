#ifndef RAYTRACER_CAMERA_HPP__
#define RAYTRACER_CAMERA_HPP__

/// c++ includes
#include <cstdint>
#include <string>

/// our includes
#include "canvas.hpp"
#include "matrix.hpp"
#include "ray.hpp"
#include "world.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this describes a virtual camera which let's us take 'pictures' of a
        /// scene f.e. allowing us easily to zoom in/out, rotate camera etc.
        class camera
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
                canvas render(world const&) const;
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
        };
} // namespace raytracer

#endif /// RAYTRACER_CAMERA_HPP__
