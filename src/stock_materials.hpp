#ifndef RAYTRACER_STOCK_MATERIALS_HPP__
#define RAYTRACER_STOCK_MATERIALS_HPP__

/// our includes
#include "color.hpp"
#include "material.hpp"

namespace raytracer
{
	/*
	 * this defines a 'catalogue', if you will, of materials that can be
	 * used in various settings, rather than discovering these over and over
	 * again.
	 **/

	/// --------------------------------------------------------------------
	/// "glassy" materials
	material create_material_transparent_glass();
	material create_material_colored_glass(color const&);

	/// --------------------------------------------------------------------
	/// "matte" material
	material create_material_matte(color const&);

	/// --------------------------------------------------------------------
	/// "noisy" material (based on perlin noise)
} // namespace raytracer

#endif /// RAYTRACER_STOCK_MATERIALS_HPP__
