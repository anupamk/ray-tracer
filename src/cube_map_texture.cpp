/// c++ includes
#include <functional>
#include <memory>

/// our includes
#include "cube_map_texture.hpp"

namespace raytracer
{
	/// --------------------------------------------------------------------
	/// create an instance of a cube-map
	cube_texture::cube_texture(std::shared_ptr<uv_pattern_interface> left,
	                           std::shared_ptr<uv_pattern_interface> front,
	                           std::shared_ptr<uv_pattern_interface> right,
	                           std::shared_ptr<uv_pattern_interface> back,
	                           std::shared_ptr<uv_pattern_interface> up,
	                           std::shared_ptr<uv_pattern_interface> down)
	    : pattern_left_face_(left)
	    , pattern_front_face_(front)
	    , pattern_right_face_(right)
	    , pattern_back_face_(back)
	    , pattern_up_face_(up)
	    , pattern_down_face_(down)
	{
	}

	/// ------------------------------------------------------------
	/// return the color at a specific point on the pattern
	color cube_texture::color_at_point(tuple const& pt) const
	{
		switch (face_from_point(pt)) {
		case CUBE_FACE_LEFT:
			return pattern_left_face_->uv_pattern_color_at(uv_map_left(pt));

		case CUBE_FACE_FRONT:
			return pattern_front_face_->uv_pattern_color_at(uv_map_front(pt));

		case CUBE_FACE_RIGHT:
			return pattern_right_face_->uv_pattern_color_at(uv_map_right(pt));

		case CUBE_FACE_BACK:
			return pattern_back_face_->uv_pattern_color_at(uv_map_back(pt));

		case CUBE_FACE_UP:
			return pattern_up_face_->uv_pattern_color_at(uv_map_up(pt));

		case CUBE_FACE_DOWN:
			return pattern_down_face_->uv_pattern_color_at(uv_map_down(pt));

		default:
			ASSERT_FAIL("invalid face");
		};

		ASSERT_FAIL("invalid face");
	}

	/*
	 * only private members from this point onwards
	 **/

	/// --------------------------------------------------------------------
	/// this function is called to find the face upon which the point lies
	cube_face cube_texture::face_from_point(tuple const& pt) const
	{
		auto const abs_pt_x   = std::abs(pt.x());
		auto const abs_pt_y   = std::abs(pt.y());
		auto const abs_pt_z   = std::abs(pt.z());
		auto const max_abs_pt = std::max(abs_pt_x, std::max(abs_pt_y, abs_pt_z));

		if (max_abs_pt == pt.x()) {
			return CUBE_FACE_RIGHT;
		}

		if (max_abs_pt == -pt.x()) {
			return CUBE_FACE_LEFT;
		}

		if (max_abs_pt == pt.y()) {
			return CUBE_FACE_UP;
		}

		if (max_abs_pt == -pt.y()) {
			return CUBE_FACE_DOWN;
		}

		if (max_abs_pt == pt.z()) {
			return CUBE_FACE_FRONT;
		}

		return CUBE_FACE_BACK;
	}

	/// --------------------------------------------------------------------
	/// map a point on the cube to a corresponding uv-point.

	/// --------------------------------------------------------------------
	/// map a point on the front-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_front(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: -1.0 ≤ x ≤ +1.0 and
		/// v: -1.0 ≤ y ≤ +1.0

		auto const u = modulus((1.0 + pt.x()), 2.0) / 2.0;
		auto const v = modulus((1.0 + pt.y()), 2.0) / 2.0;

		return uv_point(u, v);
	}

	/// --------------------------------------------------------------------
	/// map a point on the back-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_back(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: +1.0 ≤ x ≤ -1.0 and
		/// v: -1.0 ≤ y ≤ +1.0

		auto const u = modulus((1.0 - pt.x()), 2.0) / 2.0;
		auto const v = modulus((1.0 + pt.y()), 2.0) / 2.0;

		return uv_point(u, v);
	}

	/// --------------------------------------------------------------------
	/// map a point on the up-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_up(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: -1.0 ≤ x ≤ +1.0 and
		/// v: -1.0 ≤ z ≤ +1.0

		auto const u = modulus((1.0 + pt.x()), 2.0) / 2.0;
		auto const v = modulus((1.0 - pt.z()), 2.0) / 2.0;

		return uv_point(u, v);
	}

	/// --------------------------------------------------------------------
	/// map a point on the down-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_down(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: +1.0 ≤ x ≤ -1.0 and
		/// v: -1.0 ≤ z ≤ +1.0

		auto const u = modulus((-1.0 + pt.x()), 2.0) / 2.0;
		auto const v = modulus((1.0 + pt.z()), 2.0) / 2.0;

		return uv_point(u, v);
	}

	/// --------------------------------------------------------------------
	/// map a point on the up-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_left(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: -1.0 ≤ z ≤ +1.0 and
		/// v: -1.0 ≤ y ≤ +1.0

		auto const u = modulus((1.0 + pt.z()), 2.0) / 2.0;
		auto const v = modulus((1.0 + pt.y()), 2.0) / 2.0;

		return uv_point(u, v);
	}

	/// --------------------------------------------------------------------
	/// map a point on the down-face of the cube to corresponding uv point
	uv_point cube_texture::uv_map_right(tuple const& pt) const
	{
		/// ------------------------------------------------------------
		/// u: +1.0 ≤ z ≤ -1.0 and
		/// v: -1.0 ≤ y ≤ +1.0

		auto const u = modulus((1.0 - pt.z()), 2.0) / 2.0;
		auto const v = modulus((1.0 + pt.y()), 2.0) / 2.0;

		return uv_point(u, v);
	}

} // namespace raytracer
