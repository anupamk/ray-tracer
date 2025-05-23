#pragma once

/// c++ includes
#include <memory>
#include <optional>
#include <string>
#include <vector>

/// our includes
#include "primitives/intersection_record.hpp"
#include "primitives/tuple.hpp"
#include "shapes/aabb.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class material;
        class ray_t;
        template <typename T>
        class the_badge;

        /**
         * a 'group' shape defines an abstract shape i.e. a shape without a
         * surface but taking form from the shapes that it contains.
         **/
        class group final : public shape_interface
        {
            private:
                /*
                 * @brief
                 *    all the child shapes belonging to this group.
                 **/
                std::vector<std::shared_ptr<shape_interface const>> child_shapes_;

                /*
                 * @brief
                 *    aabb of a group of objects would not change. there is no
                 *    reason not to cache it for quick access.
                 **/
                aabb bounding_box_ = {};

            public:
                group(bool cast_shadow = true);

                std::shared_ptr<shape_interface> get_ptr();

            public:
                /// ------------------------------------------------------------
                /// overridden methods here

                /// compute intersection of a group with a ray
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// compute normal at a give point for this shape
                tuple normal_at_local(tuple const&, intersection_record const&) const override;

                /// stringified representation of this shape
                std::string stringify() const override;

                /// compute intersetion of a ray with this shape, and return
                /// 'true' iff the ray intersects before 'distance'.
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

                /// ------------------------------------------------------------
                /// bounding box for an instance of a group of shapes
                aabb bounds_of() const override;

                /// is the group empty ? i.e contains no child shapes
                bool is_empty() const;

                void set_material(material const&) override;

                /// ------------------------------------------------------------
                /// const-ref for child_shapes
                decltype(child_shapes_) const& child_shapes_cref() const;

                /// ------------------------------------------------------------
                /// add a child shape to a group
                void add_child(std::shared_ptr<shape_interface> new_shape);

                /// ------------------------------------------------------------
                /// does this shape include the other shape ? for instances of
                /// this class, we want this redefined method to be invoked.
                ///
                /// for a group shape, we check for the 'includes' relationship
                /// for all the shapes in the group.
                ///
                /// return 'true' if it does, 'false' otherwise.
                bool includes(std::shared_ptr<shape_interface const> const& other) const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for ray-group intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };
} // namespace raytracer
