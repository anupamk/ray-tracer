#pragma once

/// c++ includes
#include <ostream>
#include <string>

namespace raytracer
{
        /*
         * this describes a 2d point on a texture. where
         *    - u : varies from 0.0 .. 1.0 along x-axis
         *    - v : varies from 0.0 .. 1.0 along y-axis
         **/
        class uv_point final
        {
            private:
                double const u_;
                double const v_;

            public:
                constexpr explicit uv_point(double u, double v)
                    : u_(u)
                    , v_(v)
                {
                }

            public:
                /// get the values out
                constexpr double u() const
                {
                        return this->u_;
                }

                constexpr double v() const
                {
                        return this->v_;
                }

            public:
                /// stringified representation of a uv-value
                std::string stringify() const;
        };

        std::ostream& operator<<(std::ostream& os, uv_point const& a);

} // namespace raytracer
