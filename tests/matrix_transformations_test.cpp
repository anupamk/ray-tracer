/// c++ includes
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest/include/doctest.h"

/// our own thing
#include "constants.hpp"
#include "common/include/logging.h"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// conveniences
using rt_matrix_t        = raytracer::fsize_dense2d_matrix_t;
using rt_matrix_xforms_t = raytracer::matrix_transformations_t;

/// translation matrix creation test
TEST_CASE("translation matrix creation")
{
        constexpr auto xlate_x = 5.0;
        constexpr auto xlate_y = -3.0;
        constexpr auto xlate_z = 2.0;

        // clang-format off
        rt_matrix_t const expected_xlate_mat = {
                                         { 1.0, 0.0, 0.0, xlate_x },
                                         { 0.0, 1.0, 0.0, xlate_y },
                                         { 0.0, 0.0, 1.0, xlate_z },
                                         { 0.0, 0.0, 0.0, 1.0     }
        };
        // clang-format on

        rt_matrix_t const got_xlate_mat =
                rt_matrix_xforms_t::create_3d_translation_matrix(xlate_x,  /// translate-x
                                                                 xlate_y,  /// translate-y
                                                                 xlate_z); /// translate-z

        CHECK(expected_xlate_mat == got_xlate_mat);
}

/// translate a point test
TEST_CASE("translate a point test")
{
        /// point coordinates
        constexpr double pt_x = -3.0;
        constexpr double pt_y = 4.0;
        constexpr double pt_z = 5.0;

        /// translation amounts
        constexpr auto xlate_x = 5.0;
        constexpr auto xlate_y = -3.0;
        constexpr auto xlate_z = 2.0;

        raytracer::tuple const pt                 = raytracer::create_point(pt_x, pt_y, pt_z);
        raytracer::tuple const expected_xlated_pt = raytracer::create_point(xlate_x + pt_x,  /// new-x coord
                                                                            xlate_y + pt_y,  /// new-y coord
                                                                            xlate_z + pt_z); /// new-z coord

        /// compute the translation
        auto const xlate_mat     = rt_matrix_xforms_t::create_3d_translation_matrix(xlate_x,  /// translate-x
                                                                                xlate_y,  /// translate-y
                                                                                xlate_z); /// translate-z
        auto const got_xlated_pt = xlate_mat * pt;

        CHECK(expected_xlated_pt == got_xlated_pt);
}

/// inverse translate a point test
TEST_CASE("inverse translate a point test")
{
        /// point coordinates
        constexpr double pt_x = -3.0;
        constexpr double pt_y = 4.0;
        constexpr double pt_z = 5.0;

        /// translation amounts
        constexpr auto xlate_x = 5.0;
        constexpr auto xlate_y = -3.0;
        constexpr auto xlate_z = 2.0;

        raytracer::tuple const pt                = raytracer::create_point(pt_x, pt_y, pt_z);
        raytracer::tuple const exp_inv_xlated_pt = raytracer::create_point(-xlate_x + pt_x,  /// new-x coord
                                                                           -xlate_y + pt_y,  /// new-y coord
                                                                           -xlate_z + pt_z); /// new-z coord

        /// compute the translation
        auto const xlate_mat = rt_matrix_xforms_t::create_3d_translation_matrix(xlate_x,  /// translate-x
                                                                                xlate_y,  /// translate-y
                                                                                xlate_z); /// translate-z
        auto const got_inv_xlated_pt = inverse(xlate_mat) * pt;

        CHECK(exp_inv_xlated_pt == got_inv_xlated_pt);
}

/// effect of translate on a vector test
TEST_CASE("translate a vector test")
{
        /// point coordinates
        constexpr double pt_x = -3.0;
        constexpr double pt_y = 4.0;
        constexpr double pt_z = 5.0;

        /// translation amounts
        constexpr auto xlate_x = 5.0;
        constexpr auto xlate_y = -3.0;
        constexpr auto xlate_z = 2.0;

        raytracer::tuple const vec            = raytracer::create_vector(pt_x, pt_y, pt_z);
        raytracer::tuple const exp_xlated_vec = vec;

        /// compute the translation
        auto const xlate_mat      = rt_matrix_xforms_t::create_3d_translation_matrix(xlate_x,  /// translate-x
                                                                                xlate_y,  /// translate-y
                                                                                xlate_z); /// translate-z
        auto const got_xlated_vec = xlate_mat * vec;

        CHECK(exp_xlated_vec == got_xlated_vec);
}

