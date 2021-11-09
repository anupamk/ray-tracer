#ifndef RAYTRACER_PERLIN_NOISE_PATTERN_HPP__
#define RAYTRACER_PERLIN_NOISE_PATTERN_HPP__

/// c++ includes
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>

/// our includes
#include "constants.hpp"
#include "pattern_interface.hpp"
#include "perlin_noise.hpp"
#include "solid_pattern.hpp"
#include "tuple.hpp"

namespace raytracer
{
        /*
         * this class implements the improved perlin noise pattern i.e. one
         * where the color is perturbed using perlin noise.
         **/
        class perlin_noise_pattern final : public pattern_interface
        {
            private:
                std::shared_ptr<pattern_interface> const pattern_;
                perlin_noise const pn_;
                uint8_t const octaves_;

            public:
                perlin_noise_pattern(color c,
                                     uint32_t seed   = 0,  /// perlin-noise-seed
                                     uint8_t octaves = 16) /// octaves
                    : pattern_(std::make_shared<solid_pattern>(c))
                    , pn_(perlin_noise(seed))
                    , octaves_(octaves)
                {
                }

                perlin_noise_pattern(std::shared_ptr<pattern_interface> p,
                                     uint32_t seed   = 0,  /// perlin-noise-seed
                                     uint8_t octaves = 16) /// octaves
                    : pattern_(p)
                    , pn_(perlin_noise(seed))
                    , octaves_(octaves)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point
                color color_at_point(tuple const& P) const override
                {
                        double const noise = pn_.octave_noise_3d_clamped_01(P.x(), P.y(), P.z(), octaves_);

                        /// ----------------------------------------------------
                        /// now compute the perturbed color
                        double const noise_min      = 0.0;
                        double const noise_max      = 1.0;
                        double const noise_range    = (noise_max - noise_min);
                        color const perturbed_color = pattern_->color_at_point(P) * (noise_max - noise);

                        return color(perturbed_color.R() / noise_range,  /// r
                                     perturbed_color.G() / noise_range,  /// g
                                     perturbed_color.B() / noise_range); /// b
                }
        };
} // namespace raytracer

#endif /// RAYTRACER_PERLIN_NOISE_PATTERN_HPP__
