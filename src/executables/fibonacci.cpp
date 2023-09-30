#include <cmath>
#include <cstdint>
#include <cstdio>

/// our includes
#include "utils/execution_profiler.hpp"

/**
 * @brief
 *    return fibonacci of 'N'
 **/
static int fib(uint32_t N);

int main(int argc, char** argv)
{
        PROFILE_SCOPE;

        constexpr uint32_t fib_start = 1;
        constexpr uint32_t fib_end   = 32;

        uint32_t fib_sum = 0;
        for (uint32_t N = fib_start; N <= fib_end; N++) {
                fib_sum += fib(N);
        }

        printf("fib-sum['%u' .. '%u'] == %u\n", fib_start, fib_end, fib_sum);

        return 0;
}

/// ----------------------------------------------------------------------------
/// a really terrible implementation
static int fib(uint32_t N)
{
        PROFILE_SCOPE;

        if (N < 2) {
                return N;
        }

        auto fib_n_1 = fib(N - 1);
        auto fib_n_2 = fib(N - 2);

        return fib_n_1 + fib_n_2;
}
