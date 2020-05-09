/// our includes
#include "virtual_cannon.hpp"

/*
 * this function is called to simulate the movement of the projectile for a
 * single tick.
 *
 * it returns the new position of the projectile
**/
projectile tick(env_params env, projectile proj)
{
        const auto new_pos = proj.position() + proj.velocity();
        const auto new_vel = proj.velocity() + env.gravity() + env.wind();

        return projectile(new_pos, new_vel);
}
