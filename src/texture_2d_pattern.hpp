#ifndef RAYTRACER_TEXTURE_2D_PATTERN_HPP__
#define RAYTRACER_TEXTURE_2D_PATTERN_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <string>

/// our includes
#include "matrix.hpp"
#include "pattern_interface.hpp"
#include "shape_interface.hpp"
#include "uv_pattern_interface.hpp"

namespace raytracer
{
	/// forward decl
	class shape_interface;

	/*
	 * this defines a 2d texture pattern, which ofcourse needs to be
	 * associated with a shape (around/over which) it will be 'wrapped'.
	 **/
	class texture_2d_pattern final : public pattern_interface
	{
	    private:
		std::shared_ptr<shape_interface> shape_;
		std::shared_ptr<uv_pattern_interface> pattern_;

	    public:
		texture_2d_pattern(std::shared_ptr<uv_pattern_interface> uv_pattern,
		                   std::shared_ptr<shape_interface> shape)
		    : shape_(shape)
		    , pattern_(uv_pattern)
		{
		}

	    public:
		/// ------------------------------------------------------------
		/// return the color at a specific point on the shape
		color color_at_point(tuple const& pt) const override
		{
			auto const uv_pt = shape_->map_to_uv(pt);
			return pattern_->uv_pattern_color_at(uv_pt);
		}
	};
} // namespace raytracer

#endif /// RAYTRACER_TEXTURE_2D_PATTERN_HPP__
