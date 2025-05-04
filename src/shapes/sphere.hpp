#pragma once

/// c++ includes
#include <memory>
#include <optional>
#include <ostream>
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

        class aabb;

        /*
         * this defines an origin centered sphere.
         **/
        class sphere final : public shape_interface
        {
            private:
                const tuple center_ = create_point(0.0, 0.0, 0.0);
                double radius_      = 0.0;

            public:
                sphere(bool cast_shadow = true, double radius = 1.0);

            public:
                constexpr tuple center() const
                {
                        return this->center_;
                }

                constexpr double radius() const
                {
                        return this->radius_;
                }

            public:
                /// stringified reperesentation of a sphere
                std::string stringify() const override;

                /// compute intersection of a ray with the sphere
                std::optional<intersection_records> intersect(the_badge<ray_t>,
                                                              ray_t const& R) const override;

                /// normal vector at a given point on the sphere (in local
                /// coordinates)
                tuple normal_at_local(tuple const&, intersection_record const&) const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the sphere, and return
                /// 'true' iff 'R' intersects before 'distance'.
                ///
                /// return 'false' otherwise
                bool has_intersection_before(the_badge<ray_t>, ray_t const& R,
                                             double distance) const override;

                /// ------------------------------------------------------------
                /// bounding box for an instance of sphere
                aabb bounds_of() const override;

            private:
                /// ------------------------------------------------------------
                /// actual workhorse for computing ray-sphere intersections
                std::optional<intersection_records> compute_intersections_(ray_t const&) const;
        };

        std::ostream& operator<<(std::ostream& os, sphere const& S);

        /// --------------------------------------------------------------------
        /// create a glass sphere
        std::shared_ptr<shape_interface> glass_sphere();

} // namespace raytracer
