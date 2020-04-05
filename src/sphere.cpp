/*
 * implement the raytracer sphere
**/

/// c++ includes
#include <memory>
#include <optional>
#include <sstream>

/// our includes
#include "sphere.hpp"
#include "intersection_record.hpp"

namespace raytracer
{
        sphere::sphere(double radius)
            : radius_(radius)
        {
                ASSERT(radius >= 0.0);
        }


        std::string sphere::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << std::fixed    << std::left
                   << "center: ("   << this->center()       << "), "
                   << "radius: "    << this->radius()       << ", "
                   << "material: (" << this->get_material() << ")"
                   ;
                // clang-format on

                return ss.str();
        }

        /*
         * this function is called to compute the result of a ray 'R'
         * intersecting a sphere of radius 'r' centered at origin.
         *
         * intuitively, the point of intersection 'P', must lie on both the ray
         * and the sphere. therefore, it must satisfy their equations
         * simultaneously.
         *
         * in parametric form, the sphere and ray equations respectively, are:
         *
         *     P^2 - r^2 = 0 ................................. (1)
         *     P = R.origin + t * R.direction ................ (2)
         *
         * substituting (2) in (1) gives a quadratic equation in 't'. the
         * solution of which (via canonical means) gives us the desired
         * intersection points.
         **/
        std::optional<intersection_records> sphere::intersect(ray_t const& r) const
        {
                const auto R = r.transform(this->inv_transform());

                /// vector from sphere's center to the ray-origin
                const auto sphere_to_ray = R.origin() - this->center();
                const auto ray_dir	 = R.direction();

                /// compute the coefficients of the quadratic equation
                const auto A = dot(ray_dir, ray_dir);
                const auto B = 2.0 * dot(ray_dir, sphere_to_ray);
                const auto C = dot(sphere_to_ray, sphere_to_ray) - 1;

                /// aaand get the roots
                if (auto const roots = quadratic_real_roots(A, B, C)) {
                        return intersection_records{
                                intersection_record(roots->first, shared_from_this()),
                                intersection_record(roots->second, shared_from_this()),
                        };
                }

                return std::nullopt;
        }

        ///
        /// 'reasonably' formatted information of the sphere
        ///
        std::ostream& operator<<(std::ostream& os, sphere const& S)
        {
                return os << S.stringify();
        }
} // namespace raytracer
