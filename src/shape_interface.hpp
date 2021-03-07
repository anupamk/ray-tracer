#ifndef RAYTRACER_SHAPE_INTERFACE_HPP__
#define RAYTRACER_SHAPE_INTERFACE_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <optional>
#include <string>

/// our includes
#include "badge.hpp"
#include "intersection_record.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "uv_point.hpp"

namespace raytracer
{
	/// forward decl
	class ray_t;

	/*
	 * this defines the common shape_interface which is implemented by
	 * concrete shape instsances f.e. sphere etc.
	 **/
	class shape_interface : public std::enable_shared_from_this<shape_interface>
	{
	    private:
		fsize_dense2d_matrix_t xform_;
		fsize_dense2d_matrix_t inv_xform_;
		material material_;

	    public:
		shape_interface();

	    public:
		///
		/// this function is called to return zero or more intersections
		/// of a shape with a ray 'R'
		///
		virtual std::optional<intersection_records> intersect(the_badge<ray_t>,
		                                                      ray_t const& R) const = 0;

		/// ------------------------------------------------------------
		/// this function is called to return the normal at a point on
		/// the shape in object-space coordinates
		virtual tuple normal_at_local(tuple const&) const = 0;

		/// ------------------------------------------------------------
		/// stringified representation of a shape
		virtual std::string stringify() const = 0;

		/// ------------------------------------------------------------
		/// map a point on the surface of the shape to a corresponding
		/// uv-value (on a texture)
		virtual uv_point map_to_uv(tuple const&) const = 0;

	    public:
		/// ------------------------------------------------------------
		/// this function is called to return the current transform
		/// matrix associated with the shape
		///
		fsize_dense2d_matrix_t transform() const;

		///
		/// this function is called to return the current inverse
		/// transform matrix associated with the shape
		///
		fsize_dense2d_matrix_t inv_transform() const;

		///
		/// this function is called to associate a new transformation
		/// matrix with the shape
		///
		void transform(fsize_dense2d_matrix_t const& M);

		/// return true if a ray can intersect a shape before
		/// 'distance', useful for determining shadows etc.
		///
		/// return 'false' otherwise
		virtual bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
		                                     double distance) const = 0;

		/// ------------------------------------------------------------
		/// get || set a shape's ability to cast shadows
		bool cast_shadow() const
		{
			return cast_shadow_;
		}

		void cast_shadow(bool val)
		{
			cast_shadow_ = val;
		}
	};
} // namespace raytracer

#endif /// RAYTRACER_SHAPE_INTERFACE_HPP__
