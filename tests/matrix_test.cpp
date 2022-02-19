/// c++ includes
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest/doctest.h"

/// our own thing
#include "constants.hpp"
#include "common/include/logging.h"
#include "matrix.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// conveniences
using rt_matrix_t = raytracer::fsize_dense2d_matrix_t;

/// matrix creation test
TEST_CASE("matrix creation and element-access")
{
        constexpr size_t num_rows      = 4;
        constexpr size_t num_cols      = 3;
        constexpr double default_value = 0.33;

        const rt_matrix_t M(num_rows, num_cols, default_value);

        /// trivial checks
        CHECK(M.num_rows() == num_rows);
        CHECK(M.num_cols() == num_cols);

        /// each element check
        for (size_t i = 0; i < num_rows; ++i) {
                for (size_t j = 0; j < num_cols; ++j) {
                        CHECK(M(i, j) == default_value);
                }
        }
}

/// identity-matrix creation test
TEST_CASE("identity-matrix creation and element-access")
{
        constexpr size_t mat_size = 4;

        const auto M = rt_matrix_t::create_identity_matrix(mat_size);

        /// trivial checks
        CHECK(M.num_rows() == mat_size);
        CHECK(M.num_cols() == mat_size);

        /// each element check
        for (size_t i = 0; i < M.num_rows(); ++i) {
                CHECK(M(i, i) == 1.0);
        }
}

/// initializer-list-inited matrix creation test
TEST_CASE("initializer-list-inited matrix creation and row+column access")
{
        constexpr size_t NUM_ROWS = 5;
        constexpr size_t NUM_COLS = 3;

        // clang-format off
        rt_matrix_t const M = {
                               { 0.1 , 0.11 , 0.12 },
                               { 0.2 , 0.21 , 0.22 },
                               { 0.3 , 0.31 , 0.32 },
                               { 0.4 , 0.41 , 0.42 },
                               { 0.5 , 0.51 , 0.52 },
        };
        // clang-format on

        CHECK(M.num_rows() == NUM_ROWS);
        CHECK(M.num_cols() == NUM_COLS);

        using vec_2d_mat_t = std::vector<std::vector<double> >;

        // clang-format off
        vec_2d_mat_t const expected_rows = {
                                            { 0.1 , 0.11 , 0.12 },
                                            { 0.2 , 0.21 , 0.22 },
                                            { 0.3 , 0.31 , 0.32 },
                                            { 0.4 , 0.41 , 0.42 },
                                            { 0.5 , 0.51 , 0.52 },
        };
        // clang-format on

        /// expected_rows 'shape' check
        CHECK(expected_rows.size() == NUM_ROWS);
        for (auto row_data : expected_rows) {
                CHECK(row_data.size() == NUM_COLS);
        }

        /// check row-data
        for (size_t i = 0; i < M.num_rows(); i++) {
                auto const matrix_ith_row   = M.get_row(i);
                auto const expected_ith_row = expected_rows[i];

                CHECK(matrix_ith_row.size() == expected_ith_row.size());
                CHECK(matrix_ith_row == expected_ith_row);
        }

        // clang-format off
        vec_2d_mat_t const expected_cols = {
                                            { 0.1  , 0.2  , 0.3  , 0.4  , 0.5  },
                                            { 0.11 , 0.21 , 0.31 , 0.41 , 0.51 },
                                            { 0.12 , 0.22 , 0.32 , 0.42 , 0.52 },
        };
        // clang-format on

        /// expected_cols 'shape' check
        CHECK(M.num_cols() == expected_cols.size());
        for (auto col_data : expected_cols) {
                CHECK(M.num_rows() == col_data.size());
        }

        /// check col-data
        for (size_t i = 0; i < M.num_cols(); i++) {
                auto const matrix_ith_col   = M.get_column(i);
                auto const expected_ith_col = expected_cols[i];

                CHECK(matrix_ith_col.size() == expected_ith_col.size());
                CHECK(matrix_ith_col == expected_ith_col);
        }
}

