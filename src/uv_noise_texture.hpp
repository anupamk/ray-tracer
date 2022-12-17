#pragma once


/// c++ includes
#include <cstdint>

/// our includes
#include "color.hpp"
#include "gradient_perlin_noise_pattern.hpp"
#include "utils.hpp"
#include "uv_pattern_interface.hpp"

namespace raytracer
{
        /*
         * this computes perlin-noise-perturbed uv color at a specific
         * uv-point. since this is quite similar to a gradient-perlin noise, we
         * just create an instance of 'gradient_perlin_noise_pattern' and let
         * that handle all the computations
         **/
        class uv_noise final : public uv_pattern_interface
        {
            private:
                gradient_perlin_noise_pattern const grad_pn_;

            public:
                uv_noise(color u, color v,
                         uint32_t seed   = 0,  /// gradient-lattice
                         uint8_t octaves = 16) /// perline-noise
                    : grad_pn_(u, v, seed, octaves)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// compute the color at a specific point on the pattern
                color uv_pattern_color_at(uv_point const& uv) const override
                {
                        tuple uv_tuple_val{uv.u(), uv.v(), 0.0, tuple_type_t::POINT};
                        return grad_pn_.color_at_point(uv_tuple_val);
                }
        };
} // namespace raytracer


