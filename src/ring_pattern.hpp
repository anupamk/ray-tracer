#ifndef RAYTRACER_RING_PATTERN_HPP__
#define RAYTRACER_RING_PATTERN_HPP__

/// c++ includes
#include <sstream>
#include <string>

/// our includes
#include "pattern_interface.hpp"
#include "color.hpp"
#include "binary_pattern.hpp"

namespace raytracer
{
	/*
         * this class implements the ring pattern i.e. colors/pattern
         * alternating in a ring.
        **/
	class ring_pattern final : public binary_pattern<ring_pattern>
	{
	    public:
		ring_pattern(color a, color b)
		    : binary_pattern(a, b)
		{
		}

		ring_pattern(std::shared_ptr<pattern_interface> pattern_a,
		             std::shared_ptr<pattern_interface> pattern_b)
		    : binary_pattern(pattern_a, pattern_b)
		{
		}

	    public:
		/// ------------------------------------------------------------
		/// return the color at a specific point on the shape.
		color color_at_point(tuple const& pt) const override
		{
			auto pt_x_squared = pt.x() * pt.x();
			auto pt_z_squared = pt.z() * pt.z();
			auto magnitude    = std::sqrt(pt_x_squared + pt_z_squared);

			if (fast_floor(magnitude) % 2 == 0) {
				return color_a(pt);
			}

			return color_b(pt);
		}
	};

} // namespace raytracer

#endif /// RAYTRACER_GRADIENT_PATTERN_HPP__
