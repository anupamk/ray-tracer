/*
 * implement the raytracer sphere
**/

/// c++ includes
#include <ios>
#include <memory>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <cmath>

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
        std::optional<intersection_records> sphere::intersect(the_badge<ray_t>, ray_t const& R) const
        {
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
        /// this function is called to return the normal at a point 'P' on
        /// the surface of the sphere
        ///
        tuple sphere::normal_at_local(tuple const& P) const
        {
                return create_vector(P.x(), P.y(), P.z());
        }

        /// --------------------------------------------------------------------
        /// this function is called to map a point on the surface of the sphere
        /// to a corresponding uv-value. this can then be used to determine the
        /// color of the sphere at that point (via a uv-texture)
        uv_point sphere::map_to_uv(tuple const& P) const
        {
                /// ------------------------------------------------------------
                /// compute the azimuthal (along the x-z axis) angle theta,
                /// which varies over the range (-π..π]
                ///
                /// theta increases clockwise when viewed from above. but this
                /// will be fixed later.
                auto const theta = std::atan2(P.x(), P.z());

                /// ------------------------------------------------------------
                /// compute the polar (along the y-x axis) angle which varies
                /// over the range [0..π].
                auto const phi = std::acos(P.y()/radius());

                /// ------------------------------------------------------------
                /// 0.5 < raw_u ≤ 0.5
                auto const raw_u = theta / (2 * PI);

                /// ------------------------------------------------------------
                /// fix the direction as well i.e subtract from 1 so that it
                /// increases counter-clockwise (when viewed from above)
                auto const u = 1 - (raw_u + 0.5);

                /// ------------------------------------------------------------
                /// polar angle is '0' at the south pole of the sphere and is
                /// '1' at the north pole. so we flip it over
                auto const v = 1.0 - phi / PI;

                return uv_point(u, v);
        }



        ///
        /// 'reasonably' formatted information of the sphere
        ///
        std::ostream& operator<<(std::ostream& os, sphere const& S)
        {
                return os << S.stringify();
        }
} // namespace raytracer
