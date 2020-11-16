/*
 * implement the raytracer matrix
**/
/// c++ includes
#include <algorithm>
#include <cstddef>
#include <ios>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

/// our includes
#include "matrix.hpp"
#include "tuple.hpp"
#include "utils.hpp"

namespace raytracer
{
	///
	/// create a (possibly default) initialized matrix
	///
	fsize_dense2d_matrix_t::fsize_dense2d_matrix_t(size_t rows, size_t cols, double init_val)
	    : rows_{rows}
	    , cols_{cols}
	    , data_(rows_ * cols_, init_val)
	{
	}

	///
	/// create matrix via an initializer-list
	///
	fsize_dense2d_matrix_t::fsize_dense2d_matrix_t(dense_2d_init_list_t const& row_list)
	    : rows_{row_list.size()}
	    , cols_{row_list.begin()->size()}
	    , data_(rows_ * cols_)
	{
		auto col_index = 0;
		for (const auto ith_row : row_list) {
			std::copy(ith_row.begin(), ith_row.end(), &data_[col_index]);
			col_index += cols_;
		}
	}

	///
	/// this function is called to create an identity matrix of a specific size.
	///
	fsize_dense2d_matrix_t fsize_dense2d_matrix_t::create_identity_matrix(size_t sz)
	{
		fsize_dense2d_matrix_t ident_mat(sz, sz);
		for (size_t i = 0; i < sz; i++) {
			ident_mat(i, i) = 1.0;
		}

		return ident_mat;
	}

	///
	/// this function is called to return a specific row
	///
	std::vector<double> fsize_dense2d_matrix_t::get_row(size_t row_num) const
	{
		const auto data_start = this->data_.begin();
		const auto row_start  = data_start + row_num * this->num_cols();
		const auto row_end    = row_start + this->num_cols();

		const std::vector<double> ret(row_start, row_end);
		return ret;
	}

	///
	/// bounds-checked get_row
	///
	std::vector<double> fsize_dense2d_matrix_t::checked_get_row(size_t row_num) const
	{
		this->check_row_bounds_(row_num);
		return get_row(row_num);
	}

	///
	/// this function is called to return a specific column
	///
	std::vector<double> fsize_dense2d_matrix_t::get_column(size_t col_num) const
	{
		std::vector<double> ret(this->num_rows());

		for (size_t i = 0; i < this->num_rows(); i++) {
			ret[i] = this->data_[this->get_elem_index_(i, col_num)];
		}

		return ret;
	}

	/// runtime-bounds-checked get_column
	std::vector<double> fsize_dense2d_matrix_t::checked_get_column(size_t col_num) const
	{
		this->check_column_bounds_(col_num);
		return get_column(col_num);
	}

	///
	/// stringified representation of a matrix
	///
	std::string fsize_dense2d_matrix_t::stringify() const
	{
		std::stringstream ss("");

		for (size_t i = 0; i < this->num_rows(); i++) {
			for (size_t j = 0; j < this->num_cols(); j++) {
				// clang-format off
                                ss << std::fixed << std::left
                                   << this->operator()(i, j) << "\t";
				// clang-format on
			}

			if (likely(i == (this->num_rows() - 1))) {
				continue;
			}

			ss << std::endl;
		}

		return ss.str();
	}

	///
	/// fotran style element access
	///
	double fsize_dense2d_matrix_t::operator()(size_t i, size_t j) const
	{
		return this->data_[this->get_elem_index_(i, j)];
	}

	///
	/// fortran style element assignment
	///
	double& fsize_dense2d_matrix_t::operator()(size_t i, size_t j)
	{
		return this->data_[this->get_elem_index_(i, j)];
	}

	///
	/// bounds-checked access
	///
	double fsize_dense2d_matrix_t::checked_get_elem(size_t i, size_t j) const
	{
		this->check_row_bounds_(i);
		this->check_column_bounds_(j);

		return (*this)(i, j);
	}

	///
	/// bounds-checked assignment
	///
	void fsize_dense2d_matrix_t::checked_set_elem(size_t i, size_t j, double val)
	{
		this->check_row_bounds_(i);
		this->check_column_bounds_(j);

		(*this)(i, j) = val;
		return;
	}

