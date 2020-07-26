#ifndef RAYTRACER_SPHERE_HPP__
#define RAYTRACER_SPHERE_HPP__

/// c++ includes
#include <optional>
#include <ostream>
#include <string>

/// our includes
#include "shape_interface.hpp"
#include "intersection_record.hpp"
#include "tuple.hpp"
#include "ray.hpp"
#include "assert_utils.h"
#include "uv_point.hpp"

namespace raytracer
{
        /*
         * this defines an origin centered sphere.
        **/
        class sphere final : public shape_interface
        {
            private:
                const tuple center_ = create_point(0.0, 0.0, 0.0);
                double radius_	    = 0.0;

            public:
                sphere(double radius = 1.0);

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
                std::optional<intersection_records> intersect(the_badge<ray_t>, ray_t const& R) const override;

                /// normal vector at a given point on the sphere (in local
                /// coordinates)
                tuple normal_at_local(tuple const&) const override;

                /// ------------------------------------------------------------
                /// map a point on the surface of the sphere to a corresponding
                /// uv-value (on a texture)
                uv_point map_to_uv(tuple const&) const override;
        };

        std::ostream& operator<<(std::ostream& os, sphere const& S);

} // namespace raytracer

#endif /// RAYTRACER_SPHERE_HPP__
