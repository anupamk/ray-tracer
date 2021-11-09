#ifndef RAYTRACER_STRIPED_PATTERN_HPP__
#define RAYTRACER_STRIPED_PATTERN_HPP__

/// c++ includes
#include <cmath>
#include <sstream>
#include <string>

/// our includes
#include "binary_pattern.hpp"
#include "color.hpp"
#include "pattern_interface.hpp"

namespace raytracer
{
        /*
         * this class implements the striped pattern i.e. color/pattern
         * alternating from one to next.
         **/
        class striped_pattern final : public binary_pattern<striped_pattern>
        {
            public:
                striped_pattern(color a, color b)
                    : binary_pattern(a, b)
                {
                }

                striped_pattern(std::shared_ptr<pattern_interface> pattern_a,
                                std::shared_ptr<pattern_interface> pattern_b)
                    : binary_pattern(pattern_a, pattern_b)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const& pt) const override
                {
                        auto pt_x = fast_floor(pt.x());
                        if (pt_x % 2 == 0) {
                                return color_a(pt);
                        }

                        return color_b(pt);
                }
        };

} // namespace raytracer

#endif /// RAYTRACER_STRIPED_PATTERN_HPP__
