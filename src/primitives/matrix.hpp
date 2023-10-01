#pragma once

/// c++ includes
#include <cstddef>
#include <initializer_list>
#include <ostream>
#include <string>
#include <vector>

/// our includes
#include "common/include/assert_utils.h"
#include "primitives/tuple.hpp"

namespace raytracer
{
        /*
         * this is a trivial row-major implementation of a fixed-size dense-2d
         * matrix, using std::vector<double> as the underlying representation of
         * it's contents.
         **/
        class fsize_dense2d_matrix_t final
        {
                using dense_2d_init_list_t = std::initializer_list<std::initializer_list<double> >;

            private:
                size_t rows_;
                size_t cols_;
                std::vector<double> data_;

            public:
                fsize_dense2d_matrix_t(size_t rows, size_t cols, double init_val = double{});
                fsize_dense2d_matrix_t(dense_2d_init_list_t const& row_list);
                static fsize_dense2d_matrix_t create_identity_matrix(size_t size);

            public:
                // clang-format off
                constexpr size_t num_rows() const { return rows_;  }
                constexpr size_t num_cols() const { return cols_;  }
                // clang-format on

            public:
                /// ------------------------------------------------------------
                /// return a specific row and column
                std::vector<double> get_row(size_t row_num) const;
                std::vector<double> checked_get_row(size_t row_num) const;

                /// ------------------------------------------------------------
                /// return a specific column
                std::vector<double> get_column(size_t col_num) const;
                std::vector<double> checked_get_column(size_t col_num) const;

                /// ------------------------------------------------------------
                /// stringified representation of a matrix
                std::string stringify() const;

                /// ------------------------------------------------------------
                /// fotran style (unchecked) access
                double operator()(size_t i, size_t j) const;

                /// ------------------------------------------------------------
                /// fortran style (unchecked) assignment
                double& operator()(size_t i, size_t j);

                /// ------------------------------------------------------------
                /// bounds-checked get element
                double checked_get_elem(size_t i, size_t j) const;

                /// ------------------------------------------------------------
                /// bounds-checked set element
                void checked_set_elem(size_t i, size_t j, double val);

                /// ------------------------------------------------------------
                /// transpose the matrix
                fsize_dense2d_matrix_t transpose() const;

                /// ------------------------------------------------------------
                /// multiply matrices together
                fsize_dense2d_matrix_t& operator*=(fsize_dense2d_matrix_t const& rhs);

            private:
                /// row-major ordering
                constexpr size_t get_elem_index_(size_t row, size_t col) const
                {
                        return (row * this->num_cols() + col);
                }

                constexpr void check_row_bounds_(size_t row_index) const
                {
                        ASSERT(row_index <= this->rows_ - 1);
                }

                constexpr void check_column_bounds_(size_t col_index) const
                {
                        ASSERT(col_index <= this->cols_ - 1);
                }
        };

        /// --------------------------------------------------------------------
        /// all non-member operators(...)
        std::ostream& operator<<(std::ostream& os, fsize_dense2d_matrix_t const& M);
        bool operator==(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N);
        bool operator!=(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N);
        fsize_dense2d_matrix_t operator*(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N);
        tuple operator*(fsize_dense2d_matrix_t const& M, tuple const& N);

        /// --------------------------------------------------------------------
        /// all non-member functions
        fsize_dense2d_matrix_t submatrix(fsize_dense2d_matrix_t const& M, size_t rm_row, size_t rm_col);
        double determinant(fsize_dense2d_matrix_t const& M);
        double minor(fsize_dense2d_matrix_t const& M, size_t i, size_t j);
        double cofactor(fsize_dense2d_matrix_t const& M, size_t i, size_t j);
        fsize_dense2d_matrix_t inverse(fsize_dense2d_matrix_t const& M);

} // namespace raytracer
