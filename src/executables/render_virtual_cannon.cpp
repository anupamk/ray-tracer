/// c++ includes
#include <cstddef>
#include <memory>

/// our includes
#include "common/include/logging.h"
#include "io/canvas.hpp"
#include "primitives/color.hpp"
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"

/*
 * an environment contains both gravity and wind.
 **/
struct env_params {
        raytracer::tuple gravity_;
        raytracer::tuple wind_;

        env_params(raytracer::tuple gravity, raytracer::tuple wind)
            : gravity_(gravity)
            , wind_(wind)
        {
        }

        raytracer::tuple gravity() const
        {
                return this->gravity_;
        }

        raytracer::tuple wind() const
        {
                return this->wind_;
        }
};

/*
 * a projectile has both a position and a velocity
 **/
struct projectile {
        raytracer::tuple pos_;
        raytracer::tuple velocity_;

        projectile(raytracer::tuple position, raytracer::tuple velocity)
            : pos_(position)
            , velocity_(velocity)
        {
        }

        raytracer::tuple position() const
        {
                return this->pos_;
        }

        raytracer::tuple velocity() const
        {
                return this->velocity_;
        }
};

static projectile tick(env_params env, projectile proj);

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

int main(int argc, char** argv)
{
        /// --------------------------------------------------------------------
        /// convenience alias
        namespace RT = raytracer;

        /// create projectile
        auto const start_point = RT::create_point(0.0, 1.0, 0.0);
        auto velocity          = RT::normalize(RT::create_vector(78.5, 102.0, 0.0)) * 12.25;
        projectile proj(start_point, velocity);

        /// create the environment
        auto const gravity = RT::create_vector(0.0, -0.1, 0.0);
        auto const wind    = raytracer::create_vector(-0.01, 0.0, 0.0);
        env_params env(gravity, wind);

        /// and the canvas where everything will be shown
        auto canvas = RT::canvas::create_binary(RT::canvas::X_PIXELS, RT::canvas::Y_PIXELS);

        for (; proj.position().y() > 0;) {
                auto const proj_pos    = proj.position();
                const size_t proj_xpos = proj_pos.x();
                const size_t proj_ypos = proj_pos.y();

                /// ------------------------------------------------------------
                /// the (0, 0) of the canvas is at the top-left. however the
                /// viewing coordinates have (0, 0) at the bottom-left.
                ///
                /// so, we subtract y-position of the projectil from
                /// canvas.height()
                canvas.write_pixel(proj_xpos, canvas.height() - proj_ypos, RT::color_white());

                /// next round
                proj = tick(env, proj);
        }

        /// now write the canvas data out.
        auto const img_fname = "virtual-cannon.ppm";
        canvas.write(img_fname);

        LOG_INFO("image-file: '%s' generated", img_fname);

        return 0;
}

/*
 * this function is called to simulate the movement of the projectile for a
 * single tick.
 *
 * it returns the new position of the projectile
 **/
static projectile tick(env_params env, projectile proj)
{
        auto const new_pos = proj.position() + proj.velocity();
        auto const new_vel = proj.velocity() + env.gravity() + env.wind();

        return projectile(new_pos, new_vel);
}
