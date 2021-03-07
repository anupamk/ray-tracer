/*
 * implement raytracer point-light
 **/

/// c++ includes
#include <ostream>
#include <sstream>
#include <string>

/// our includes
#include "point_light.hpp"
#include "tuple.hpp"

namespace raytracer
{
	point_light::point_light(tuple const& position, color const& color)
	    : color_val_(color)
	    , position_(position)
	{
	}

	color point_light::get_color() const
	{
		return this->color_val_;
	}

	tuple point_light::position() const
	{
		return this->position_;
	}

	std::string point_light::stringify() const
	{
		std::stringstream ss("");

		// clang-format off
                ss << "position: (" << this->position()  << "), "
                   << "color: ("    << this->get_color() << ")";
		// clang-format on

		return ss.str();
	}

	std::ostream& operator<<(std::ostream& os, point_light const& L)
	{
		return os << L.stringify();
	}

	bool operator==(point_light const& lhs, point_light const& rhs)
	{
		// clang-format off
                return ((lhs.position()  == rhs.position()) &&
                        (lhs.get_color() == rhs.get_color()));
		// clang-format on
	}
} // namespace raytracer
