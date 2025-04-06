#pragma once

#include <cstdint>

namespace platform_utils
{
        /*
         * this class implements the interface for hooking into platform
         * specific threading routines, and their interactions.
         *
         * pretty basic for starters.
         **/
        class thread_utils final
        {
            private:
                /*
                 * nothing here :-)
                 **/
            public:
                /*
                 * @brief
                 *    this function is called to set cpu affinity of a thread
                 *    instance.
                 *
                 * @return
                 *    '0' on success.
                 **/
                static int set_thread_affinity(void* native_thread_handle, uint16_t cpu_id);
        };

} // namespace platform_utils
