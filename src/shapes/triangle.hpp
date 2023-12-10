#pragma once

/// c++ includes
#include <optional>                            // for optional
#include <string>                              // for string

/// our includes
#include "primitives/intersection_record.hpp"  // for intersection_records
#include "primitives/tuple.hpp"                // for tuple
#include "shapes/shape_interface.hpp"          // for shape_interface

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class ray_t;
        template <typename T> class the_badge;
        
        /*
         * this defines a triangle identified by 3 points (p1, p2 and p3)
         **/
        class triangle final : public shape_interface
        {
            private:
                /// ------------------------------------------------------------
                /// points making a triangle
                tuple const pt_1_;
                tuple const pt_2_;
                tuple const pt_3_;

                /// ------------------------------------------------------------
                /// the edge vectors
                tuple const e1_; /// pt_2_ - pt_1_
                tuple const e2_; /// pt_3_ - pt_1_

                /// ------------------------------------------------------------
                /// vertex normals
                tuple const n_1_;
                tuple const n_2_;
                tuple const n_3_;

            public:
                triangle(tuple pt_1, tuple pt_2, tuple pt_3, bool cast_shadow = true);
                triangle(tuple pt_1, tuple pt_2, tuple pt_3, tuple n_1, tuple n_2, tuple n_3,
                         bool cast_shadow = true);

                /// ------------------------------------------------------------
                /// access various values
                tuple p1() const;
                tuple p2() const;
                tuple p3() const;

                tuple e1() const;
                tuple e2() const;

                tuple n1() const;
                tuple n2() const;
                tuple n3() const;

            public:
                /// stringified reperesentation of a triangle
                std::string stringify() const override;

                /// compute intersection of a ray with the triangle
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// normal vector at a given point on the triangle (in local
                /// coordinates)
                tuple normal_at_local(tuple const&, intersection_record const&) const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the triangle, and return
                /// 'true' iff 'R' intersects before 'distance'.
                ///
                /// return 'false' otherwise
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-sphere intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };

        bool operator==(triangle const& lhs, triangle const& rhs);

} // namespace raytracer
