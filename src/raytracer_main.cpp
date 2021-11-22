/// our includes
#include "common/include/logging.h"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

int main(int argc, char** argv)
{
        LOG_INFO("starting ray-tracing. it will be awesome !");
        return 0;
}
