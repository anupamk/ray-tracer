#ifndef RAYTRACER_UV_MAPPER_HPP__
#define RAYTRACER_UV_MAPPER_HPP__

/// our includes
#include "tuple.hpp"
#include "uv_point.hpp"

namespace raytracer
{
	/*
	 * this file describes uv-mapper routines for various shapes. uv-mapper
	 * is a function (in true sense of that word) mapping a 3d-point (on a
	 * shape) to a corresponding uv-point on the texture.
	 *
	 * thus, all uv-mapper routines look like instances of this:
	 *        std::function<uv_point(tuple const&)>
	 *
	 * once a uv-point is obtained, the texture color (at that uv-point) is
	 * applied to the surface that is being rendered.
	 **/

	/**
	 * @brief
	 * map a point 'P' on the surface of a sphere to a corresponding
	 * uv-point on the texture
	 **/
	uv_point spherical_map(tuple const&);

	/**
	 * @brief
	 * map a point 'P' on the surface of a plane to a corresponding
	 * uv-point on the texture
	 **/
	uv_point planar_map(tuple const&);

	/// add more mappings ↑

} // namespace raytracer

#endif
