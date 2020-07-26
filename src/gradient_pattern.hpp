#ifndef RAYTRACER_GRADIENT_PATTERN_HPP__
#define RAYTRACER_GRADIENT_PATTERN_HPP__

/// c++ includes
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

/// our includes
#include "pattern_interface.hpp"
#include "color.hpp"
#include "binary_pattern.hpp"

namespace raytracer
{
        /*
         * this class implements a gradient pattern where the color varies
         * smoothly from one color/pattern to another.
        **/
        class gradient_pattern final : public binary_pattern<gradient_pattern>
        {
            public:
                gradient_pattern(color a, color b)
                    : binary_pattern(a, b)
                {
                }

                gradient_pattern(std::shared_ptr<pattern_interface> pattern_a,
                                 std::shared_ptr<pattern_interface> pattern_b)
                    : binary_pattern(pattern_a, pattern_b)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const& pt) const override
                {
                        auto pt_x_floor  = fast_floor(pt.x());
                        auto fraction    = pt.x() - pt_x_floor;
                        auto color_a_val = color_a(pt);
                        auto color_b_val = color_b(pt);
                        auto delta_color = color_b_val - color_a_val;

                        return color_a_val + delta_color * fraction;
                }
        };

} // namespace raytracer

#endif /// RAYTRACER_GRADIENT_PATTERN_HPP__
