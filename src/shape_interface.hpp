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
	    public:
		/// ------------------------------------------------------------
		/// when set to 'false', the shape doesn't cast any shadow. by
		/// default all shapes cast a shadow.
		bool const cast_shadow;

	    private:
		/// ------------------------------------------------------------
		/// transformation matrices associated with a shape. allows for
		/// moving shapes around, deforming them etc. etc.
		fsize_dense2d_matrix_t xform_;
		fsize_dense2d_matrix_t inv_xform_;

		/// ------------------------------------------------------------
		/// the material which makes up the shape
		material material_;

	    protected:
		/// ------------------------------------------------------------
		/// don't allow deletion through a base
		virtual ~shape_interface()
		{
		}

	    public:
		shape_interface(bool cast_shadow);

		/// ------------------------------------------------------------
		/// this function is called to return zero or more intersections
		/// of a shape with a ray 'R'
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
		/// return true if a ray can intersect a shape before
		/// 'distance', useful for determining shadows etc.
		///
		/// return 'false' otherwise
		virtual bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
		                                     double distance) const = 0;

	    public:
		/// ------------------------------------------------------------
		/// this function is called to return the current transform
		/// matrix associated with the shape
		fsize_dense2d_matrix_t transform() const;

		/// ------------------------------------------------------------
		/// this function is called to return the current inverse
		/// transform matrix associated with the shape
		fsize_dense2d_matrix_t inv_transform() const;

		/// ------------------------------------------------------------
		/// this function is called to associate a new transformation
		/// matrix with the shape
		void transform(fsize_dense2d_matrix_t const& M);

		/// ------------------------------------------------------------
		/// this function is called to return the normal at a point on
		/// the shape in world-space coordinates
		tuple normal_at_world(tuple const&) const;

		/// ------------------------------------------------------------
		/// convert world-space coordinates into local
		tuple world_to_local(tuple const&) const;

		/// ------------------------------------------------------------
		/// adjust material properties of a shape
		material get_material() const;
		void set_material(material const&);
	};
} // namespace raytracer

#endif /// RAYTRACER_SHAPE_INTERFACE_HPP__
