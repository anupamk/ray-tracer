/*
 * implement the raytracer uv_point for procedural textures
**/

/// c++ includes
#include <ios>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

/// our includes
#include "uv_point.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// stringified representation of a uv_point
        std::string uv_point::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << std::fixed << std::left
                   << "u: " << this->u() << ", "
                   << "v: " << this->v();
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted output
        std::ostream& operator<<(std::ostream& os, uv_point const& a)
        {
                return os << a.stringify();
        }

} // namespace raytracer
