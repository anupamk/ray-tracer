/*
 * implement the raytracer color
 **/

/// c++ includes
#include <ios>
#include <ostream>
#include <sstream>
#include <string>

/// our includes
#include "color.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// implement 'a += b' where both 'a' and 'b' are colors
        color& color::operator+=(color other)
        {
                const auto new_r = this->R() + other.R();
                const auto new_g = this->G() + other.G();
                const auto new_b = this->B() + other.B();

                this->rgb_ = create_point(new_r, new_g, new_b);
                return *this;
        }

        /// --------------------------------------------------------------------
        /// implement 'a -= b' where both 'a' and 'b' are colors
        color& color::operator-=(color other)
        {
                const auto new_r = this->R() - other.R();
                const auto new_g = this->G() - other.G();
                const auto new_b = this->B() - other.B();

                this->rgb_ = create_point(new_r, new_g, new_b);
                return *this;
        }

        /// --------------------------------------------------------------------
        /// stringified representation of a color
        std::string color::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << std::fixed << std::left
                   << "r: " << this->R() << ", "
                   << "g: " << this->G() << ", "
                   << "b: " << this->B()
                   ;
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// we are better off reducing the number of functions that need access
        /// to class internals, so, just define binary '+', '-' in terms of '+='
        /// and '-='.
        color operator+(color a, color b)
        {
                return a += b;
        }

        color operator-(color a, color b)
        {
                return a -= b;
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted output for color
        std::ostream& operator<<(std::ostream& os, color const& C)
        {
                return os << C.stringify();
        }

} // namespace raytracer
