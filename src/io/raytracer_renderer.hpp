#pragma once


/*
 * this file contains declarations for routines commonly used for rendering a
 * specific scene.
 **/

/// c++ includes
#include <string>

/// our includes
#include "io/camera.hpp"
#include "io/world.hpp"

namespace raytracer
{
        /*
         * single threaded rendering of a scene. rendered canvas is returned
         * on completion.
         **/
        canvas single_threaded_renderer(raytracer::world,   /// the world
                                        raytracer::camera); /// the camera

        /*
         * multi threaded rendering of a scene. rendered canvas is returned
         * on completion.
         **/
        canvas multi_threaded_renderer(uint32_t,           /// num-hw-threads
                                       raytracer::world,   /// the world
                                       raytracer::camera); /// the camera

} // namespace raytracer
