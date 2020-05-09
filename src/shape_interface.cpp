/// c++ includes
#include <algorithm>

/// our includes
#include "shape_interface.hpp"
#include "tuple.hpp"

namespace raytracer
{
        shape_interface::shape_interface()
            : xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , material_(material())
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

        ///
        /// this function is called to return the normal at a point on
        /// the shape. 'point' is in world-space coordinates
        ///
        tuple shape_interface::normal_at_world(tuple const& world_pt) const
        {
                /// first convert the world-point to object space, and determine
                /// the normal there
                auto const obj_pt     = this->inv_transform() * world_pt;
                auto const obj_normal = normal_at_local(obj_pt);

                /// now go back to world space and return a normalized vector.
                auto const tmp		= this->inv_transform().transpose() * obj_normal;
                auto const world_normal = normalize(create_vector(tmp.x(), tmp.y(), tmp.z()));

                return world_normal;
        }

        ///
        /// this function is called to get the current material associated with
        /// the shape
        ///
        material shape_interface::get_material() const
        {
                return this->material_;
        }

        ///
        /// this function is called to set the current material associated with
        /// the shape
        ///
        void shape_interface::set_material(material const& M)
        {
                this->material_ = M;
        }

} // namespace raytracer