/// scaling matrix creation test
TEST_CASE("scaling matrix creation test")
{
        constexpr auto scale_x = 5.0;
        constexpr auto scale_y = -3.0;
        constexpr auto scale_z = 2.0;

        // clang-format off
        rt_matrix_t const expected_scale_mat = {
                                         { scale_x , 0.0     , 0.0     , 0.0 },
                                         { 0.0     , scale_y , 0.0     , 0.0 },
                                         { 0.0     , 0.0     , scale_z , 0.0 },
                                         { 0.0     , 0.0     , 0.0     , 1.0 }
        };
        // clang-format on

        rt_matrix_t const got_scale_mat = rt_matrix_xforms_t::create_3d_scaling_matrix(scale_x,  /// scale-x
                                                                                       scale_y,  /// scale-y
                                                                                       scale_z); /// scale-z

        CHECK(expected_scale_mat == got_scale_mat);
}

/// scale a point test
TEST_CASE("scale a point test")
{
        /// point coordinates
        constexpr double pt_x = -4.0;
        constexpr double pt_y = 6.0;
        constexpr double pt_z = 8.0;

        /// scale amounts
        constexpr auto scale_x = 2.0;
        constexpr auto scale_y = 3.0;
        constexpr auto scale_z = 4.0;

        raytracer::tuple const pt            = raytracer::create_point(pt_x, pt_y, pt_z);
        raytracer::tuple const exp_scaled_pt = raytracer::create_point(scale_x * pt_x,  /// new-x coord
                                                                       scale_y * pt_y,  /// new-y coord
                                                                       scale_z * pt_z); /// new-z coord

        /// compute the scaling
        auto const scale_mat     = rt_matrix_xforms_t::create_3d_scaling_matrix(scale_x,  /// scale-x
                                                                            scale_y,  /// scale-y
                                                                            scale_z); /// scale-z
        auto const got_scaled_pt = scale_mat * pt;

        CHECK(exp_scaled_pt == got_scaled_pt);
}

/// inverse-scale i.e. shrink a point test
TEST_CASE("inverse-scale a point test")
{
        /// point coordinates
        constexpr double pt_x = -4.0;
        constexpr double pt_y = 6.0;
        constexpr double pt_z = 8.0;

        /// scale amounts
        constexpr auto scale_x = 2.0;
        constexpr auto scale_y = 3.0;
        constexpr auto scale_z = 4.0;

        raytracer::tuple const pt            = raytracer::create_point(pt_x, pt_y, pt_z);
        raytracer::tuple const exp_scaled_pt = raytracer::create_point(pt_x / scale_x,  /// new-x coord
                                                                       pt_y / scale_y,  /// new-y coord
                                                                       pt_z / scale_z); /// new-z coord

        /// compute the inverse scaling
        auto const scale_mat     = rt_matrix_xforms_t::create_3d_scaling_matrix(scale_x,  /// scale-x
                                                                            scale_y,  /// scale-y
                                                                            scale_z); /// scale-z
        auto const got_scaled_pt = inverse(scale_mat) * pt;

        CHECK(exp_scaled_pt == got_scaled_pt);
}

/// effect of scaling a vector
TEST_CASE("scale a vector")
{
        /// point coordinates
        constexpr double vec_x = -4.0;
        constexpr double vec_y = 6.0;
        constexpr double vec_z = 8.0;

        /// scale amounts
        constexpr auto scale_x = 2.0;
        constexpr auto scale_y = 3.0;
        constexpr auto scale_z = 4.0;

        raytracer::tuple const vec            = raytracer::create_vector(vec_x, vec_y, vec_z);
        raytracer::tuple const exp_scaled_vec = raytracer::create_vector(scale_x * vec_x,  /// new-x coord
                                                                         scale_y * vec_y,  /// new-y coord
                                                                         scale_z * vec_z); /// new-z coord

        /// compute the scaling
        auto const scale_mat      = rt_matrix_xforms_t::create_3d_scaling_matrix(scale_x,  /// scale-x
                                                                            scale_y,  /// scale-y
                                                                            scale_z); /// scale-z
        auto const got_scaled_vec = scale_mat * vec;

        CHECK(exp_scaled_vec == got_scaled_vec);
}

