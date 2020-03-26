#ifndef RAYTRACER_UTILS_HPP__
#define RAYTRACER_UTILS_HPP__

/// c++ includes
#include <algorithm>
#include <cmath>
#include <optional>
#include <type_traits>
#include <utility>

/// our includes
#include "constants.hpp"

namespace raytracer
{
///
/// gcc'izms
///
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

        ///
        /// this function returns true if 'a' is approximately equal to 'b',
        /// false otherwise.
        ///
        constexpr bool epsilon_equal(float a, float b)
        {
                const auto abs_diff = std::abs(a - b);
                return (abs_diff < EPSILON);
        }

        /// real roots of a quadratic equation
        using real_roots_t = std::pair<double, double>;

        ///
        /// This function is called to return the real roots of the canonical
        /// quadratic equation:
        ///
        ///    A.x^2 + B.x + C = 0.0 .......................... (1)
        ///                     ___________
        ///    r_1, r_2 = -B ± √ B^2 - 4AC) / (2.0 * A) ....... (1.sol)
        ///
        /// The term, (B^2 - 4AC) is popularly known as the DISCRIMINANT.
        ///
        /// When quantities of the same sign are subtracted, some loss in
        /// precision may be expected. This is a particular concern here if AC
        /// is relatively small compared to B^2, in which case B has about the
        /// same magnitude as the DISCRIMINANT.
        ///
        /// After slightly massaging the above equation (1) we can write it as
        /// follows:
        ///
        ///    C.X^2 + B.X + A = 0.0, where X == 1/x .......... (2)
        ///                                  __________
        ///    r_1, r_2 = (2.0 * C) / (-B ± √ B^2 - 4AC) ...... (2.sol)
        ///
        /// So, depending on the value of B we can do the following:
        ///
        ///     case-1 : B ≥ 0.0
        ///         r_1 = (-B - SQRT(DISCRIMINANT)) / (2.0 * A) and
        ///         r_2 = (2.0 * C) / (-B - SQRT(DISCRIMINANT))
        ///
        ///     case-2 : B < 0.0
        ///         r_1 = (2.0 * C) / (-B + SQRT(DISCRIMINANT)) and
        ///         r_2 = (-B + SQRT(DISCRIMINANT)) / (2.0 * A)
        ///
        /// No extra work is required in using the above, as opposed to blindly
        /// using either (1.sol) or (2.sol)
        ///
        inline const std::optional<real_roots_t> quadratic_real_roots(double A, double B, double C)
        {
                const auto discriminant = B * B - 4.0 * A * C;

                /// only complex roots
                if (discriminant < 0) {
                        return std::nullopt;
                }

                /// equal real roots
                if (epsilon_equal(discriminant, 0.0)) {
                        const auto r_1 = -B / (2.0 * A);
                        const auto r_2 = r_1;

                        return real_roots_t{r_1, r_2};
                }

                /// non-equal real roots
                const auto sqrt_discriminant = std::sqrt(discriminant);
                const auto two_A	     = 2.0 * A;
                const auto two_C	     = 2.0 * C;

                /// case-1 : B ≥ 0.0
                if (B >= 0.0) {
                        const auto tmp = (-B - sqrt_discriminant);
                        const auto r_1 = tmp / two_A;
                        const auto r_2 = two_C / tmp;

                        return real_roots_t{r_1, r_2};
                }

                /// case-2 : B < 0.0
                const auto tmp = (-B + sqrt_discriminant);
                const auto r_1 = two_C / tmp;
                const auto r_2 = tmp / two_A;

                return real_roots_t{r_1, r_2};
        }

        /// --------------------------------------------------------------------
        /// ensure that 'value' lies in [min_val, max_val] range
        template <typename T>
        constexpr typename std::enable_if_t<std::is_arithmetic_v<T>, T>
        clamp_in_range(T value, T min_val, T max_val)
        {
                return std::max(min_val, std::min(value, max_val));
        }

} // namespace raytracer

#endif