/// transpose-inited matrix creation test
TEST_CASE("transpose-inited matrix creation test")
{
        constexpr size_t NUM_ROWS = 5;
        constexpr size_t NUM_COLS = 3;

        // clang-format off
        rt_matrix_t const m = {
                               { 0.1 , 0.11 , 0.12 },
                               { 0.2 , 0.21 , 0.22 },
                               { 0.3 , 0.31 , 0.32 },
                               { 0.4 , 0.41 , 0.42 },
                               { 0.5 , 0.51 , 0.52 },
        };
        // clang-format on
        CHECK(m.num_rows() == NUM_ROWS);
        CHECK(m.num_cols() == NUM_COLS);

        rt_matrix_t const m_transpose = m.transpose();

        /// 'shape' checks
        CHECK(m.num_rows() == m_transpose.num_cols());
        CHECK(m.num_cols() == m_transpose.num_rows());

        /// 'data' checks
        for (size_t i = 0; i < m.num_rows(); i++) {
                auto const m_ith_row  = m.get_row(i);
                auto const mt_ith_col = m_transpose.get_column(i);

                CHECK(m_ith_row.size() == mt_ith_col.size());
                CHECK(m_ith_row == mt_ith_col);
        }
}

/// transpose-inited matrix creation test
TEST_CASE("matrix epsilon comparison test")
{
        constexpr size_t NUM_ROWS = 5;
        constexpr size_t NUM_COLS = 3;

        // clang-format off
        rt_matrix_t const x_1 = {
                               { 0.1 , 0.11 , 0.12 },
                               { 0.2 , 0.21 , 0.22 },
                               { 0.3 , 0.31 , 0.32 },
                               { 0.4 , 0.41 , 0.42 },
                               { 0.5 , 0.51 , 0.52 },
        };

        rt_matrix_t const x_2 = {
                               { 0.1 , 0.11 , 0.12 },
                               { 0.2 , 0.21 , 0.22 },
                               { 0.3 , 0.31 , 0.32 },
                               { 0.4 , 0.41 , 0.42 },
                               { 0.5 , 0.51 , 0.52 },
        };

        rt_matrix_t const x_3 = {
                               { 1 , 11 , 0.12 },
                               { 2 , 21 , 0.22 },
                               { 3 , 31 , 0.32 },
                               { 4 , 41 , 0.42 },
                               { 5 , 51 , 0.52 },
        };

        rt_matrix_t const x_4 = {
                               { 0.1000011 , 0.11000012 , 0.12000013 },
                               { 0.2       , 0.21       , 0.22       },
                               { 0.3       , 0.31       , 0.32       },
                               { 0.4       , 0.41       , 0.42       },
                               { 0.5       , 0.51       , 0.52       },
        };

        rt_matrix_t const x_5 = {
                               { 0.10002 , 0.11002 , 0.12002 },
                               { 0.2     , 0.21    , 0.22    },
                               { 0.3     , 0.31    , 0.32    },
                               { 0.4     , 0.41    , 0.42    },
                               { 0.5     , 0.51    , 0.52    },
        };

        // clang-format on

        CHECK(x_1 == x_2); /// they are equal
        CHECK(x_1 != x_3); /// they are not equal
        CHECK(x_1 == x_4); /// they are _epsilon_ equal
        CHECK(x_1 != x_5); /// they are _epsilon_ not equal

        /// assign + check
        rt_matrix_t x_6(NUM_ROWS, NUM_COLS);
        x_6 = x_4;

        CHECK(x_1 == x_4); /// they are equal
        CHECK(x_1 == x_6); /// they are _epsilon_ equal
}

/// test matrix-matrix multiplication
TEST_CASE("matrix-matrix multiplication test")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 1.0 , 2.0 , 3.0 , 4.0 },
                               { 5.0 , 6.0 , 7.0 , 8.0 },
                               { 9.0 , 8.0 , 7.0 , 6.0 },
                               { 5.0 , 4.0 , 3.0 , 2.0 },
        };

        rt_matrix_t const n = {
                               { -2.0 , 1.0 , 2.0 , 3.0  },
                               { 3.0  , 2.0 , 1.0 , -1.0 },
                               { 4.0  , 3.0 , 6.0 , 5.0  },
                               { 1.0  , 2.0 , 7.0 , 8.0  },
        };

        rt_matrix_t const expected_mxn = {
                                 { 20.0 , 22.0 , 50.0  , 48.0  },
                                 { 44.0 , 54.0 , 114.0 , 108.0 },
                                 { 40.0 , 58.0 , 110.0 , 102.0 },
                                 { 16.0 , 26.0 , 46.0  , 42.0  },
        };
        // clang-format on

        auto const got_mxn = m * n;
        CHECK(got_mxn == expected_mxn);
}

