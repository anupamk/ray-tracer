#pragma once


/// c++ includes
#include <cstdint>

/// our includes
#include "color.hpp"
#include "utils.hpp"
#include "uv_pattern_interface.hpp"

namespace raytracer
{
        class uv_checkers final : public uv_pattern_interface
        {
            private:
                color u_color_;
                uint16_t width_;
                color v_color_;
                uint16_t height_;

            public:
                uv_checkers(uint16_t num_u_squares, color u_color, uint16_t num_v_squares, color v_color)
                    : u_color_(u_color)
                    , width_(num_u_squares)
                    , v_color_(v_color)
                    , height_(num_v_squares)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// compute the color at a specific point on the pattern
                color uv_pattern_color_at(uv_point const& uv) const override
                {
                        auto const u2 = fast_floor(uv.u() * this->width_);
                        auto const v2 = fast_floor(uv.v() * this->height_);

                        if ((u2 + v2) % 2 == 0) {
                                return u_color_;
                        }

                        return v_color_;
                }
        };
} // namespace raytracer


