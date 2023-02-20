/// c++ includes
#include <algorithm>
#include <iostream>

/// our includes
#include "patterns/pattern_interface.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// by default both transform and inverse-transform matrices are
        /// identity.
        pattern_interface::pattern_interface()
            : xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
        {
        }

        /// --------------------------------------------------------------------
        /// adjust both transform and the inverse transform matrix.
        void pattern_interface::transform(fsize_dense2d_matrix_t const& M)
        {
                this->xform_     = M;
                this->inv_xform_ = inverse(M);

                return;
        }

        /// --------------------------------------------------------------------
        /// return the transformation matrix
        fsize_dense2d_matrix_t pattern_interface::transform() const
        {
                return this->xform_;
        }

        /// --------------------------------------------------------------------
        /// return the inverse-transformation matrix
        fsize_dense2d_matrix_t pattern_interface::inv_transform() const
        {
                return this->inv_xform_;
        }

        /// --------------------------------------------------------------------
        /// return the pattern-color at a specific point on a shape
        color pattern_interface::color_at_shape(std::shared_ptr<shape_interface const> shape,
                                                tuple const& where) const
        {
                auto const object_pt = shape->world_to_local(where);
                auto pattern_pt      = inv_transform() * object_pt;
                return color_at_point(pattern_pt);
        }
} // namespace raytracer
