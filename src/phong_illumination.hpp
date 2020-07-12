#ifndef RAYTRACER_PHONG_ILLUMINATION_HPP__
#define RAYTRACER_PHONG_ILLUMINATION_HPP__

/// our includes
#include "color.hpp"
#include "point_light.hpp"
#include "tuple.hpp"
#include "material.hpp"

namespace raytracer
{
	color phong_illumination(tuple const& surface_point,	    /// where ray intersects surface
				 material const& surface_material,  /// material
				 point_light const& incident_light, /// light illuminating the scene
				 tuple const& eye_vector,	    /// camera || viewer
				 tuple const& surface_normal,	    /// normal at intersection
				 bool is_shadowed = false);	    /// is the point shadowed ?
}

#endif /// RAYTRACER_PHONG_ILLUMINATION_HPP__
