#ifndef RAYTRACER_PLANE_HPP__
#define RAYTRACER_PLANE_HPP__

/// c++ includes
#include <optional>
#include <ostream>
#include <string>

/// our includes
#include "badge.hpp"
#include "common/include/assert_utils.h"
#include "intersection_record.hpp"
#include "ray.hpp"
#include "shape_interface.hpp"
#include "tuple.hpp"
#include "uv_point.hpp"

namespace raytracer
{
        /*
         * this defines a xz-plane
         **/
        class plane final : public shape_interface
        {
            public:
                plane(bool cast_shadow = true);

            public:
                /// ------------------------------------------------------------
                /// stringified representation of a plane
                std::string stringify() const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the plane
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// ------------------------------------------------------------
                /// normal vector at a give point on the plane (in local
                /// coordinates)
                tuple normal_at_local(tuple const&) const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the plane, and return
                /// 'true' iff 'R' intersects before 'distance'.
                ///
                /// return 'false' otherwise
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-plane intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };

        std::ostream& operator<<(std::ostream& os, plane const& P);
} // namespace raytracer

#endif /// RAYTRACER_PLANE_HPP__
