#ifndef RAYTRACER_MATERIAL_HPP__
#define RAYTRACER_MATERIAL_HPP__

/// c++ includes
#include <memory>
#include <ostream>
#include <string>

/// our includes
#include "color.hpp"
#include "pattern_interface.hpp"

namespace raytracer
{
	/*
         * this defines the material which makes up a shape. it contains
         * attributes from the phong reflection model as well the pattern that
         * makes up the material.
        **/
	class material
	{
	    private:
		/// ------------------------------------------------------------
		/// phong reflection-model properties
		float ambient_;
		float diffuse_;
		float specular_;
		float shininess_;

		/// ------------------------------------------------------------
		/// material pattern
		std::shared_ptr<pattern_interface> pattern_;

	    public:
		explicit material();

	    public:
		/// getters
		float get_ambient() const;
		float get_diffuse() const;
		float get_specular() const;
		float get_shininess() const;
		color get_color(std::shared_ptr<shape_interface const>, tuple const&) const;
		std::shared_ptr<pattern_interface> get_pattern() const;

		/// setters
		material& set_ambient(float val);
		material& set_diffuse(float val);
		material& set_specular(float val);
		material& set_shininess(float val);
		material& set_pattern(std::shared_ptr<pattern_interface> val);

	    public:
		/// stringified representation of the material
		std::string stringify() const;
	};

        /// --------------------------------------------------------------------
        /// few useful overloaded operators
	bool operator==(material const& lhs, material const& rhs);
	std::ostream& operator<<(std::ostream& os, material const& M);

} // namespace raytracer

#endif /// RAYTRACER_MATERIAL_HPP__
