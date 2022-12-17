#pragma once


/// c++ includes
#include <sstream>
#include <string>

/// our includes
#include "color.hpp"
#include "pattern_interface.hpp"

namespace raytracer
{
        /*
         * this class implements the canonical solid pattern i.e. one where
         * color is a constant throughout
         **/
        class solid_pattern final : public pattern_interface
        {
            private:
                color c_;

            public:
                solid_pattern(color c = color_white())
                    : c_(c)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the shape.
                color color_at_point(tuple const&) const override
                {
                        return this->c_;
                }
        };

} // namespace raytracer


