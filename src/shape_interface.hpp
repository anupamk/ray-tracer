#ifndef RAYTRACER_SHAPE_INTERFACE_HPP__
#define RAYTRACER_SHAPE_INTERFACE_HPP__

/// c++ includes
#include <memory>
#include <optional>

/// our includes
#include "intersection_record.hpp"
#include "matrix.hpp"

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

	    public:
		shape_interface();

	    public:
		///
		/// this function is called to return zero or more intersections
		/// of a shape with a ray 'R'
		///
		virtual std::optional<intersection_records> intersect(ray_t const& r) const = 0;

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
	};
} // namespace raytracer

#endif /// RAYTRACER_SHAPE_INTERFACE_HPP__