/// reflect a point test
TEST_CASE("reflect a point test")
{
        /// point coordinates
        constexpr double pt_x = 2.0;
        constexpr double pt_y = 3.0;
        constexpr double pt_z = 4.0;

        /// reflect about the x-axis
        constexpr auto scale_x = -1.0;
        constexpr auto scale_y = 1.0;
        constexpr auto scale_z = 1.0;

        raytracer::tuple const pt               = raytracer::create_point(pt_x, pt_y, pt_z);
        raytracer::tuple const exp_reflected_pt = raytracer::create_point(scale_x * pt_x,  /// new-x coord
                                                                          scale_y * pt_y,  /// new-y coord
                                                                          scale_z * pt_z); /// new-z coord

        /// compute the reflection
        auto const scale_mat        = rt_matrix_xforms_t::create_3d_scaling_matrix(scale_x,  /// scale-x
                                                                            scale_y,  /// scale-y
                                                                            scale_z); /// scale-z
        auto const got_reflected_pt = scale_mat * pt;

        CHECK(exp_reflected_pt == got_reflected_pt);
}

/// test rotation about x-axis
TEST_CASE("rotation about x-axis test")
{
        /// point coordinates
        constexpr double pt_x = 0.0;
        constexpr double pt_y = 1.0;
        constexpr double pt_z = 0.0;

        auto const pt = raytracer::create_point(pt_x, pt_y, pt_z);

        /// rotation matrices
        auto const cw_half_quarter_rot = rt_matrix_xforms_t::create_rotx_matrix(raytracer::PI / 4);
        auto const cw_full_quarter_rot = rt_matrix_xforms_t::create_rotx_matrix(raytracer::PI / 2);

        constexpr double sqrt_2_div_2         = std::sqrt(2) / 2.0;
        auto const exp_cw_half_quarter_rot_pt = raytracer::create_point(0.0, sqrt_2_div_2, sqrt_2_div_2);
        auto const exp_cw_full_quarter_rot_pt = raytracer::create_point(0.0, 0.0, 1.0);

        /// compute the rotations on the point
        auto const got_cw_half_quarter_rot_pt = cw_half_quarter_rot * pt;
        auto const got_cw_full_quarter_rot_pt = cw_full_quarter_rot * pt;

        /// validate
        CHECK(exp_cw_half_quarter_rot_pt == got_cw_half_quarter_rot_pt);
        CHECK(exp_cw_full_quarter_rot_pt == got_cw_full_quarter_rot_pt);
}

/// test rotation about y-axis
TEST_CASE("rotation about y-axis test")
{
        /// point coordinates
        constexpr double pt_x = 0.0;
        constexpr double pt_y = 0.0;
        constexpr double pt_z = 1.0;

        auto const pt = raytracer::create_point(pt_x, pt_y, pt_z);

        /// rotation matrices
        auto const cw_half_quarter_rot = rt_matrix_xforms_t::create_roty_matrix(raytracer::PI / 4);
        auto const cw_full_quarter_rot = rt_matrix_xforms_t::create_roty_matrix(raytracer::PI / 2);

        constexpr double sqrt_2_div_2         = std::sqrt(2) / 2.0;
        auto const exp_cw_half_quarter_rot_pt = raytracer::create_point(sqrt_2_div_2, 0.0, sqrt_2_div_2);
        auto const exp_cw_full_quarter_rot_pt = raytracer::create_point(1.0, 0.0, 0.0);

        /// compute the rotations on the point
        auto const got_cw_half_quarter_rot_pt = cw_half_quarter_rot * pt;
        auto const got_cw_full_quarter_rot_pt = cw_full_quarter_rot * pt;

        /// validate
        CHECK(exp_cw_half_quarter_rot_pt == got_cw_half_quarter_rot_pt);
        CHECK(exp_cw_full_quarter_rot_pt == got_cw_full_quarter_rot_pt);
}

/// test rotation about z-axis
TEST_CASE("rotation about z-axis test")
{
        /// point coordinates
        constexpr double pt_x = 0.0;
        constexpr double pt_y = 1.0;
        constexpr double pt_z = 0.0;

        auto const pt = raytracer::create_point(pt_x, pt_y, pt_z);

        /// rotation matrices
        auto const cw_half_quarter_rot = rt_matrix_xforms_t::create_rotz_matrix(raytracer::PI / 4);
        auto const cw_full_quarter_rot = rt_matrix_xforms_t::create_rotz_matrix(raytracer::PI / 2);

        constexpr double sqrt_2_div_2         = std::sqrt(2) / 2.0;
        auto const exp_cw_half_quarter_rot_pt = raytracer::create_point(-sqrt_2_div_2, sqrt_2_div_2, 0.0);
        auto const exp_cw_full_quarter_rot_pt = raytracer::create_point(-1.0, 0.0, 0.0);

        /// compute the rotations on the point
        auto const got_cw_half_quarter_rot_pt = cw_half_quarter_rot * pt;
        auto const got_cw_full_quarter_rot_pt = cw_full_quarter_rot * pt;

        /// validate
        CHECK(exp_cw_half_quarter_rot_pt == got_cw_half_quarter_rot_pt);
        CHECK(exp_cw_full_quarter_rot_pt == got_cw_full_quarter_rot_pt);
}

