/*
 *  implement raytracer shape material
**/

/// c++ includes
#include <ios>
#include <ostream>
#include <sstream>
#include <string>

/// our includes
#include "material.hpp"

namespace raytracer
{
	material::material()
	    : ambient_(0.1)
	    , diffuse_(0.9)
	    , specular_(0.9)
	    , shininess_(200.0)
	    , color_(color_white())
	{
	}

	float material::get_ambient() const
	{
		return this->ambient_;
	}

	float material::get_diffuse() const
	{
		return this->diffuse_;
	}

	float material::get_specular() const
	{
		return this->specular_;
	}

	float material::get_shininess() const
	{
		return this->shininess_;
	}

	color material::get_color() const
	{
		return this->color_;
	}

	material& material::set_ambient(float val)
	{
		this->ambient_ = val;
		return *this;
	}

	material& material::set_diffuse(float val)
	{
		this->diffuse_ = val;
		return *this;
	}

	material& material::set_specular(float val)
	{
		this->specular_ = val;
		return *this;
	}

	material& material::set_shininess(float val)
	{
		this->shininess_ = val;
		return *this;
	}

	material& material::set_color(color val)
	{
		this->color_ = val;
		return *this;
	}

	///
	/// stringified representation of the material
	///
	std::string material::stringify() const
	{
		std::stringstream ss("");

		// clang-format off
                ss << std::fixed << std::left
                   << "ambient: "   << this->get_ambient()   << ", "
                   << "diffuse: "   << this->get_diffuse()   << ", "
                   << "specular: "  << this->get_specular()  << ", "
                   << "shininess: " << this->get_shininess() << ", "
                   << "color: ("    << this->get_color()     << ")";
		// clang-format on

		return ss.str();
	}

	std::ostream& operator<<(std::ostream& os, material const& M)
	{
		return os << M.stringify();
	}

	bool operator==(material const& lhs, material const& rhs)
	{
		// clang-format off
                return ((lhs.get_ambient()   == rhs.get_ambient())   &&
                        (lhs.get_diffuse()   == rhs.get_diffuse())   &&
                        (lhs.get_specular()  == rhs.get_specular())  &&
                        (lhs.get_shininess() == rhs.get_shininess()) &&
                        (lhs.get_color()     == rhs.get_color()));
		// clang-format on
	}

} // namespace raytracer
