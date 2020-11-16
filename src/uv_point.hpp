#ifndef RAYTRACER_UV_POINT_HPP__
#define RAYTRACER_UV_POINT_HPP__

/// c++ includes
#include <ostream>
#include <stdexcept>
#include <string>

/// our includes
#include "assert_utils.h"

namespace raytracer
{
	/*
         * this describes a 2d point on a texture. where
         *    - u : varies from 0.0 .. 1.0 along x-axis
         *    - v : varies from 0.0 .. 1.0 along y-axis
        **/
	class uv_point
	{
	    private:
		double const u_;
		double const v_;

	    public:
		constexpr explicit uv_point(double u, double v)
		    : u_(u)
		    , v_(v)
		{
			ASSERT((u >= 0.0) && (u <= 1.0));
			ASSERT((v >= 0.0) && (v <= 1.0));
		}

	    public:
		/// get the values out
		constexpr double u() const
		{
			return this->u_;
		}

		constexpr double v() const
		{
			return this->v_;
		}

	    public:
		/// stringified representation of a uv-value
		std::string stringify() const;
	};

	std::ostream& operator<<(std::ostream& os, uv_point const& a);

} // namespace raytracer

#endif // RAYTRACER_UV_POINT_HPP__
