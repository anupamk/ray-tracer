#ifndef RAYTRACER_UV_CUBE_MAP_HPP__
#define RAYTRACER_UV_CUBE_MAP_HPP__

/// c++ includes
#include <cstdint>

/// our includes
#include "color.hpp"
#include "utils.hpp"
#include "uv_pattern_interface.hpp"

namespace raytracer
{
        /*
         * this defines a cube-map-pattern i.e. map different patterns on 6
         * faces of the cube.
         *
         * cube faces are expressed in canonical cross-format notation as
         * follows:
         *
         *        +---+                     U: up     (+y)
         *        | U |                     L: left   (-x)
         *    +---+---+---+---+             F: front  (+z)
         *    | L | F | R | B |             R: right  (+x)
         *    +---+---+---+---+             B: bottom (-z)
         *        | D |                     D: down   (-y)
         *        +---+
         **/
        class uv_cube_map final : public uv_pattern_interface
        {
            private:
                color const up_;
                color const down_;
                color const left_;
                color const right_;
                color const front_;
                color const back_;

            public:
                /// ------------------------------------------------------------
                /// group faces together for a _marginally_ better interface.
                uv_cube_map(std::pair<color, color> up_down_color,    /// up-down-faces
                            std::pair<color, color> left_right_color, /// left-right-faces
                            std::pair<color, color> front_back_color) /// front-back-faces
                    : up_(up_down_color.first)
                    , down_(up_down_color.second)
                    , left_(left_right_color.first)
                    , right_(left_right_color.second)
                    , front_(front_back_color.first)
                    , back_(front_back_color.second)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// compute the color at a specific point on the pattern
                color uv_pattern_color_at(uv_point const& uv) const override
                {
                }
        };
} // namespace raytracer

#endif /// RAYTRACER_UV_CUBE_MAP_HPP__
