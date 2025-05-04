#pragma once

/// c++ includes
#include <optional>
#include <string>

/// our includes
#include "primitives/intersection_record.hpp"
#include "primitives/tuple.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class ray_t;
        template <typename T>
        class the_badge;

        /*
         * this defines an infinitely long or finite sized cone extending
         * along ±Y-axis having a unit radius.
         **/
        class cone final : public shape_interface
        {
            public:
                /// ------------------------------------------------------------
                /// instead of extending to infinity, we can truncate the
                /// cone between these bounds.
                ///
                /// the bounds are exclusive, so if max_y_ == 1.3 then it
                /// implies that the cone extends upto, but not including
                /// that limit.
                float const min_y = 0.0;
                float const max_y = 0.0;

                /// ------------------------------------------------------------
                /// is the end capped or is it open (like a pipe)
                bool const capped = false;

            public:
                /// ------------------------------------------------------------
                /// constructors etc
                cone();
                cone(bool cast_shadow, float min, float max, bool capped);

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
                /// bounding box for an instance of cone
                aabb bounds_of() const override;

                /// ------------------------------------------------------------
                /// stringified representation of a cube
                std::string stringify() const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-cone intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;

                /// ------------------------------------------------------------
                /// this function is called to check if the intersection of the
                /// ray 'R' at point 't' falls within the cone i.e. in
                /// [min_y_ .. max_y_]
                ///
                /// returns 'true' when it is, 'false' otherwise.
                bool intersection_in_range(ray_t const& R, double t) const;

                /// ------------------------------------------------------------
                /// this function is called to update intersections so far with
                /// possible intersections at the cone's end caps
                void compute_caps_intersections_(ray_t const& R, intersection_records& xs) const;

                /// ------------------------------------------------------------
                /// cone-ray-intersection quadratic equation (A, B, C)
                /// parameters
                float A(ray_t const&) const;
                float B(ray_t const&) const;
                float C(ray_t const&) const;
        };

} // namespace raytracer