	///
	/// this function is called to compute the transpose of the matrix.
	///
	fsize_dense2d_matrix_t fsize_dense2d_matrix_t::transpose() const
	{
		fsize_dense2d_matrix_t ret(this->num_cols(), this->num_rows());

		for (size_t i = 0; i < ret.num_rows(); i++) {
			for (size_t j = 0; j < ret.num_cols(); j++) {
				ret(i, j) = data_[this->get_elem_index_(j, i)];
			}
		}

		return ret;
	}

	///
	/// this function is called to compute the product of this matrix by
	/// another.
	///
	fsize_dense2d_matrix_t& fsize_dense2d_matrix_t::operator*=(fsize_dense2d_matrix_t const& rhs)
	{
		fsize_dense2d_matrix_t lhs(this->num_rows(), rhs.num_cols());

		if (likely((lhs.num_cols() == 4) && (lhs.num_rows() == 4))) {
			/// ----------------------------------------------------
			/// 'unrolled' 4x4 multiplication
			for (size_t i = 0; i < lhs.num_rows(); i++) {
				for (size_t j = 0; j < lhs.num_cols(); j++) {
					lhs(i, j) = ((*this)(i, 0) * rhs(0, j) + /// 1
					             (*this)(i, 1) * rhs(1, j) + /// 2
					             (*this)(i, 2) * rhs(2, j) + /// 3
					             (*this)(i, 3) * rhs(3, j)); /// 4
				}
			}
		} else {
			/// ----------------------------------------------------
			/// canonical 'generic' multiplication
			for (size_t i = 0; i < lhs.num_rows(); i++) {
				for (size_t j = 0; j < lhs.num_cols(); j++) {
					for (size_t k = 0; k < this->num_cols(); k++) {
						lhs(i, j) += (*this)(i, k) * rhs(k, j);
					}
				}
			}
		}

		*this = lhs;
		return *this;
	}

	///
	/// 'reasonably' formatted output for the matrix
	///
	std::ostream& operator<<(std::ostream& os, fsize_dense2d_matrix_t const& M)
	{
		return os << M.stringify();
	}

	///
	/// return 'true' if M == N, 'false' otherwise. this is an
	/// 'epsilon_equal(...)' comparison for each M(i,j) and N(i,j)
	///
	bool operator==(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N)
	{
		// clang-format off
                if (unlikely(M.num_rows() != N.num_rows())) { return false; }
                if (unlikely(M.num_cols() != N.num_cols())) { return false; }
		// clang-format on

		/// element-by-element epsilon-comparison
		for (size_t i = 0; i < M.num_rows(); i++) {
			for (size_t j = 0; j < M.num_cols(); j++) {
				const auto m_ij = M(i, j);
				const auto n_ij = N(i, j);

				if (unlikely(!epsilon_equal(m_ij, n_ij))) {
					return false;
				}
			}
		}

		return true;
	}

	///
	/// return 'true' if M != N, 'false' otherwise. this is an
	/// 'epsilon_equal(...)' comparison for each M(i,j) and N(i,j)
	///
	bool operator!=(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N)
	{
		return !(M == N);
	}

	///
	/// return the product of two matrices M and N such that
	///   C(m, p) = M(m, n) * N(n, p)
	///
	fsize_dense2d_matrix_t operator*(fsize_dense2d_matrix_t const& M, fsize_dense2d_matrix_t const& N)
	{
		fsize_dense2d_matrix_t ret(M);
		return ret *= N;
	}

	///
	/// this function is called to multiply a matrix by a tuple.
	///
	tuple operator*(fsize_dense2d_matrix_t const& M, tuple const& N)
	{
		/// ------------------------------------------------------------
		/// an 'unrolled' specialization can be used i.e. no
		/// need to go through elaborate/generic matrix-tuple
		/// multiplication
		float n_w = N.is_point() ? 1.0 : 0.0;

		float tup_ret_x = (M(0, 0) * N.x() + /// [0,0].0
		                   M(0, 1) * N.y() + /// [0,0].1
		                   M(0, 2) * N.z() + /// [0,0].2
		                   M(0, 3) * n_w);   /// [0,0].3

		float tup_ret_y = (M(1, 0) * N.x() + /// [1,0].0
		                   M(1, 1) * N.y() + /// [1,0].1
		                   M(1, 2) * N.z() + /// [1,0].2
		                   M(1, 3) * n_w);   /// [1,0].3

		float tup_ret_z = (M(2, 0) * N.x() + /// [2,0].0
		                   M(2, 1) * N.y() + /// [2,0].1
		                   M(2, 2) * N.z() + /// [2,0].2
		                   M(2, 3) * n_w);   /// [2,0].3

		return tuple{tup_ret_x, /// x
		             tup_ret_y, /// y
		             tup_ret_z, /// z
		             /// -----------------------------------------------
		             /// ∵ the 'nature' of the tuple doesn't
		             /// change after the product i.e. a point
		             /// remains a point and vector remains a
		             /// vector, copying this is good enough
		             N.tuple_type()};
	}

