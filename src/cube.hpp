#ifndef RAYTRACER_CUBE_HPP__
#define RAYTRACER_CUBE_HPP__

/// c++ includes
#include <optional>
#include <ostream>
#include <string>
#include <tuple>

/// our includes
#include "assert_utils.h"
#include "intersection_record.hpp"
#include "ray.hpp"
#include "shape_interface.hpp"
#include "tuple.hpp"
#include "uv_point.hpp"

namespace raytracer
{
	/*
	 * this defines a cube with each side of 2 units length. origin is
	 * centered within the cube such that its extremeties are at
	 *     - bottom-left : (-1.0, -1.0, -1.0)
	 *     - top-right   : (1.0, 1.0, 1.0)
	 **/
	class cube final : public shape_interface
	{
	    public:
		cube(bool cast_shadow = true);

	    public:
		/// ------------------------------------------------------------
		/// compute intersection of a ray with the plane
		std::optional<intersection_records> intersect(the_badge<ray_t>,
		                                              ray_t const& R) const override;

		/// ------------------------------------------------------------
		/// normal vector at a give point on the cube (in local
		/// coordinates)
		tuple normal_at_local(tuple const&) const override;

		/// ------------------------------------------------------------
		/// compute intersection of a ray with the plane, and return
		/// 'true' iff 'R' intersects before 'distance'.
		///
		/// return 'false' otherwise
		bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
		                             double distance) const override;

		/// ------------------------------------------------------------
		/// stringified representation of a cube
		std::string stringify() const override;

	    private:
		/// ------------------------------------------------------------
		/// actual workhorse for computing ray-cube intersections
		std::optional<intersection_records> compute_intersections_(ray_t const&) const;

		/// ------------------------------------------------------------
		/// for each of the x,y,z axes check where a ray intersects
		/// corresponding planes.
		std::tuple<double, double> check_axes_(double origin, double direction) const;
	};

} // namespace raytracer

#endif /// RAYTRACER_CUBE_HPP__
