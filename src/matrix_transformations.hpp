#ifndef RAYTRACER_SRC_MATRIX_TRANSFORMATIONS_HPP__
#define RAYTRACER_SRC_MATRIX_TRANSFORMATIONS_HPP__

/// our includes
#include "assert_utils.h"
#include "matrix.hpp"
#include "tuple.hpp"

namespace raytracer
{
	/*
	 * this implements various matrix transformations
	 **/
	class matrix_transformations_t
	{
	    public:
		static fsize_dense2d_matrix_t create_3d_translation_matrix(double x,  /// x-translate
		                                                           double y,  /// y-translate
		                                                           double z); /// z-translate

		static fsize_dense2d_matrix_t create_3d_scaling_matrix(double x,  /// x-scale
		                                                       double y,  /// y-scale
		                                                       double z); /// z-scale

		static fsize_dense2d_matrix_t create_rotx_matrix(double alpha);
		static fsize_dense2d_matrix_t create_roty_matrix(double alpha);
		static fsize_dense2d_matrix_t create_rotz_matrix(double alpha);

		static fsize_dense2d_matrix_t create_reflect_x_matrix();
		static fsize_dense2d_matrix_t create_reflect_y_matrix();
		static fsize_dense2d_matrix_t create_reflect_z_matrix();

		// clang-format off

                /// shearing transform changes each component in a tuple in
                /// proportion to the other two components
                static fsize_dense2d_matrix_t create_shearing_matrix(double xy, double xz,
                                                                     double yx, double yz,
                                                                     double zx, double zy);
		// clang-format on

		static fsize_dense2d_matrix_t create_view_transform(tuple from_point, tuple to_point,
		                                                    tuple up_vector);
	};
} // namespace raytracer

#endif /// RAYTRACER_SRC_MATRIX_TRANSFORMATIONS_HPP__
