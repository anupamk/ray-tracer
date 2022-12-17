#pragma once


/// c++ includes
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

/// our includes
#include "binary_pattern.hpp"
#include "color.hpp"
#include "pattern_interface.hpp"

namespace raytracer
{
        /*
         * this class implements a gradient-ring pattern, where the
         * color/pattern varies smoothly from color-a -> color-b in a ring.
         **/
        class gradient_ring_pattern final : public binary_pattern<gradient_ring_pattern>
        {
            public:
                gradient_ring_pattern(color a, color b)
                    : binary_pattern(a, b)
                {
                }

                gradient_ring_pattern(std::shared_ptr<pattern_interface> pattern_a,
                                      std::shared_ptr<pattern_interface> pattern_b)
                    : binary_pattern(pattern_a, pattern_b)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const& pt) const override
                {
                        auto const pt_x_squared = pt.x() * pt.x();
                        auto const pt_z_squared = pt.z() * pt.z();
                        auto const magnitude    = std::sqrt(pt_x_squared + pt_z_squared);
                        auto const color_a_val  = color_a(pt);
                        auto const color_b_val  = color_b(pt);
                        auto const delta_color  = color_b_val - color_a_val;

                        return color_a_val + delta_color * magnitude;
                }
        };

} // namespace raytracer


