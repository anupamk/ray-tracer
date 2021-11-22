#ifndef COMMON_INCLUDE_ASSERT_UTILS_H__
#define COMMON_INCLUDE_ASSERT_UTILS_H__

/* system includes */
#include <assert.h>

/* our includes */
#include "common/include/logging.h"

// clang-format off

/* generate a stringified representation of a 'name' */
#define STRINGIFY__(name) #name

#define ASSERT_MESSAGE__(cond, msg, ...)                                \
        if (!(cond)) {                                                  \
                LOG_FATAL(msg, ##__VA_ARGS__);                          \
                                                                        \
                /* die die die */                                       \
                assert(cond);                                           \
        }

/*
 * macros that we can call
**/
#define ASSERT(cond)     ASSERT_MESSAGE__(cond, STRINGIFY__(cond))
#define ASSERT_FAIL(msg) ASSERT_MESSAGE__(0, msg)

// clang-format on

#endif
