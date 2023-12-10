/*
 * implement the raytracer matrix transformations
 **/

#include "primitives/matrix_transformations.hpp"

/// c++ includes
#include <cmath>

/// our includes
#include "primitives/matrix.hpp"
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this function is called to create a translation matrix. a
        /// translation matrix 'translates' a point i.e. moves a point to a new
        /// location.
        fsize_dense2d_matrix_t
        matrix_transformations_t::create_3d_translation_matrix(double x, /// x-translate
                                                               double y, /// y-translate
                                                               double z) /// z-translate
        {
                auto translation_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                translation_matrix(0, 3) = x;
                translation_matrix(1, 3) = y;
                translation_matrix(2, 3) = z;

                return translation_matrix;
        }

        /// --------------------------------------------------------------------
        /// this function is called to create a scaling matrix. a scaling matrix
        /// 'scales' a point i.e. makes an object larger / smaller
        fsize_dense2d_matrix_t matrix_transformations_t::create_3d_scaling_matrix(double x, /// x-scale
                                                                                  double y, /// y-scale
                                                                                  double z) /// z-scale
        {
                auto scaling_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                scaling_matrix(0, 0) = x;
                scaling_matrix(1, 1) = y;
                scaling_matrix(2, 2) = z;

                return scaling_matrix;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that rotates a point
        /// about the x-axis.
        fsize_dense2d_matrix_t matrix_transformations_t::create_rotx_matrix(double alpha)
        {
                auto rotx_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                /// ------------------------------------------------------------
                /// matrix for rotation about x-axis looks like so:
                ///    columns   0      1         2       3
                ///    row-0  { 1.0 , 0.0    , 0.0     , 0.0 }
                ///    row-1  { 0.0 , cos(α) , -sin(α) , 0.0 }
                ///    row-2  { 0.0 , sin(α) , cos(α)  , 0.0 }
                ///    row-3  { 0.0 , 0.0    , 0.0     , 1.0 }
                rotx_matrix(1, 1) = std::cos(alpha);
                rotx_matrix(1, 2) = -std::sin(alpha);
                rotx_matrix(2, 1) = std::sin(alpha);
                rotx_matrix(2, 2) = std::cos(alpha);

                return rotx_matrix;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that rotates a point
        /// about the y-axis.
        fsize_dense2d_matrix_t matrix_transformations_t::create_roty_matrix(double alpha)
        {
                auto roty_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                /// ------------------------------------------------------------
                /// matrix for rotation about y-axis looks like so:
                ///    columns   0         1      2       3
                ///    row-0  { cos(α)  , 0.0 , sin(α) , 0.0 }
                ///    row-1  { 0.0     , 1.0 , 0.0    , 0.0 }
                ///    row-2  { -sin(α) , 0.0 , cos(α) , 0.0 }
                ///    row-3  { 0.0     , 0.0 , 0.0    , 1.0 }
                ///
                roty_matrix(0, 0) = std::cos(alpha);
                roty_matrix(0, 2) = std::sin(alpha);
                roty_matrix(2, 0) = -std::sin(alpha);
                roty_matrix(2, 2) = std::cos(alpha);

                return roty_matrix;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that rotates a point
        /// about the z-axis.
        fsize_dense2d_matrix_t matrix_transformations_t::create_rotz_matrix(double alpha)
        {
                auto rotz_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                /// ------------------------------------------------------------
                /// matrix for rotation about z-axis looks like so:
                ///    columns   0      1         2       3
                ///    row-0  { cos(α) , -sin(α) , 0.0 , 0.0 }
                ///    row-1  { sin(α) , cos(α)  , 0.0 , 0.0 }
                ///    row-2  { 0.0    , 0.0     , 1.0 , 0.0 }
                ///    row-3  { 0.0    , 0.0     , 0.0 , 1.0 }
                rotz_matrix(0, 0) = std::cos(alpha);
                rotz_matrix(0, 1) = -std::sin(alpha);
                rotz_matrix(1, 0) = std::sin(alpha);
                rotz_matrix(1, 1) = std::cos(alpha);

                return rotz_matrix;
        }

        /*
         * reflection about x/y/z axis is same as scaling by a negative value.
         **/

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that reflects a point
        /// about x-axis.
        fsize_dense2d_matrix_t matrix_transformations_t::create_reflect_x_matrix()
        {
                return matrix_transformations_t::create_3d_scaling_matrix(-1.0, 1.0, 1.0);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that reflects a point
        /// about y-axis.
        fsize_dense2d_matrix_t matrix_transformations_t::create_reflect_y_matrix()
        {
                return matrix_transformations_t::create_3d_scaling_matrix(1.0, -1.0, 1.0);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a matrix, that reflects a point
        /// about z-axis.
        ///
        fsize_dense2d_matrix_t matrix_transformations_t::create_reflect_z_matrix()
        {
                return matrix_transformations_t::create_3d_scaling_matrix(1.0, 1.0, -1.0);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a shearing-matrix.
        ///
        /// a shearing-matrix 'shears' a  point, where each component in a tuple
        /// is transformed in proportion to the other two components.
        fsize_dense2d_matrix_t matrix_transformations_t::create_shearing_matrix(double xy, double xz,
                                                                                double yx, double yz,
                                                                                double zx, double zy)
        {
                auto shear_matrix = fsize_dense2d_matrix_t::create_identity_matrix(4);

                /// ------------------------------------------------------------
                /// shear matrix looks like so:
                ///    columns   0    1     2      3
                ///    row-0  { 1.0 , xy  , xz  , 0.0 }
                ///    row-1  { yx  , 1.0 , yz  , 0.0 }
                ///    row-2  { zx  , zy  , 1.0 , 0.0 }
                ///    row-3  { 0.0 , 0.0 , 0.0 , 1.0 }
                shear_matrix(0, 1) = xy;
                shear_matrix(0, 2) = xz;
                shear_matrix(1, 0) = yx;
                shear_matrix(1, 2) = yz;
                shear_matrix(2, 0) = zx;
                shear_matrix(2, 1) = zy;

                return shear_matrix;
        }

        /// --------------------------------------------------------------------
        /// this function is called to create a view-transform matrix. which
        /// basically, orients the world relative to our eye, allowing us to
        /// easily line up the image/world...
        fsize_dense2d_matrix_t
        matrix_transformations_t::create_view_transform(tuple from_point, tuple to_point, tuple up_vector)
        {
                auto const forward     = normalize(to_point - from_point);
                auto const norm_up_vec = normalize(up_vector);
                auto const left_vec    = cross(forward, norm_up_vec);
                auto const true_up_vec = cross(left_vec, forward);

                /// ------------------------------------------------------------
                /// setup the view-transformation matrix as follows
                ///
                ///       left_vec.x    ,     left_vec.y ,  left_vec.z    ,  0
                ///       true_up_vec.x ,  true_up_vec.y ,  true_up_vec.z ,  0
                ///       -forward.x    ,     -forward.y ,  forward.z     ,  0
                ///       0             ,              0 ,             0  ,  1

                auto view_xform_mat = fsize_dense2d_matrix_t::create_identity_matrix(4);

                /// row-0
                view_xform_mat(0, 0) = left_vec.x();
                view_xform_mat(0, 1) = left_vec.y();
                view_xform_mat(0, 2) = left_vec.z();
                view_xform_mat(0, 3) = 0.0;

                /// row-1
                view_xform_mat(1, 0) = true_up_vec.x();
                view_xform_mat(1, 1) = true_up_vec.y();
                view_xform_mat(1, 2) = true_up_vec.z();
                view_xform_mat(1, 3) = 0.0;

                /// row-2
                view_xform_mat(2, 0) = -forward.x();
                view_xform_mat(2, 1) = -forward.y();
                view_xform_mat(2, 2) = -forward.z();
                view_xform_mat(2, 3) = 0.0;

                /// row-3
                view_xform_mat(3, 0) = 0.0;
                view_xform_mat(3, 1) = 0.0;
                view_xform_mat(3, 2) = 0.0;
                view_xform_mat(3, 3) = 1.0;

                // clang-format off
                return (view_xform_mat * create_3d_translation_matrix(-from_point.x(),
                                                                      -from_point.y(),
                                                                      -from_point.z()));
                // clang-format on
        }

} // namespace raytracer
