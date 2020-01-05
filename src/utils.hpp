#ifndef RAYTRACER_UTILS_HPP__
#define RAYTRACER_UTILS_HPP__

/// c++ includes
#include <cmath>

/// our includes
#include "constants.hpp"

namespace raytracer
{
        ///
        /// this function returns true if 'a' is approx. equal to 'b', false
        /// otherwise.
        ///
        constexpr bool epsilon_equal(float a, float b)
        {
                const auto abs_diff = std::abs(a - b);
                return (abs_diff < EPSILON);
        }
} // namespace raytracer

#endif
