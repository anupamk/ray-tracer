/*
 * This file declares primitives required for constructive-solid-geometry aka
 * CSG. This allows combining various shapes with set (union, intersection,
 * difference) operations.
 **/

#pragma once

#include <memory>

#include "primitives/intersection_record.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        /*
         * @brief
         *    an operation that can be performed on a csg_shape that yields a
         *    new shape.
         **/
        class csg_operation
        {
            public:
                virtual ~csg_operation() = default;

                /*
                 * @brief
                 *    stringified representation of the operation being
                 *    performed here. mostly used for logging purposes.
                 **/
                virtual std::string stringify() const = 0;

                /*
                 * @brief
                 *    for a given operation, when a ray hits two shapes, what
                 *    all intersections are allowed ?
                 *
                 * @left_shape_hit
                 *    when 'true' left shape was hit, when 'false' right shape
                 *    was hit.
                 *
                 * @hit_inside_left_shape
                 *    when 'true', hit occurs inside left shape.
                 *
                 * @hit_inside_right_shape
                 *    when 'true', hit occurs inside right shape.
                 **/
                virtual bool intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                                  bool hit_inside_right_shape) const = 0;
        };

        /*
         * @brief
         *    implement union of two shapes
         **/
        class csg_union final : public csg_operation
        {
            public:
                csg_union()  = default;
                ~csg_union() = default;

                std::string stringify() const override;

                bool intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                          bool hit_inside_right_shape) const override;
        };

        /*
         * @brief
         *    implement intersection of two shapes
         **/
        class csg_intersection final : public csg_operation
        {
            public:
                csg_intersection()  = default;
                ~csg_intersection() = default;

                std::string stringify() const override;

                bool intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                          bool hit_inside_right_shape) const override;
        };

        /*
         * @brief
         *    implement difference of two shapes
         **/
        class csg_difference final : public csg_operation
        {
            public:
                csg_difference()  = default;
                ~csg_difference() = default;

                std::string stringify() const override;

                bool intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                          bool hit_inside_right_shape) const override;
        };

        /*
         * @brief
         *    a shape created by combining two primitive shapes by a
         *    'csg_operation'.
         **/
        class csg_shape final : public shape_interface
        {
            private:
                std::shared_ptr<shape_interface> l_shape;
                std::shared_ptr<csg_operation> csg_op;
                std::shared_ptr<shape_interface> r_shape;

            public:
                static std::shared_ptr<csg_shape>
                create_csg(std::shared_ptr<shape_interface> left,  /// left-shape
                           std::shared_ptr<csg_operation> csg_op,  /// operation
                           std::shared_ptr<shape_interface> right, /// right-shape
                           bool cast_shadow = true);               /// cast-shadow ?

            private:
                csg_shape(std::shared_ptr<shape_interface> left, std::shared_ptr<csg_operation> csg_op,
                          std::shared_ptr<shape_interface> right, bool cast_shadow);

            public:
                /*
                 * @brief
                 *    left and right shapes associated with this instance of
                 *    the csg
                 **/
                std::shared_ptr<shape_interface> left() const;
                std::shared_ptr<shape_interface> right() const;

                /*
                 * @brief
                 *    actual csg operation
                 **/
                std::shared_ptr<csg_operation> operation() const;

            public:
                /*
                 * all overrides from the base class
                 **/

                /// ------------------------------------------------------------
                /// compute intersection of a ray
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// ------------------------------------------------------------
                /// normal vector at a given point on the 'csg-shape' (in local
                /// coordinates).
                ///
                /// however, keep in mind that the intersection records always
                /// point to the primitive object that was hit and not the
                /// parent csg-shape. this means that the primitive shape
                /// performs the normal computation.
                tuple normal_at_local(tuple const&, intersection_record const&) const override;

                /// ------------------------------------------------------------
                /// stringified reperesentation
                std::string stringify() const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the csg_shape, and return
                /// 'true' iff 'R' intersects before 'distance'.
                ///
                /// return 'false' otherwise
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

                /// ------------------------------------------------------------
                /// does this shape include the other shape ? for instances of
                /// this class, we want this redefined method to be invoked.
                ///
                /// for a csg shape, we check for the 'includes' relationship
                /// in both the 'left' and 'right' shape in the csg.
                ///
                /// return 'true' if it does, 'false' otherwise.
                bool includes(std::shared_ptr<shape_interface const> const& other) const override;

                /// ------------------------------------------------------------
                /// given a list of intersections, produce a subset of only
                /// those intersections that conform to the 'csg_op'
                std::optional<intersection_records>
                filter_intersections(intersection_records const& xs_list) const;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-csg-shape intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };
} // namespace raytracer
