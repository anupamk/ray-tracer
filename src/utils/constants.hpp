#pragma once

/// c++ includes
#include <cmath>
#include <cstdint>
#include <limits>

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// floating point approx comparison
        constexpr auto EPSILON       = 1.0e-5;
        constexpr auto EPSILON_BY_2F = EPSILON / 2.0;

        /// --------------------------------------------------------------------
        /// max + min color values
        constexpr uint8_t PPM_MAX_COLOR_VALUE = 255; /// for colors ≥ 1.0
        constexpr uint8_t PPM_MIN_COLOR_VALUE = 0;   /// for colors ≤ 0.0

        /// --------------------------------------------------------------------
        /// a line in ppm file cannot be longer than this
        constexpr uint8_t PPM_MAX_LINE_LENGTH = 70;

        /// --------------------------------------------------------------------
        /// π specific constants
        constexpr double PI       = 3.14159265358979323846;
        constexpr double PI_BY_2F = PI / 2.0;
        constexpr double PI_BY_3F = PI / 3.0;
        constexpr double PI_BY_4F = PI / 4.0;
        constexpr double PI_BY_5F = PI / 5.0;
        constexpr double PI_BY_6F = PI / 6.0;

        /// --------------------------------------------------------------------
        /// some commonly used square-roots
        constexpr double SQRT_2       = 1.4142135;
        constexpr double SQRT_2_BY_2F = SQRT_2 / 2.0;
        constexpr double SQRT_3       = 1.7320508;
        constexpr double SQRT_3_BY_3F = SQRT_3 / 3.0;

        /// --------------------------------------------------------------------
        /// easy access numeric constants, IEEE 754 is needed
        static_assert(std::numeric_limits<double>::is_iec559, "IEEE 754 required");
        constexpr double INF = std::numeric_limits<double>::infinity();

        /// --------------------------------------------------------------------
        /// a common place for all textures and OBJ files
        constexpr auto TEXTURE_ROOT = "../../assets/textures/";
        constexpr auto OBJ_ROOT     = "../../assets/OBJ/";

} // namespace raytracer
