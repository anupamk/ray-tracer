#pragma once

/// our includes
#include "patterns/uv_pattern_interface.hpp"
#include "primitives/color.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        /*
         * this class implements the align-check pattern. this pattern looks
         * something like this:
         *
         *             +--+--------------------------------------+--+
         *             |ul|                                      |ur|
         *             +--+                                      +--+
         *             |                                            |
         *             |                                            |
         *             |                main-color                  |
         *             |                                            |
         *             |                                            |
         *             +--+                                      +--+
         *          0.2|bl|                                      |br|
         *             +--+--------------------------------------+--+
         *              0.2
         **/
        class align_check final : public uv_pattern_interface
        {
            private:
                /// ------------------------------------------------------------
                /// dimensions of corner square and consequently, the remainder
                /// of the pattern as well
                static constexpr float square_size    = 0.2;
                static constexpr float remainder_size = 1.0 - square_size;

                /// ------------------------------------------------------------
                /// the colors that make up this pattern
                color const main_color_;
                color const ul_color_;
                color const ur_color_;
                color const bl_color_;
                color const br_color_;

            public:
                align_check(color main_color,   /// main-color
                            color ul, color ur, /// upper-color
                            color bl, color br) /// bottom-color
                    : main_color_(main_color)
                    , ul_color_(ul)
                    , ur_color_(ur)
                    , bl_color_(bl)
                    , br_color_(br)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the pattern
                color uv_pattern_color_at(uv_point const& uv) const override
                {
                        if (uv.v() > remainder_size) {
                                if (uv.u() > remainder_size) {
                                        return ur_color_;
                                }

                                if (uv.u() < square_size) {
                                        return ul_color_;
                                }
                        } else if (uv.v() < square_size) {
                                if (uv.u() > remainder_size) {
                                        return br_color_;
                                }

                                if (uv.u() < square_size) {
                                        return bl_color_;
                                }
                        }

                        return main_color_;
                }
        };

} // namespace raytracer
