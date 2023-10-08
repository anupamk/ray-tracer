#pragma once

/// c++ includes
#include <optional>
#include <thread> /// for std::thread::hardware_concurrency(...)

/// our includes
#include "constants.hpp"
#include "execution_profiler.hpp"

namespace raytracer
{

        /// ----------------------------------------------------------------------------
        /// gcc'izms
#ifndef likely

#define likely(x) __builtin_expect(!!(x), 1)

#endif /// likely

#ifndef unlikely

#define unlikely(x) __builtin_expect(!!(x), 0)

#endif /// unlikely

        /// ----------------------------------------------------------------------------
        /// are we building on gcc ?
#if defined(__GNUC__) && !defined(__clang__)

#define HAS_GCC_COMPILER 1 /// yes

#else

#define HAS_GCC_COMPILER 0 /// no

#endif

        /// --------------------------------------------------------------------
        /// this function returns true if 'a' is approximately equal to 'b',
        /// false otherwise.
        constexpr bool epsilon_equal(float a, float b)
        {
                auto const abs_diff = std::abs(a - b);
                return (abs_diff < EPSILON);
        }

        /// --------------------------------------------------------------------
        /// cast between unrelated types, hopefully used sparingly...
        template <typename T, typename U>
        inline T punned_cast(U* ptr)
        {
                uintptr_t x = reinterpret_cast<uintptr_t>(ptr);
                return reinterpret_cast<T>(x);
        }

#if HAS_GCC_COMPILER

        /// --------------------------------------------------------------------
        /// some gcc-inline-asm phun
        inline double asm_sqrt_1(double n)
        {
                double ret = 0.0;

                asm volatile("movq %1, %%xmm0 \n"
                             "sqrtsd %%xmm0, %%xmm1 \n"
                             "movq %%xmm1, %0 \n"
                             : "=r"(ret)
                             : "g"(n)
                             : "xmm0", "xmm1", "memory");

                return ret;
        }

#endif /// HAS_GCC_COMPILER

#if HAS_GCC_COMPILER

        /// --------------------------------------------------------------------
        /// just use the floating-point instruction directly...
        inline double asm_sqrt_2(double n)
        {
                double result;

                asm volatile("fsqrt\n\t" // st(0) = square root st(0)
                             : "=t"(result)
                             : "0"(n));

                return result;
        }

#endif /// HAS_GCC_COMPILER

        /// --------------------------------------------------------------------
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
        inline std::optional<std::pair<double const, double const>> const
        quadratic_real_roots(double A, double B, double C)
        {
                PROFILE_SCOPE;

                auto const discriminant = B * B - 4.0 * A * C;

                /// only complex roots
                if (discriminant < 0) {
                        return std::nullopt;
                }

                /// equal real roots
                if (epsilon_equal(discriminant, 0.0)) {
                        double const r_1 = -B / (2.0 * A);
                        double const r_2 = r_1;

                        return std::pair(r_1, r_2);
                }

                auto const sqrt_discriminant = std::sqrt(discriminant);
                auto const two_A             = 2.0 * A;
                auto const two_C             = 2.0 * C;

                /// case-1 : B ≥ 0.0
                if (B >= 0.0) {
                        double const tmp = (-B - sqrt_discriminant);
                        double const r_1 = tmp / two_A;
                        double const r_2 = two_C / tmp;

                        return std::pair(r_1, r_2);
                }

                /// case-2 : B < 0.0
                double const tmp = (-B + sqrt_discriminant);
                double const r_1 = two_C / tmp;
                double const r_2 = tmp / two_A;

                return std::pair(r_1, r_2);
        }

        /// --------------------------------------------------------------------
        /// ensure that 'value' lies in [min_val, max_val] range
        template <typename T>
        constexpr typename std::enable_if_t<std::is_arithmetic_v<T>, T> clamp_in_range(T value, T min_val,
                                                                                       T max_val)
        {
                return std::max(min_val, std::min(value, max_val));
        }

        /// --------------------------------------------------------------------
        /// faster floor function ?
        constexpr int32_t fast_floor(float val)
        {
                int32_t int_val = static_cast<int32_t>(val);
                return (val < int_val) ? (int_val - 1) : (int_val);
        }

        /// --------------------------------------------------------------------
        /// compute floating-point modulus
        inline double modulus(double num, double denom)
        {
                auto const val_mod = std::fmod(num, denom);

                if (val_mod < 0.0) {
                        return val_mod + denom;
                }

                return val_mod;
        }

        /// --------------------------------------------------------------------
        /// return the number of hw-threads on the system
        inline uint32_t max_cores()
        {
                return std::thread::hardware_concurrency();
        }

        /// --------------------------------------------------------------------
        /// return a stringfied boolean value i.e. 'yes' / 'no' for easier to
        /// parse output
        inline const char* str_boolean(bool value)
        {
                return (value == true) ? "yes" : "no";
        }

} // namespace raytracer
