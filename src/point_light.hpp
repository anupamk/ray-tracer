#pragma once


/// c++ includes
#include <ostream>
#include <string>

/// our includes
#include "color.hpp"
#include "tuple.hpp"

namespace raytracer
{
        /*
         * this defines a 'point-light' type i.e. a light source with no size
         * and existing at a single point in space.
         **/
        class point_light final
        {
            private:
                color color_val_;
                tuple position_;

            public:
                point_light(tuple const& position, color const& color);

            public:
                color get_color() const;
                tuple position() const;

            public:
                /// stringified representation
                std::string stringify() const;
        };

        std::ostream& operator<<(std::ostream& os, point_light const& L);
        bool operator==(point_light const& lhs, point_light const& rhs);
} // namespace raytracer