	///
	/// return the sub-matrix of a given matrix i.e. from 'M' remove (row,
	/// column) and thereby reduce it's dimension by 1.
	///
	fsize_dense2d_matrix_t submatrix(fsize_dense2d_matrix_t const& M, size_t rm_row, size_t rm_col)
	{
		fsize_dense2d_matrix_t ret(M.num_rows() - 1, M.num_cols() - 1);

		for (size_t src_i = 0, dst_i = 0; src_i < M.num_rows(); src_i++) {
			if (unlikely(src_i == rm_row)) {
				continue;
			}

			for (size_t src_j = 0, dst_j = 0; src_j < M.num_cols(); src_j++) {
				if (unlikely(src_j == rm_col)) {
					continue;
				}

				ret(dst_i, dst_j) = M(src_i, src_j);

				/// next column
				dst_j += 1;
			}

			/// next row
			dst_i += 1;
		}

		return ret;
	}

	///
	/// this function is called to compute the determinant of a 2x2 matrix.
	///
	static inline double compute_2x2_matrix_determinant_(fsize_dense2d_matrix_t const& M)
	{
		auto const result = (M(0, 0) * M(1, 1) - M(0, 1) * M(1, 0));
		return result;
	}

	///
	/// this function recursively computes the determinant of a nxn matrix
	///
	double determinant(fsize_dense2d_matrix_t const& M)
	{
		double result = 0.0;

		/// base case
		if (M.num_rows() == 2) {
			return compute_2x2_matrix_determinant_(M);
		}

		// clang-format off

                /// use a anonymous struct rather than any other chicanery. it
                /// is a much better alternative than other constructs.
                for (struct {
                            size_t col_  = 0;               /// start-column
                            double sign_ = 1.0;
                     } X;
                     X.col_ < M.num_cols();                 /// go over all columns
                     X.col_++, X.sign_ *= -1.0) {           /// sign alternates
                        ///
                        /// so for a matrix, M, like so:
                        ///
                        ///  col-0  col-1 col-2 col-3
                        /// | -2.0  -8.0   3.0   5.0  | row-0
                        /// | -3.0   1.0   7.0   3.0  | row-1
                        /// |  1.0   2.0  -9.0   6.0  | row-2
                        /// | -6.0   7.0   7.0  -9.0  | row-3
                        ///
                        /// det(M) =  M(0,0) * (det(submatrix(M, 0, 0)))
                        ///         - M(0,1) * (det(submatrix(M, 0, 1)))
                        ///         + M(0,2) * (det(submatrix(M, 0, 2)))
                        ///         - M(0,3) * (det(submatrix(M, 0, 3)))
                        ///
                        result += X.sign_
                                * M(0, X.col_)
                                * determinant(submatrix(M, 0, X.col_));
                }
		// clang-format on

		return result;
	}

	///
	/// this function is called to return the minor of a matrix.
	///
	double minor(fsize_dense2d_matrix_t const& M, size_t i, size_t j)
	{
		return determinant(submatrix(M, i, j));
	}

	///
	/// this function is called to compute the cofactor of a matrix.
	///
	double cofactor(fsize_dense2d_matrix_t const& M, size_t i, size_t j)
	{
		double const sign = ((i + j) % 2 == 0) ? 1.0 : -1.0;

		return sign * minor(M, i, j);
	}

	///
	/// this function is called to compute the inverse of a matrix.
	///
	fsize_dense2d_matrix_t inverse(fsize_dense2d_matrix_t const& M)
	{
		double const m_det = determinant(M);
		ASSERT(m_det != 0);

		fsize_dense2d_matrix_t m_inv(M.num_rows(), M.num_cols());

		for (size_t i = 0; i < M.num_rows(); i++) {
			for (size_t j = 0; j < M.num_cols(); j++) {
				double const cofactor_m_ij = cofactor(M, i, j);

				/// transpose
				m_inv(j, i) = cofactor_m_ij / m_det;
			}
		}

		return m_inv;
	}

} // namespace raytracer
