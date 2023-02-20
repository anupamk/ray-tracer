#pragma once

/// c++ includes
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

/// our includes
#include "patterns/binary_pattern.hpp"
#include "patterns/pattern_interface.hpp"
#include "primitives/color.hpp"

namespace raytracer
{
        /*
         * this class implements the blended-pattern where color is half the sum
         * of individual colors.
         **/
        class blended_pattern final : public binary_pattern<blended_pattern>
        {
            public:
                blended_pattern(color a, color b)
                    : binary_pattern(a, b)
                {
                }

                blended_pattern(std::shared_ptr<pattern_interface> pattern_a,
                                std::shared_ptr<pattern_interface> pattern_b)
                    : binary_pattern(pattern_a, pattern_b)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const& pt) const override
                {
                        return (color_a(pt) + color_b(pt)) * 0.5;
                }
        };

} // namespace raytracer
