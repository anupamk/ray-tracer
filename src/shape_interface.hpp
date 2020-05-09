#ifndef RAYTRACER_SHAPE_INTERFACE_HPP__
#define RAYTRACER_SHAPE_INTERFACE_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <optional>

/// our includes
#include "intersection_record.hpp"
#include "matrix.hpp"
#include "material.hpp"

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
		virtual std::optional<intersection_records> intersect(ray_t const& R) const = 0;

		///
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

		///
		/// this function is called to return the normal at a point on
		/// the shape in object-space coordinates
		///
		virtual tuple normal_at_local(tuple const&) const = 0;

		///
		/// this function is called to return the normal at a point on
		/// the shape in world-space coordinates
		///
		tuple normal_at_world(tuple const&) const;

		///
		/// adjust material properties of a shape
		///
		material get_material() const;
		void set_material(material const&);
	};
} // namespace raytracer

#endif /// RAYTRACER_SHAPE_INTERFACE_HPP__