/// test matrix-tuple multiplication
TEST_CASE("matrix-tuple multiplication test")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 1.0 , 2.0 , 3.0 , 4.0 },
                               { 2.0 , 4.0 , 4.0 , 2.0 },
                               { 8.0 , 6.0 , 4.0 , 1.0 },
                               { 0.0 , 0.0 , 0.0 , 1.0 },
        };
        // clang-format on

        raytracer::tuple const t            = raytracer::create_point(1.0, 2.0, 3.0);
        raytracer::tuple const expected_mxt = raytracer::create_point(18.0, 24.0, 33.0);
        raytracer::tuple const got_mxt      = m * t;

        CHECK(got_mxt.x() == expected_mxt.x());
}

/// test matrix identity-matrix multiplication
TEST_CASE("matrix identity-matrix multiplication test")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 1.0 , 2.0 , 3.0 , 4.0 },
                               { 5.0 , 6.0 , 7.0 , 8.0 },
                               { 9.0 , 8.0 , 7.0 , 6.0 },
                               { 5.0 , 4.0 , 3.0 , 2.0 },
        };
        // clang-format on

        rt_matrix_t const I = raytracer::fsize_dense2d_matrix_t::create_identity_matrix(m.num_rows());
        rt_matrix_t const expected_mxn = m;

        auto const got_mxn = m * I;
        CHECK(got_mxn == expected_mxn);
}

/// test matrix transpose
TEST_CASE("matrix transpose test")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 0.0 , 9.0 , 3.0 , 0.0 },
                               { 9.0 , 8.0 , 0.0 , 8.0 },
                               { 1.0 , 8.0 , 5.0 , 3.0 },
                               { 0.0 , 0.0 , 5.0 , 8.0 },
        };

        rt_matrix_t const expected_m_transpose = {
                               { 0.0 , 9.0 , 1.0 , 0.0 },
                               { 9.0 , 8.0 , 8.0 , 0.0 },
                               { 3.0 , 0.0 , 5.0 , 5.0 },
                               { 0.0 , 8.0 , 3.0 , 8.0 },
        };
        // clang-format on

        rt_matrix_t const got_m_transpose = m.transpose();
        CHECK(got_m_transpose == expected_m_transpose);

        /// transpose identity matrix
        rt_matrix_t const I = raytracer::fsize_dense2d_matrix_t::create_identity_matrix(m.num_rows());
        rt_matrix_t const got_I_transpose = I.transpose();

        CHECK(got_I_transpose == I);
}

/// extract submatrix from a matrix
TEST_CASE("matrix extract submatrix test")
{
        /// a 3x3 -> 2x2 matrix
        {
                // clang-format off
                rt_matrix_t const m = {
                                       {  1.0 , 5.0 ,  0.0 },
                                       { -3.0 , 2.0 ,  7.0 },
                                       {  0.0 , 6.0 , -3.0 },
                };

                size_t const m_rm_row = 0;
                size_t const m_rm_col = 2;

                rt_matrix_t const expected_m_submatrix = {
                                                          { -3.0 , 2.0 },
                                                          {  0.0 , 6.0 },
                };
                // clang-format on

                rt_matrix_t const got_m_submatrix = submatrix(m, m_rm_row, m_rm_col);
                CHECK(got_m_submatrix == expected_m_submatrix);
        }

        /// a 4x4 -> 3x3 matrix
        {
                // clang-format off
                rt_matrix_t const m = {
                                       { -6.0 , 1.0 ,  1.0 , 6.0 },
                                       { -8.0 , 5.0 ,  8.0 , 6.0 },
                                       { -1.0 , 0.0 ,  8.0 , 2.0 },
                                       { -7.0 , 1.0 , -1.0 , 1.0 },
                };

                size_t const m_rm_row = 2;
                size_t const m_rm_col = 1;

                rt_matrix_t const expected_m_submatrix = {
                                       { -6.0 ,  1.0 , 6.0 },
                                       { -8.0 ,  8.0 , 6.0 },
                                       { -7.0 , -1.0 , 1.0 },
                };
                // clang-format on

                rt_matrix_t const got_m_submatrix = submatrix(m, m_rm_row, m_rm_col);
                CHECK(got_m_submatrix == expected_m_submatrix);
        }
}

