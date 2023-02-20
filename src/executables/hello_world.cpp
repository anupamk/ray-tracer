#include "common/include/logging.h"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

int main(int argc, char** argv)
{
        LOG_INFO("hello world from:'%s'", argv[0]);

        return 0;
}
