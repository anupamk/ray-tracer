#ifndef RAYTRACER_MATERIAL_HPP__
#define RAYTRACER_MATERIAL_HPP__

/// c++ includes
#include <ostream>
#include <string>

/// our includes
#include "color.hpp"

namespace raytracer
{
	/*
         * this defines the material which makes up a shape. it is not just
         * restricted to surface color, but encapsulates attributes from the
         * phong reflection model.
        **/
	class material
	{
	    private:
		float ambient_;
		float diffuse_;
		float specular_;
		float shininess_;
		color color_;

	    public:
		explicit material();

	    public:
		/// getters
		float get_ambient() const;
		float get_diffuse() const;
		float get_specular() const;
		float get_shininess() const;
		color get_color() const;

		/// setters
		material& set_ambient(float val);
		material& set_diffuse(float val);
		material& set_specular(float val);
		material& set_shininess(float val);
		material& set_color(color val);

	    public:
		/// stringified representation of the material
		std::string stringify() const;
	};

	bool operator==(material const& lhs, material const& rhs);
	std::ostream& operator<<(std::ostream& os, material const& M);

} // namespace raytracer

#endif /// RAYTRACER_MATERIAL_HPP__
