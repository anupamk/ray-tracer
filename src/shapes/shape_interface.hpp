#pragma once

/// c++ includes
#include <algorithm>
#include <memory>
#include <optional>
#include <string>

/// our includes
#include "patterns/material.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/matrix.hpp"
#include "primitives/uv_point.hpp"
#include "utils/badge.hpp"

namespace raytracer
{
        /// forward decl
        class ray_t;

        /*
         * this defines the common shape_interface which is implemented by
         * concrete shape instsances f.e. sphere etc.
         **/
        class shape_interface : public std::enable_shared_from_this<shape_interface>
        {
            private:
                /// ------------------------------------------------------------
                /// when set to 'false', the shape doesn't cast any shadow. by
                /// default all shapes cast a shadow.
                bool cast_shadow_;

                /// ------------------------------------------------------------
                /// transformation matrices associated with a shape. allows for
                /// moving shapes around, deforming them etc. etc.
                fsize_dense2d_matrix_t xform_;
                fsize_dense2d_matrix_t inv_xform_;
                fsize_dense2d_matrix_t inv_xform_transpose_;

                /// ------------------------------------------------------------
                /// the material which makes up the shape
                material material_;

                /// ------------------------------------------------------------
                /// the parent of this shape.
                std::weak_ptr<const shape_interface> parent_;

            protected:
                /// ------------------------------------------------------------
                /// don't allow deletion through a base
                virtual ~shape_interface()
                {
                }

            public:
                shape_interface(bool cast_shadow);

                /// ------------------------------------------------------------
                /// this function is called to return zero or more intersections
                /// of a shape with a ray 'R'
                virtual std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                                      ray_t const& R) const = 0;

                /// ------------------------------------------------------------
                /// this function is called to return the normal at a point on
                /// the shape in object-space coordinates
                virtual tuple normal_at_local(tuple const& world_pt, intersection_record const& xs) const = 0;

                /// ------------------------------------------------------------
                /// stringified representation of a shape
                virtual std::string stringify() const = 0;

                /// ------------------------------------------------------------
                /// return true if a ray can intersect a shape before
                /// 'distance', useful for determining shadows etc.
                ///
                /// return 'false' otherwise
                virtual bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                                     double distance) const = 0;

                /// ------------------------------------------------------------
                /// does this shape include the other shape ?
                ///
                /// return 'true' if it does, 'false' otherwise.
                virtual bool includes(std::shared_ptr<const shape_interface> const& other) const;

            public:
                /// ------------------------------------------------------------
                /// this function is called to return the current transform
                /// matrix associated with the shape
                fsize_dense2d_matrix_t transform() const;

                /// ------------------------------------------------------------
                /// this function is called to return the current inverse
                /// transform matrix associated with the shape
                fsize_dense2d_matrix_t inv_transform() const;

                /// ------------------------------------------------------------
                /// this function is called to return the transpose of the the
                /// inverse transform matrix associated with the shape
                fsize_dense2d_matrix_t inv_transform_transpose() const;

                /// ------------------------------------------------------------
                /// this function is called to associate a new transformation
                /// matrix with the shape
                virtual void transform(fsize_dense2d_matrix_t const& M);

                /// ------------------------------------------------------------
                /// normal at a an object of a group
                tuple normal_at(tuple const& world_pt,
                                intersection_record const& xs = NULL_INTERSECTION_RECORD) const;

                /// ------------------------------------------------------------
                /// this function is called to return the normal at a point on
                /// the shape in world-space coordinates
                tuple normal_at_world(tuple const&) const;

                /// ------------------------------------------------------------
                /// convert world-space coordinates into local
                tuple world_to_local(tuple const&) const;

                /// ------------------------------------------------------------
                /// adjust material properties of a shape
                material get_material() const;
                virtual void set_material(material const&);

                /// ------------------------------------------------------------
                /// adjust parent of a shape
                std::shared_ptr<const shape_interface> get_parent() const;
                void set_parent(std::shared_ptr<shape_interface>);

                /// ------------------------------------------------------------
                /// can this shape cast a shadow ?
                bool get_cast_shadow() const;
                void set_cast_shadow(bool);

                /// ------------------------------------------------------------
                /// is this shape part of a group ?
                bool is_grouped() const;
        };
} // namespace raytracer
