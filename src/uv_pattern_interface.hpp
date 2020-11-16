#ifndef RAYTRACER_UV_PATTERN_INTERFACE_HPP__
#define RAYTRACER_UV_PATTERN_INTERFACE_HPP__

/// c++ includes
#include <memory>

/// our includes
#include "color.hpp"
#include "uv_point.hpp"

namespace raytracer
{
	/*
         * this class defines an interface for all uv patterns.
         *
         * uv-patterns can be either mathematically defined f.e. a checkers
         * pattern, polka-dot pattern etc. or it can be an image as well.
        **/
	class uv_pattern_interface : std::enable_shared_from_this<uv_pattern_interface>
	{
	    public:
		virtual color uv_pattern_color_at(uv_point const& uv) const = 0;
	};
} // namespace raytracer

#endif /// RAYTRACER_UV_PATTERN_INTERFACE_HPP__
