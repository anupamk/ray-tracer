#ifndef RAYTRACER_GROUP_HPP__
#define RAYTRACER_GROUP_HPP__

/// c++ includes
#include <memory>
#include <string>
#include <vector>

/// our includes
#include "intersection_record.hpp"
#include "ray.hpp"
#include "shape_interface.hpp"

namespace raytracer
{
        /**
         * a 'group' shape defines an abstract shape i.e. a shape without a
         * surface but taking form from the shapes that it contains.
         **/
        class group : public shape_interface
        {
            private:
                std::vector<std::shared_ptr<const shape_interface>> child_shapes_;

            public:
                group(bool cast_shadow = false);

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

                /// is the group empty ? i.e contains no child shapes
                bool is_empty() const;

                /// does the group include the specified shape ?
                bool includes(std::shared_ptr<const shape_interface> const& the_shape) const;

                void set_material(material const&) override;

                /// ------------------------------------------------------------
                /// const-ref for child_shapes
                decltype(child_shapes_) const& child_shapes_cref() const;

                /// ------------------------------------------------------------
                /// add a child shape to a group
                void add_child(std::shared_ptr<shape_interface> new_shape);

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for ray-group intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };
} // namespace raytracer

#endif /// RAYTRACER_GROUP_HPP__