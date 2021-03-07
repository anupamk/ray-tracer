/*
 * implement the raytracer plane
 **/

/// c++ includes
#include "constants.hpp"
#include "tuple.hpp"

/// our includes
#include "intersection_record.hpp"
#include "plane.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace raytracer
{
	plane::plane()
	{
	}

	/// --------------------------------------------------------------------
	/// stringified representation of a plane
	std::string plane::stringify() const
	{
		std::stringstream ss("");

		ss << "XZ-PLANE";

		return ss.str();
	}

	/// --------------------------------------------------------------------
	/// compute intersection of a ray with the plane
	std::optional<intersection_records> plane::intersect(the_badge<ray_t>, ray_t const& R) const
	{
		auto const ray_y_dir = R.direction().y();

		if (std::abs(ray_y_dir) < EPSILON) {
			return std::nullopt;
		}

		/// ------------------------------------------------------------
		/// a ray can intersect the plane at only a single
		/// point. co-planar rays are not interesting at all
		auto const xs_point = -R.origin().y() / ray_y_dir;

		return intersection_records{intersection_record(xs_point, shared_from_this())};
	}

	/// --------------------------------------------------------------------
	/// normal vector at any point on the plane is a constant.
	tuple plane::normal_at_local(tuple const&) const
	{
		/// for an xz-plane, the normal is always along the y-axis
		return create_vector(0.0, 1.0, 0.0);
	}

	/// --------------------------------------------------------------------
	/// this function is called to map a point on the plane to corresponding
	/// uv-value (on a texture)
	uv_point plane::map_to_uv(tuple const& pt) const
	{
		auto const u_val = modulus(pt.x(), 1.0);
		auto const v_val = modulus(pt.z(), 1.0);

		return uv_point(u_val, v_val);
	}

	/// --------------------------------------------------------------------
	/// 'reasonably' formatted information of the sphere
	std::ostream& operator<<(std::ostream& os, plane const& P)
	{
		return os << P.stringify();
	}

} // namespace raytracer
