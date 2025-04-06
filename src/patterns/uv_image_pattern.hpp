#pragma once

/// c++ includes
#include <cstdint>

/// our includes
#include "io/canvas.hpp"
#include "patterns/uv_pattern_interface.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        class uv_image final : public uv_pattern_interface
        {
            private:
                canvas ppm_img;

            public:
                uv_image(canvas decoded_ppm_image)
                    : ppm_img(std::move(decoded_ppm_image))
                {
                }

            public:
                /// ------------------------------------------------------------
                /// compute the color at a specific point on the pattern
                color uv_pattern_color_at(uv_point const& uv) const override
                {
                        /// ----------------------------------------------------
                        /// flip 'v' so that it matches image layout with 'y' at
                        /// the top
                        double flipped_v = 1.0 - uv.v();

                        /// ----------------------------------------------------
                        /// multiply both 'u' and 'v' coordinates by
                        /// 'canvas.width - 1' and 'canvas.height - 1'
                        /// respectively instead of the full 'canvas.width' ||
                        /// 'canvas.height'
                        ///
                        /// this is to avoid overflowing the canvas when 'u' and
                        /// 'v' are both 1.
                        uint32_t x = std::round(uv.u() * (ppm_img.width() - 1));
                        uint32_t y = std::round(flipped_v * (ppm_img.height() - 1));

                        return ppm_img.read_pixel(x, y);
                }
        };
} // namespace raytracer