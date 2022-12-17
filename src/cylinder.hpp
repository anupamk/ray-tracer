#pragma once

/// c++ includes
#include <optional>
#include <ostream>
#include <string>
#include <tuple>

/// our includes
#include "intersection_record.hpp"
#include "ray.hpp"
#include "shape_interface.hpp"
#include "tuple.hpp"

namespace raytracer
{
        /*
         * this defines an infinitely long or finite sized cylinder extending
         * along ±Y-axis having a unit radius.
         **/
        class cylinder final : public shape_interface
        {
            public:
                /// ------------------------------------------------------------
                /// instead of extending to infinity, we can truncate the
                /// cylinder between these bounds.
                ///
                /// the bounds are exclusive, so if max_y_ == 1.3 then it
                /// implies that the cylinder extends upto, but not including
                /// that limit.
                float const min_y = 0.0;
                float const max_y = 0.0;

                /// ------------------------------------------------------------
                /// is the end capped or is it open (like a pipe)
                bool const capped = false;

            public:
                /// ------------------------------------------------------------
                /// constructors etc
                cylinder();
                cylinder(bool cast_shadow, float min, float max, bool capped);

            public:
                /*
                 * overridden methods
                 **/

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the plane
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// ------------------------------------------------------------
                /// normal vector at a give point on the cube (in local
                /// coordinates)
                tuple normal_at_local(tuple const&, intersection_record const&) const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the plane, and return
                /// 'true' iff 'R' intersects before 'distance'.
                ///
                /// return 'false' otherwise
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

                /// ------------------------------------------------------------
                /// stringified representation of a cube
                std::string stringify() const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-cylinder intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;

                /// ------------------------------------------------------------
                /// this function is called to check if the intersection of the
                /// ray 'R' at point 't' falls within the cylinder i.e. in
                /// [min_y_ .. max_y_]
                ///
                /// returns 'true' when it is, 'false' otherwise.
                bool intersection_in_range(ray_t const& R, double t) const;

                /// ------------------------------------------------------------
                /// this function is called to update intersections so far with
                /// possible intersections at the cylinder's end caps
                void compute_caps_intersections_(ray_t const& R, intersection_records& xs) const;
        };

} // namespace raytracer
