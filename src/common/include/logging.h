#pragma once

/* system includes */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>

/* c++ includes */
#include <string>

/*
 * return current time upto ms resolution in a static buffer
 **/
static inline std::string log_ts_string()
{
        struct timeval tv;
        gettimeofday(&tv, NULL);

        /* round off to nearest millisecond */
        uint16_t msec = lrint(tv.tv_usec / 1000.0);
        if (msec >= 1000) {
                msec -= 1000;
                tv.tv_sec += 1;
        }

        /* stringify */
        constexpr uint8_t bufsize = 64;
        char fmt_time_buf[bufsize];

        struct tm* tm_info  = localtime(&tv.tv_sec);
        size_t const offset = strftime(fmt_time_buf, bufsize, "%Y-%m-%d %H:%M:%S", tm_info);
        snprintf(fmt_time_buf + offset, (bufsize - offset), ".%03d", msec);

        return std::string(fmt_time_buf);
}

/*
 * various logging levels. no need to go full-monte and mimic syslog's
 * 8 levels. that just seems too inordinate.
 */
typedef enum {
        LOG_LEVEL_FATAL = 1,
        LOG_LEVEL_ERROR = 2,
        LOG_LEVEL_INFO  = 3,
        LOG_LEVEL_DEBUG = 4,
} log_level_t;

/* where do we log ? */
#ifndef LOG_DEST
#define LOG_DEST stderr
#endif

/* log level */
extern log_level_t GLOBAL_LOG_LEVEL_NOW;

/*
 * logging format string (left -> right)
 *    '%-6s'     -> log-level-string
 *    '%-23s'    -> current-time (upto ms resolution)
 *    '%-15s'    -> file-name
 *    '%05d:%s'  -> line:function
 **/
#define LOGGING_FMT__(fmt) "%-6s | %-23s | %-20s : %05d | " fmt "\n"

/*
 * evaluates to true iff log-level (ll) can be logged (in conjunction
 * with LOG_LEVEL_DEFAULT).
 *
 * as convention, if default-log-level == x, all logs from
 * log-level-[x..0] are logged.
 **/
#define LOGGING_ENABLED_AT_LEVEL(ll) ((ll) <= (GLOBAL_LOG_LEVEL_NOW))

// clang-format off
#define DO_LOG__(log_level, log_level_str, fmt, ...)                    \
        do {                                                            \
                if (LOGGING_ENABLED_AT_LEVEL(log_level)) {              \
                        fprintf(LOG_DEST,                               \
                                LOGGING_FMT__(fmt),                     \
                                log_level_str,                          \
                                log_ts_string().c_str(),                \
                                __FILE__,                               \
                                __LINE__,                               \
                                ##__VA_ARGS__);                         \
                                                                        \
                        /* performance...what's that ? */               \
                        fflush(LOG_DEST);                               \
                }                                                       \
        } while (0)
// clang-format on

/* convenience logging macros that we can call */
#define LOG_DEBUG(fmt, ...) DO_LOG__(LOG_LEVEL_DEBUG, "DEBUG", fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) DO_LOG__(LOG_LEVEL_INFO, "INFO", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) DO_LOG__(LOG_LEVEL_ERROR, "ERROR", fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) DO_LOG__(LOG_LEVEL_FATAL, "FATAL", fmt, ##__VA_ARGS__)
