#ifndef RAYTRACER_PATTERN_INTERFACE_HPP__
#define RAYTRACER_PATTERN_INTERFACE_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <string>

/// our includes
#include "matrix.hpp"
#include "color.hpp"
#include "tuple.hpp"

namespace raytracer
{
	/// forward decl
	class shape_interface;

	/*
         * this defines a common pattern interface, which is then implemented by
         * every new type of pattern that we support.
        **/
	class pattern_interface : public std::enable_shared_from_this<pattern_interface>
	{
	    private:
		fsize_dense2d_matrix_t xform_;
		fsize_dense2d_matrix_t inv_xform_;

	    public:
		pattern_interface();

		/// ------------------------------------------------------------
		/// associate a new transformation matrix with the pattern
		void transform(fsize_dense2d_matrix_t const&);

	    public:
		/// ------------------------------------------------------------
		/// return the pattern-color at a specific point on the shape
		virtual color color_at_point(tuple const&) const = 0;

	    public:
		/// ------------------------------------------------------------
		/// return the pattern-color at a specific point on a shape
		color color_at_shape(std::shared_ptr<shape_interface const>, tuple const&) const;

		/// ------------------------------------------------------------
		/// return the transform matrix for the pattern
		fsize_dense2d_matrix_t transform() const;

		/// ------------------------------------------------------------
		/// return the inverse-transform matrix for the pattern
		fsize_dense2d_matrix_t inv_transform() const;
	};
} // namespace raytracer

#endif /// RAYTRACER_PATTERN_INTERFACE_HPP__
