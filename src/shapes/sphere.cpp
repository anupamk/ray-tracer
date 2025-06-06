/*
 * implement the raytracer sphere
 **/
#include "shapes/sphere.hpp"

/// c++ includes
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/// our includes
#include "common/include/assert_utils.h"
#include "patterns/material.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "shapes/aabb.hpp"
#include "utils/badge.hpp"
#include "utils/constants.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        sphere::sphere(bool cast_shadow, double radius)
            : shape_interface(cast_shadow)
            , radius_(radius)
        {
                ASSERT(radius >= 0.0);
        }

        /// --------------------------------------------------------------------
        /// just forward the computation to the actual workhorse.
        std::optional<intersection_records> sphere::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the normal at a point 'P' on
        /// the surface of the sphere
        tuple sphere::normal_at_local(tuple const& P, intersection_record const&) const
        {
                return create_vector(P.x(), P.y(), P.z());
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff 'R' can intersect this sphere before 'distance'.
        ///
        /// return 'false' otherwise
        bool sphere::has_intersection_before(the_badge<ray_t> b, ray_t const& R, double distance) const
        {
                auto maybe_xs_records = compute_intersections_(R);

                if (!maybe_xs_records.has_value()) {
                        return false;
                }

                auto const& xs_records = maybe_xs_records.value();
                for (auto const& xs_i : xs_records) {
                        auto const where = xs_i.where();
                        if ((where >= EPSILON) && (where < distance)) {
                                return true;
                        }
                }

                return false;
        }

        /// --------------------------------------------------------------------
        /// return the bounding box for this instance of the sphere.
        aabb sphere::bounds_of() const
        {
                auto min_pt = create_point(-1.0, -1.0, -1.0);
                auto max_pt = create_point(1.0, 1.0, 1.0);

                return aabb(min_pt, max_pt);
        }

        /// --------------------------------------------------------------------
        /// 'divide' a sphere into nothing ... nothing at all
        void sphere::divide(size_t threshold)
        {
        }

        /// --------------------------------------------------------------------
        /// stringified representation of sphere's information
        std::string sphere::stringify() const
        {
                std::stringstream ss("");

                ss << "ray-tracer-sphere: {"
                   << "center: " << this->center() << ", "
                   << "radius: " << this->radius() << ", "
                   << "material: " << this->get_material() << ", "
                   << "grouped: " << this->is_grouped() << "}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// this function is called to compute the result of a ray 'R'
        /// intersecting a sphere of radius 'r' centered at origin.
        ///
        /// intuitively, the point of intersection 'P', must lie on both the ray
        /// and the sphere. therefore, it must satisfy their equations
        /// simultaneously.
        ///
        /// in parametric form, the sphere and ray equations respectively, are:
        ///
        ///            P^2 - r^2 = 0 ................................. (1)
        ///            P = R.origin + t * R.direction ................ (2)
        ///
        /// substituting (2) in (1) gives a quadratic equation in 't'. the
        /// solution of which (via canonical means) gives us the desired
        /// intersection points.
        std::optional<intersection_records> sphere::compute_intersections_(ray_t const& R) const
        {
                /// vector from sphere's center to the ray-origin
                auto const sphere_to_ray = R.origin() - this->center();
                auto const ray_dir       = R.direction();

                /// compute the coefficients of the quadratic equation
                auto const A = dot(ray_dir, ray_dir);
                auto const B = 2.0 * dot(ray_dir, sphere_to_ray);
                auto const C = dot(sphere_to_ray, sphere_to_ray) - 1;

                /// aaand get the roots
                if (auto const roots = quadratic_real_roots(A, B, C)) {
                        return intersection_records{
                                intersection_record(roots->first, shared_from_this()),
                                intersection_record(roots->second, shared_from_this()),
                        };
                }

                return std::nullopt;
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted information of the sphere
        std::ostream& operator<<(std::ostream& os, sphere const& S)
        {
                return os << S.stringify();
        }

        /// --------------------------------------------------------------------
        /// this function is called to return an instance of a glassy sphere
        std::shared_ptr<shape_interface> glass_sphere()
        {
                auto tmp_sphere      = std::make_shared<sphere>();
                auto glassy_material = material()
                                               .set_transparency(1.0)                     /// transparent
                                               .set_refractive_index(material::RI_GLASS); /// made-of-glass

                tmp_sphere->set_material(glassy_material);

                return tmp_sphere;
        }

} // namespace raytracer
