#pragma once

/// c++ includes
#include <memory>

/// our includes
#include "primitives/color.hpp"
#include "primitives/uv_point.hpp"

namespace raytracer
{
        /*
         * this class defines an interface for all uv patterns.
         *
         * uv-patterns can be either mathematically defined f.e. a checkers
         * pattern, polka-dot pattern etc. or it can be an image as well.
         **/
        class uv_pattern_interface : std::enable_shared_from_this<uv_pattern_interface>
        {
            protected:
                /// ------------------------------------------------------------
                /// don't allow deletion through a base
                virtual ~uv_pattern_interface()
                {
                }

            public:
                virtual color uv_pattern_color_at(uv_point const& uv) const = 0;
        };

} // namespace raytracer