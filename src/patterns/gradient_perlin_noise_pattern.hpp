#pragma once


/// c++ includes
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>

/// our includes
#include "patterns/binary_pattern.hpp"
#include "patterns/pattern_interface.hpp"
#include "patterns/perlin_noise.hpp"
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"

namespace raytracer
{
        /*
         * this class implements the improved perlin noise pattern i.e. one
         * where the color is perturbed using perlin noise.
         **/
        class gradient_perlin_noise_pattern final : public binary_pattern<gradient_perlin_noise_pattern>
        {
            private:
                perlin_noise const pn_;
                uint8_t const octaves_;

            public:
                gradient_perlin_noise_pattern(color a, color b,
                                              uint32_t seed   = 0,  /// gradient-lattice
                                              uint8_t octaves = 16) /// perlin-noise
                    : binary_pattern(a, b)
                    , pn_(perlin_noise(seed))
                    , octaves_(octaves)
                {
                }

                gradient_perlin_noise_pattern(std::shared_ptr<pattern_interface> a,
                                              std::shared_ptr<pattern_interface> b,
                                              uint32_t seed   = 0,  /// gradient-lattice
                                              uint8_t octaves = 16) /// perlin-noise
                    : binary_pattern(a, b)
                    , pn_(perlin_noise(seed))
                    , octaves_(octaves)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return a noise-perturbed color at a specific point
                color color_at_point(tuple const& P) const override
                {
                        double const noise = pn_.octave_noise_3d_clamped_01(P.x(), P.y(), P.z(), octaves_);

                        /// ----------------------------------------------------
                        /// now compute the perturbed color
                        double const noise_min   = 0.0;
                        double const noise_max   = 1.0;
                        double const noise_range = (noise_max - noise_min);

                        auto const color_start     = color_a(P) * (noise_max - noise);
                        auto const color_end       = color_b(P) * (noise - noise_min);
                        auto const perturbed_color = (color_start + color_end) / noise_range;

                        return color(perturbed_color.R(), perturbed_color.G(), perturbed_color.B());
                }
        };

} // namespace raytracer
