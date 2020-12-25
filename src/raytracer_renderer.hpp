#ifndef RAYTRACER_RENDERER_HPP__
#define RAYTRACER_RENDERER_HPP__

/*
 * this file contains declarations for routines commonly used for rendering a
 * specific scene.
 **/

/// c++ includes
#include <string>

/// our includes
#include "world.hpp"
#include "camera.hpp"

namespace raytracer
{
	/*
         * single threaded rendering of a scene. once the rendering is complete,
         * the rendered canvas is returned to the callee.
        **/
	canvas single_threaded_renderer(raytracer::world,   /// the world
	                                raytracer::camera); /// the camera

	/*
         * multi threaded rendering of a scene. once the rendering is complete,
         * the rendered canvas is returned to the callee.
        **/
	canvas multi_threaded_renderer(raytracer::world,   /// the world
	                               raytracer::camera); /// the camera

} // namespace raytracer

#endif /// RAYTRACER_RENDERER_HPP__
