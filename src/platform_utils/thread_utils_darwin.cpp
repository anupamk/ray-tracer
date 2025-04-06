/*
 * implementation of platform specific thread utils. this is the implementation
 * for the darwin platform.
 **/

#include "thread_utils.hpp"

namespace platform_utils
{
        /// --------------------------------------------------------------------
        /// just a stub
        int thread_utils::set_thread_affinity(void*, uint16_t)
        {
                return 0;
        }

} // namespace platform_utils
