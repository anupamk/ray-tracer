/// our includes
#include "shape_interface.hpp"

namespace raytracer
{
        shape_interface::shape_interface()
            : xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
        {
        }

        fsize_dense2d_matrix_t shape_interface::transform() const
        {
                return this->xform_;
        }

        fsize_dense2d_matrix_t shape_interface::inv_transform() const
        {
                return this->inv_xform_;
        }

        void shape_interface::transform(fsize_dense2d_matrix_t const& M)
        {
                this->xform_	 = M;
                this->inv_xform_ = inverse(M);

                return;
        }

} // namespace raytracer
