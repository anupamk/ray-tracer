/*
 * implement single-threaded rendering of a scene.
 **/

/// our includes
#include "raytracer_renderer.hpp"

namespace raytracer
{
	/*
	 * this implements a single threaded rendering of scene in a world 'W'
	 * that is looked at by a camera 'C' with the generated image stored in
	 * a file 'dst_fname'
	 **/
	canvas single_threaded_renderer(raytracer::world W,  /// the world
	                                raytracer::camera C) /// the camera
	{
		return C.render(W);
	}

} // namespace raytracer
