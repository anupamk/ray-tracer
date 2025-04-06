#pragma once

/// c++ includes
#include <functional>
#include <memory>

/// our includes
#include "patterns/pattern_interface.hpp"
#include "patterns/uv_pattern_interface.hpp"

namespace raytracer
{
        /*
         * a "texture" is a pattern that needs to be wrapped and mapped on a
         * shape.
         *
         * other than a pattern, it needs a 'mapper' function as well, which
         * transforms points in 3d-space to corresponding points on the
         * texture.
         *
         * from here on, the shape is colored with the color of the point on the
         * texture.
         **/
        class texture_2d_pattern final : public pattern_interface
        {
            private:
                std::function<uv_point(tuple const&)> uv_mapper_;
                std::shared_ptr<uv_pattern_interface> pattern_;

            public:
                texture_2d_pattern(std::shared_ptr<uv_pattern_interface> uv_pattern,
                                   std::function<uv_point(tuple const&)> uv_mapper)
                    : uv_mapper_(uv_mapper)
                    , pattern_(uv_pattern)
                {
                }

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the pattern
                color color_at_point(tuple const& pt) const override
                {
                        auto const uv_pt = uv_mapper_(pt);
                        return pattern_->uv_pattern_color_at(uv_pt);
                }
        };

} // namespace raytracer