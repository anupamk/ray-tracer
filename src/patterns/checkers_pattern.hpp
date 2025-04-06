#pragma once

/// c++ includes
#include <sstream>
#include <string>

/// our includes
#include "patterns/binary_pattern.hpp"
#include "patterns/pattern_interface.hpp"
#include "primitives/color.hpp"

namespace raytracer
{
        /*
         * this class implements the checkers or the chessboard pattern.
         **/
        class checkers_pattern final : public binary_pattern<checkers_pattern>
        {
            public:
                checkers_pattern(color a, color b)
                    : binary_pattern(a, b)
                {
                }

                checkers_pattern(std::shared_ptr<pattern_interface> pattern_a,
                                 std::shared_ptr<pattern_interface> pattern_b)
                    : binary_pattern(pattern_a, pattern_b)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const& pt) const override
                {
                        auto pt_x_floor   = fast_floor(pt.x());
                        auto pt_y_floor   = fast_floor(pt.y());
                        auto pt_z_floor   = fast_floor(pt.z());
                        auto pt_xyz_floor = pt_x_floor + pt_y_floor + pt_z_floor;

                        if ((pt_xyz_floor % 2) == 0) {
                                return color_a(pt);
                        }

                        return color_b(pt);
                }
        };

} // namespace raytracer