/// test reflection about the x/y/z axes
TEST_CASE("reflection about x/y/z axis test")
{
        /// point coordinates
        constexpr double pt_x = 2.0;
        constexpr double pt_y = 3.0;
        constexpr double pt_z = 4.0;

        struct {
                raytracer::fsize_dense2d_matrix_t mat_;
                raytracer::tuple exp_reflected_pt_;
        } const tc_list[] = {
                /// x-axis reflection
                {
                        rt_matrix_xforms_t::create_reflect_x_matrix(),
                        raytracer::create_point(-pt_x, pt_y, pt_z),
                },

                /// y-axis reflection
                {
                        rt_matrix_xforms_t::create_reflect_y_matrix(),
                        raytracer::create_point(pt_x, -pt_y, pt_z),
                },

                /// z-axis reflection
                {
                        rt_matrix_xforms_t::create_reflect_z_matrix(),
                        raytracer::create_point(pt_x, pt_y, -pt_z),
                },
        };

        auto const pt = raytracer::create_point(pt_x, pt_y, pt_z);

        /// let the reflections begin
        for (auto const& tc : tc_list) {
                auto const got_pt = tc.mat_ * pt;
                CHECK(got_pt == tc.exp_reflected_pt_);
        }
}

/// test shearing
TEST_CASE("test shearing")
{
        struct {
                double xy_, xz_;
                double yx_, yz_;
                double zx_, zy_;
                raytracer::tuple exp_sheared_pt_;
        } const shearing_tc[] = {
                /// [0]
                {1.0, 0.0, /// xy, xz
                 0.0, 0.0, /// yx, yz
                 0.0, 0.0, /// zx, zy
                 raytracer::create_point(5.0, 3.0, 4.0)},

                /// [1]
                {0.0, 1.0, /// xy, xz
                 0.0, 0.0, /// yx, yz
                 0.0, 0.0, /// zx, zy
                 raytracer::create_point(6.0, 3.0, 4.0)},

                /// [2]
                {0.0, 0.0, /// xy, xz
                 1.0, 0.0, /// yx, yz
                 0.0, 0.0, /// zx, zy
                 raytracer::create_point(2.0, 5.0, 4.0)},

                /// [3]
                {0.0, 0.0, /// xy, xz
                 0.0, 1.0, /// yx, yz
                 0.0, 0.0, /// zx, zy
                 raytracer::create_point(2.0, 7.0, 4.0)},

                /// [4]
                {0.0, 0.0, /// xy, xz
                 0.0, 0.0, /// yx, yz
                 1.0, 0.0, /// zx, zy
                 raytracer::create_point(2.0, 3.0, 6.0)},

                /// [5]
                {0.0, 0.0, /// xy, xz
                 0.0, 0.0, /// yx, yz
                 0.0, 1.0, /// zx, zy
                 raytracer::create_point(2.0, 3.0, 7.0)},
        };

        /// create the point that we want to shear
        auto const pt = raytracer::create_point(2.0, 3.0, 4.0);

        /// let the shearing begin
        for (auto const& tc : shearing_tc) {
                // clang-format off
                auto const tc_shear_mat = rt_matrix_xforms_t::create_shearing_matrix(tc.xy_, tc.xz_,
                                                                                     tc.yx_, tc.yz_,
                                                                                     tc.zx_, tc.zy_);
                // clang-format on
                auto const got_sheared_pt = tc_shear_mat * pt;
                CHECK(got_sheared_pt == tc.exp_sheared_pt_);
        }
}

/// test chained-transforms
TEST_CASE("test chained transforms")
{
        auto const pt           = raytracer::create_point(1.0, 0.0, 1.0);
        auto const rotx_mat     = rt_matrix_xforms_t::create_rotx_matrix(raytracer::PI / 2);
        auto const scale_mat    = rt_matrix_xforms_t::create_3d_scaling_matrix(5.0, 5.0, 5.0);
        auto const xlate_mat    = rt_matrix_xforms_t::create_3d_translation_matrix(10.0, 5.0, 7.0);
        auto const exp_final_pt = raytracer::create_point(15.0, 0.0, 7.0);

        /// first we apply these transformations in one-step-at-a-time mode
        {
                /// step-01: rotation
                auto const exp_pt_02 = raytracer::create_point(1.0, -1.0, 0.0);
                auto const got_pt_02 = rotx_mat * pt;
                CHECK(got_pt_02 == exp_pt_02);

                /// step-02 : scaling
                auto const exp_pt_03 = raytracer::create_point(5.0, -5.0, 0.0);
                auto const got_pt_03 = scale_mat * got_pt_02;
                CHECK(got_pt_03 == exp_pt_03);

                /// step-03 : translate
                auto const got_pt_04 = xlate_mat * got_pt_03;
                CHECK(got_pt_04 == exp_final_pt);
        }

        /// second, we compute the final transformation matrix, and then apply that on the point.
        auto const final_xform_mat = xlate_mat * scale_mat * rotx_mat;
        auto const got_final_pt    = final_xform_mat * pt;
        CHECK(got_final_pt == exp_final_pt);
}