/// compute the determinant of a matrix
TEST_CASE("determinant of a matrix")
{
        // clang-format off
        rt_matrix_t const m = {
                               {  1.0 , 2.0 ,  6.0 },
                               { -5.0 , 8.0 , -4.0 },
                               {  2.0 , 6.0 ,  4.0 },
        };
        double const expected_det_m = -196.0;
        // clang-format on

        double const got_det_m = determinant(m);
        CHECK(got_det_m == expected_det_m);

        // clang-format off
        rt_matrix_t const n = {
                               { -2.0 , -8.0  ,  3.0  ,  5.0  },
                               { -3.0 ,  1.0  ,  7.0  ,  3.0  },
                               {  1.0 ,  2.0  , -9.0  ,  6.0  },
                               { -6.0 ,  7.0  ,  7.0  , -9.0  },
        };
        double const expected_det_n = -4071.0;
        // clang-format on

        double const got_det_n = determinant(n);
        CHECK(got_det_n == expected_det_n);
}

/// compute the minor a matrix
TEST_CASE("minor of a matrix")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 3.0 ,  5.0 ,  0.0 },
                               { 2.0 , -1.0 , -7.0 },
                               { 6.0 , -1.0 ,  5.0 },
        };
        // clang-format on

        size_t const minor_row      = 1;
        size_t const minor_col      = 0;
        double const expected_minor = 25.0;
        double const got_minor      = minor(m, minor_row, minor_col);

        CHECK(got_minor == expected_minor);
}

/// compute the cofactor a matrix
TEST_CASE("cofactor of a matrix")
{
        // clang-format off
        rt_matrix_t const m = {
                               { 3.0,  5.0,  0.0 },
                               { 2.0, -1.0, -7.0 },
                               { 6.0, -1.0,  5.0 },
        };
        // clang-format on

        struct {
                size_t row_;
                size_t col_;
                double expected_minor_;
                double expected_cofactor_;
        } const cofactor_tc[] = {
                {0, 0, -12.0, -12.0},
                {1, 0, 25.0, -25.0},
        };

        for (auto const tc : cofactor_tc) {
                auto got_minor    = minor(m, tc.row_, tc.col_);
                auto got_cofactor = cofactor(m, tc.row_, tc.col_);

                CHECK(got_minor == tc.expected_minor_);
                CHECK(got_cofactor == tc.expected_cofactor_);
        }
}

/// compute the inverse a matrix
TEST_CASE("inverse of a matrix")
{
        // clang-format off
        struct {
                rt_matrix_t const m_;
                rt_matrix_t const expected_m_inv_;
        } const mat_inv_tc[] = {
                /// [0]
                {
                        /// [0].m_
                        {
                                { -5.0 ,  2.0 ,  6.0 , -8.0 },
                                {  1.0 , -5.0 ,  1.0 ,  8.0 },
                                {  7.0 ,  7.0 , -6.0 , -7.0 },
                                {  1.0 , -3.0 ,  7.0 ,  4.0 },
                        },

                        /// [0].expected_m_inv_
                        {
                                {  0.21805 ,  0.45113 ,  0.24060 , -0.04511 },
                                { -0.80827 , -1.45677 , -0.44361 ,  0.52068 },
                                { -0.07895 , -0.22368 , -0.05263 ,  0.19737 },
                                { -0.52256 , -0.81391 , -0.30075 ,  0.30639 },
                        },
                },

                /// [1]
                {
                        /// [1].m_
                        {
                                {  8.0 , -5.0 ,  9.0 ,  2.0 },
                                {  7.0 ,  5.0 ,  6.0 ,  1.0 },
                                { -6.0 ,  0.0 ,  9.0 ,  6.0 },
                                { -3.0 ,  0.0 , -9.0 , -4.0 },
                        },

                        /// [1].expected_m_inv_
                        {
                                { -0.15385 , -0.15385 , -0.28205 , -0.53846 },
                                { -0.07692 ,  0.12308 ,  0.02564 ,  0.03077 },
                                {  0.35897 ,  0.35897 ,  0.43590 ,  0.92308 },
                                { -0.69231 , -0.69231 , -0.76923 , -1.92308 },
                        },
                },

                /// [2]
                {
                        /// [2].m_
                        {
                                {  9.0 ,  3.0 ,  0.0 ,  9.0 },
                                { -5.0 , -2.0 , -6.0 , -3.0 },
                                { -4.0 ,  9.0 ,  6.0 ,  4.0 },
                                { -7.0 ,  6.0 ,  6.0 ,  2.0 },
                        },

                        /// expected_m_inv_
                        {
                                { -0.04074 , -0.07778 ,  0.14444 , -0.22222 },
                                { -0.07778 ,  0.03333 ,  0.36667 , -0.33333 },
                                { -0.02901 , -0.14630 , -0.10926 ,  0.12963 },
                                {  0.17778 ,  0.06667 , -0.26667 ,  0.33333 },
                        },
                },
        };
        // clang-format on

        for (auto tc : mat_inv_tc) {
                auto const got_inv_mat = inverse(tc.m_);
                CHECK(got_inv_mat == tc.expected_m_inv_);
        }
}

