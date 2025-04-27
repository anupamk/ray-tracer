/*
 * implementation of platform specific thread utils. this is the implementation
 * for the linux platform.
 **/

#include "thread_utils.hpp"

/// system includes
#include <pthread.h>
#include <sched.h>
#include <stddef.h>

namespace platform_utils
{
        /// --------------------------------------------------------------------
        /// force || pin threads to cores
        int thread_utils::set_thread_affinity(pthread_t thread_handle, uint16_t cpu_id)
        {
                cpu_set_t cpuset;
                CPU_ZERO(&cpuset);
                CPU_SET(cpu_id, &cpuset);

                return pthread_setaffinity_np(thread_handle, sizeof(cpuset), &cpuset);
        }

} // namespace platform_utils
