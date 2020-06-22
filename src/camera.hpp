#ifndef RAYTRACER_CAMERA_HPP__
#define RAYTRACER_CAMERA_HPP__

/// c++ includes
#include <cstdint>
#include <string>

/// our includes
#include "matrix.hpp"
#include "ray.hpp"
#include "world.hpp"
#include "canvas.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this desscribes a virtual camera which let's us take 'pictures' of a
        /// scene f.e. allowing us easily to zoom in/out, rotate camera etc.
        class camera
        {
            private:
                uint32_t const horiz_size_;
                uint32_t const vert_size_;
                double const field_of_view_;
                double half_width_;
                double half_height_;
                double pixel_size_;
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
