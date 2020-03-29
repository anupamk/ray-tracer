#ifndef RAYTRACER_CONSTANTS_HPP__
#define RAYTRACER_CONSTANTS_HPP__

/// c++ includes
#include <cstdint>

namespace raytracer
{
        /// floating point approx comparison
        constexpr auto EPSILON = 1.0e-5;

        /// max + min color values
        constexpr uint8_t PPM_MAX_COLOR_VALUE = 255; /// for colors ≥ 1.0
        constexpr uint8_t PPM_MIN_COLOR_VALUE = 0;   /// for colors ≤ 0.0

        /// a line in ppm file cannot be longer than this
        constexpr uint8_t PPM_MAX_LINE_LENGTH = 70;

        /// the value of Π
        constexpr double PI = 3.14159265358979323846;
} // namespace raytracer

#endif // RAYTRACER_CONSTANTS_HPP__