/// matrix inversion and its product
TEST_CASE("matrix inversion and its product")
{
        // clang-format off
        rt_matrix_t const mat_a = {
                                   {  3.0 , -9.0 ,  7.0 ,  3.0 },
                                   {  3.0 , -8.0 ,  2.0 , -9.0 },
                                   { -4.0 ,  4.0 ,  4.0 ,  1.0 },
                                   { -6.0 ,  5.0 , -1.0 ,  1.0 },
        };

        rt_matrix_t const mat_b = {
                                   { 8.0 ,  2.0 , 2.0 , 2.0 },
                                   { 3.0 , -1.0 , 7.0 , 0.0 },
                                   { 7.0 ,  0.0 , 5.0 , 4.0 },
                                   { 6.0 , -2.0 , 0.0 , 5.0 },
        };
        // clang-format on

        ///
        /// prove the following: if you multiply a matrix A by another matrix B
        /// producing matrix C, you can multupliy C by inverse of B to get A
        /// again.
        ///
        auto const mat_c       = mat_a * mat_b;
        auto const mat_c_prime = mat_c * inverse(mat_b);
        CHECK(mat_c_prime == mat_a);

        ///
        /// since matrix products are not commutative, do this again the other
        /// way around
        ///
        auto const mat_d       = mat_b * mat_a;
        auto const mat_d_prime = mat_d * inverse(mat_a);
        CHECK(mat_d_prime == mat_b);
}

/// invert identity matrix
TEST_CASE("invert identity matrix")
{
        rt_matrix_t const mat_4_ident     = rt_matrix_t::create_identity_matrix(4);
        rt_matrix_t const mat_4_ident_inv = inverse(mat_4_ident);

        CHECK(mat_4_ident == mat_4_ident_inv);
}

///
/// difference between:
///    matrix -> transpose -> invert and
///    matrix -> inverse -> transpose
///
TEST_CASE("diff matrix-transpose-inverse and matrix-inverse-transpose")
{
        // clang-format off
        rt_matrix_t const mat_a = {
                                   {  3.0 , -9.0 ,  7.0 ,  3.0 },
                                   {  3.0 , -8.0 ,  2.0 , -9.0 },
                                   { -4.0 ,  4.0 ,  4.0 ,  1.0 },
                                   { -6.0 ,  5.0 , -1.0 ,  1.0 },
        };
        // clang-format on

        auto const mat_a_transpose_inverse = raytracer::inverse(mat_a.transpose());
        auto const mat_a_inverse_transpose = raytracer::inverse(mat_a).transpose();

        CHECK(mat_a_transpose_inverse == mat_a_inverse_transpose);
}

///
/// identity matrix multiplication by a tuple
///
TEST_CASE("identity matrix multiplication by a tuple")
{
        rt_matrix_t ident_4x4     = rt_matrix_t::create_identity_matrix(4);
        raytracer::tuple const pt = raytracer::create_vector(3.14, 1.59, 2.65);

        /// when multiplied by an identity-matrix, a tuple is left unchanged.
        {
                raytracer::tuple const expected_pt = pt;
                raytracer::tuple const got_pt      = ident_4x4 * pt;

                CHECK(expected_pt == got_pt);
        }

        ///
        /// however, changing a specific element in the identity matrix, will
        /// multiply the corresponding element in the tuple by the same amount.
        ///
        /// 'corresponding element' here implies:
        ///      ident_4x4(0, 0) ==> tuple.x
        ///      ident_4x4(1, 1) ==> tuple.y
        ///      ident_4x4(2, 2) ==> tuple.z
        ///
        {
                ident_4x4(1, 1) = 0.0;

                raytracer::tuple const expected_pt = raytracer::create_vector(3.14, 0.0, 2.65);
                raytracer::tuple const got_pt      = ident_4x4 * pt;

                CHECK(expected_pt == got_pt);
        }
}
