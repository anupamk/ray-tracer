/// c++ includes
#include <iostream>

/// our includes
#include "logging.h"
#include "virtual_cannon.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

int main(int argc, char** argv)
{
        projectile proj(raytracer::create_point(0.0, 1.0, 0.0),	  // position
                        raytracer::create_vector(1.0, 1.0, 0.0)); // velocity

        const env_params environment(raytracer::create_vector(0.0, -0.1, 0.0),	 // gravity
                                     raytracer::create_vector(-0.01, 0.0, 0.0)); // wind

        int num_ticks = 0;
        for (; proj.position().y() > 0; ++num_ticks) {
                LOG_INFO("tick: %03d, projectile y-pos: %f", num_ticks, proj.position().y());

                proj = tick(environment, proj);
        }

        LOG_INFO("number of ticks before projectile hits ground: %d", num_ticks);

        return 0;
}