/// ----------------------------------------------------------------------------
/// test view transformations: default orientation
TEST_CASE("matrix_transformations_t::create_view_transform(...) tests")
{
        auto const look_from = raytracer::create_point(0.0, 0.0, 0.0);
        auto const look_to   = raytracer::create_point(0.0, 0.0, -1.0);
        auto const up_vec    = raytracer::create_vector(0.0, 1.0, 0.0);

        auto const got_view_xform = rt_matrix_xforms_t::create_view_transform(look_from, look_to, up_vec);
        auto const exp_view_xform = raytracer::fsize_dense2d_matrix_t::create_identity_matrix(4);

        CHECK(got_view_xform == exp_view_xform);
}

/// ----------------------------------------------------------------------------
/// test view transformations: looking in +ve z-direction
TEST_CASE("matrix_transformations_t::create_view_transform(...) tests")
{
        auto const look_from = raytracer::create_point(0.0, 0.0, 0.0);
        auto const look_to   = raytracer::create_point(0.0, 0.0, 1.0);
        auto const up_vec    = raytracer::create_vector(0.0, 1.0, 0.0);

        auto const got_view_xform = rt_matrix_xforms_t::create_view_transform(look_from, look_to, up_vec);
        auto const exp_view_xform = rt_matrix_xforms_t::create_3d_scaling_matrix(-1.0, 1.0, -1.0);

        CHECK(got_view_xform == exp_view_xform);
}

/// ----------------------------------------------------------------------------
/// test view transformations: moving the world
TEST_CASE("matrix_transformations_t::create_view_transform(...) tests")
{
        auto const look_from = raytracer::create_point(0.0, 0.0, 8.0);
        auto const look_to   = raytracer::create_point(0.0, 0.0, 0.0);
        auto const up_vec    = raytracer::create_vector(0.0, 1.0, 0.0);

        auto const got_view_xform = rt_matrix_xforms_t::create_view_transform(look_from, look_to, up_vec);
        auto const exp_view_xform = rt_matrix_xforms_t::create_3d_translation_matrix(0.0, 0.0, -8.0);

        CHECK(got_view_xform == exp_view_xform);
}

/// ----------------------------------------------------------------------------
/// test view transformations: arbitrary view
TEST_CASE("matrix_transformations_t::create_view_transform(...) tests")
{
        auto const look_from = raytracer::create_point(1.0, 3.0, 2.0);
        auto const look_to   = raytracer::create_point(4.0, -2.0, 8.0);
        auto const up_vec    = raytracer::create_vector(1.0, 1.0, 0.0);

        auto const got_view_xform = rt_matrix_xforms_t::create_view_transform(look_from, look_to, up_vec);

        /// setup the expected matrix
        auto exp_view_xform = raytracer::fsize_dense2d_matrix_t::create_identity_matrix(4);

        /// row-0
        exp_view_xform(0, 0) = -0.50709;
        exp_view_xform(0, 1) = 0.50709;
        exp_view_xform(0, 2) = 0.67612;
        exp_view_xform(0, 3) = -2.36643;

        /// row-1
        exp_view_xform(1, 0) = 0.76772;
        exp_view_xform(1, 1) = 0.60609;
        exp_view_xform(1, 2) = 0.12122;
        exp_view_xform(1, 3) = -2.82843;

        /// row-2
        exp_view_xform(2, 0) = -0.35857;
        exp_view_xform(2, 1) = 0.59761;
        exp_view_xform(2, 2) = -0.71714;
        exp_view_xform(2, 3) = 0.00000;

        /// row-3
        exp_view_xform(3, 0) = 0.00000;
        exp_view_xform(3, 1) = 0.00000;
        exp_view_xform(3, 2) = 0.00000;
        exp_view_xform(3, 3) = 1.00000;

        CHECK(got_view_xform == exp_view_xform);
}
