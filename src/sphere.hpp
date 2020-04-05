#ifndef RAYTRACER_SPHERE_HPP__
#define RAYTRACER_SPHERE_HPP__

/// c++ includes
#include <optional>

/// our includes
#include "shape_interface.hpp"
#include "intersection_record.hpp"
#include "tuple.hpp"
#include "ray.hpp"
#include "assert_utils.h"

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
                std::string stringify() const;

                /// compute intersection of a ray with the sphere
                std::optional<intersection_records> intersect(ray_t const& r) const override;
        };

        std::ostream& operator<<(std::ostream& os, sphere const& S);

} // namespace raytracer

#endif /// RAYTRACER_SPHERE_HPP__
