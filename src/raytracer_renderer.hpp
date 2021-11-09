#ifndef RAYTRACER_RENDERER_HPP__
#define RAYTRACER_RENDERER_HPP__

/*
 * this file contains declarations for routines commonly used for rendering a
 * specific scene.
 **/

/// c++ includes
#include <string>

/// our includes
#include "camera.hpp"
#include "world.hpp"

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
        canvas multi_threaded_renderer(raytracer::world,   /// the world
                                       raytracer::camera); /// the camera

} // namespace raytracer

#endif /// RAYTRACER_RENDERER_HPP__
