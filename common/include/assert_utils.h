#ifndef COMMON_INCLUDE_ASSERT_UTILS_H__
#define COMMON_INCLUDE_ASSERT_UTILS_H__

/* system includes */
#include <assert.h>

/* our includes */
#include "common/include/logging.h"

/* generate a stringified representation of a 'name' */
#define STRINGIFY__(name) #name

// clang-format off
#define ASSERT_MESSAGE__(cond, msg, ...)                \
        if (!(cond)) {                                  \
                LOG_FATAL(msg, ##__VA_ARGS__);          \
                                                        \
                /* die die die */                       \
                assert(cond);                           \
        }
// clang-format on

/*
 * macros that we can call
 **/
#define ASSERT(cond) ASSERT_MESSAGE__(cond, STRINGIFY__(cond))
#define ASSERT_FAIL_LOG(fmt, ...) ASSERT_MESSAGE__(0, fmt, ##__VA_ARGS__)
#define ASSERT_FAIL(msg) ASSERT_FAIL_LOG("%s", msg)

#endif
