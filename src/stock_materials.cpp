/*
 * implement raytracer stock materials
 **/

/// our includes
#include "stock_materials.hpp"
#include "solid_pattern.hpp"

namespace raytracer
{
	/// --------------------------------------------------------------------
	/// this function is called to return an instance of a "white glass"
	/// material.
	material create_material_transparent_glass()
	{
		return create_material_colored_glass(color(0.1, 0.1, 0.1));
	}

	/// --------------------------------------------------------------------
	/// this function is called to return an instance of a colored glass
	/// material.
	material create_material_colored_glass(color const& c)
	{
		auto solid_color_pattern = std::make_shared<solid_pattern>(c);
		return material()
		        .set_ambient(0.0)
		        .set_diffuse(0.4)
		        .set_specular(0.9)
		        .set_shininess(300)
		        .set_reflective(0.9)
		        .set_transparency(0.9)
		        .set_refractive_index(material::RI_GLASS)
		        .set_pattern(std::move(solid_color_pattern));
	}

	/// --------------------------------------------------------------------
	/// this function is called to return an instance of a matte material.
	material create_material_matte(color const& c)
	{
		auto solid_color_pattern = std::make_shared<solid_pattern>(c);
		return material().set_pattern(std::move(solid_color_pattern));
	}

} // namespace raytracer
