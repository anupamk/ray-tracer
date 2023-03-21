/// c++ includes
#include <algorithm>

/// our includes
#include "primitives/intersection_record.hpp"
#include "primitives/tuple.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        shape_interface::shape_interface(bool cast_shadow)
            : cast_shadow_(cast_shadow)
            , xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_xform_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , inv_xform_transpose_(fsize_dense2d_matrix_t::create_identity_matrix(4))
            , material_()
            , parent_({})
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

        fsize_dense2d_matrix_t shape_interface::inv_transform_transpose() const
        {
                return this->inv_xform_transpose_;
        }

        void shape_interface::transform(fsize_dense2d_matrix_t const& M)
        {
                this->xform_               = M;
                this->inv_xform_           = inverse(M);
                this->inv_xform_transpose_ = this->inv_xform_.transpose();

                return;
        }

        /// --------------------------------------------------------------------
        /// this function is called to find the normal on a child object of a
        /// group.
        tuple shape_interface::normal_at(tuple const& world_pt, intersection_record const& xs) const
        {
                auto const local_pt     = world_to_local(world_pt);
                auto const local_normal = normal_at_local(local_pt, xs);
                return normal_at_world(local_normal);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return object-space coordinates of a
        /// specific point on the shape.
        tuple shape_interface::world_to_local(tuple const& world_pt) const
        {
                auto local_pt = world_pt;

                if (auto parent_sp = this->parent_.lock()) {
                        local_pt = parent_sp->world_to_local(world_pt);
                        return inv_transform() * local_pt;
                }

                return inv_transform() * local_pt;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the normal at a point on
        /// the shape. 'point' is in world-space coordinates
        tuple shape_interface::normal_at_world(tuple const& world_pt) const
        {
                /// first convert the world-point to object space, and determine
                /// the normal there
                auto obj_space_normal = this->inv_transform_transpose() * world_pt;
                obj_space_normal.vectorify();
                obj_space_normal = normalize(obj_space_normal);

                if (auto parent_sp = this->parent_.lock()) {
                        return parent_sp->normal_at_world(obj_space_normal);
                }

                return obj_space_normal;
        }

        /// --------------------------------------------------------------------
        /// this function is called to get the current material associated with
        /// the shape
        material shape_interface::get_material() const
        {
                return this->material_;
        }

        /// --------------------------------------------------------------------
        /// this function is called to set the current material associated with
        /// the shape
        void shape_interface::set_material(material const& M)
        {
                this->material_ = M;
        }

        /// --------------------------------------------------------------------
        /// this function is called to get the parent of the current shape
        std::shared_ptr<const shape_interface> shape_interface::get_parent() const
        {
                return parent_.lock();
        }

        /// --------------------------------------------------------------------
        /// this function is called to set the parent of the current shape
        void shape_interface::set_parent(std::shared_ptr<shape_interface> new_parent)
        {
                parent_ = new_parent;
        }

        /// --------------------------------------------------------------------
        /// this function is called to get shadow_cast'ing property for the
        /// current shape
        bool shape_interface::get_cast_shadow() const
        {
                return cast_shadow_;
        }

        /// --------------------------------------------------------------------
        /// let a shape cast a shadow (true) or not (false)
        void shape_interface::set_cast_shadow(bool val)
        {
                cast_shadow_ = val;
        }

        /// --------------------------------------------------------------------
        /// this function is called to check if this shape is part of a
        /// group. returns 'true' if it is, 'false' otherwise.
        bool shape_interface::is_grouped() const
        {
                return (parent_.lock() != nullptr);
        }

        /// --------------------------------------------------------------------
        /// does this shape include the other ?
        bool shape_interface::includes(std::shared_ptr<const shape_interface> const& other) const
        {
                return this == other.get();
        }

} // namespace raytracer
