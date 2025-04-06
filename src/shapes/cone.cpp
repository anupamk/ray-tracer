/*
 * implement raytracer shape cone
 **/

#include "shapes/cone.hpp"

/// c++ includes
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/// our includes
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "utils/badge.hpp"
#include "utils/constants.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        cone::cone()
            : cone(true,      /// cast-shadow
                   -INF, INF, /// y min+max
                   false)     /// capped
        {
        }

        cone::cone(bool cast_shadow, float min, float max, bool capped)
            : shape_interface(cast_shadow)
            , min_y(min)
            , max_y(max)
            , capped(capped)
        {
        }

        /// --------------------------------------------------------------------
        /// compute intersection of a ray with the plane
        std::optional<intersection_records> cone::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff 'R' can intersect this sphere before 'distance'.
        ///
        /// return 'false' otherwise
        bool cone::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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
        /// compute normal vector at a given point on the cone surface
        tuple cone::normal_at_local(tuple const& P, intersection_record const&) const
        {
                auto const dist = P.x() * P.x() + P.z() * P.z();

                if (dist < 1.0) {
                        if (P.y() >= (max_y - EPSILON)) {
                                return create_vector(0.0, 1.0, 0.0);
                        } else if (P.y() <= (min_y + EPSILON)) {
                                return create_vector(0.0, -1.0, 0.0);
                        }
                }

                auto y = std::sqrt(dist);
                if (P.y() > 0) {
                        y = -y;
                }

                return create_vector(P.x(), y, P.z());
        }

        /// stringified representation of a cube
        std::string cone::stringify() const
        {
                std::stringstream ss("");
                ss << "ray-tracer-cone: {";
                ss << "cast_shadow: " << (get_cast_shadow() ? "yes" : "no") << ", "
                   << "min_y: " << min_y << ", "
                   << "max_y: " << max_y << ", "
                   << "capped: " << capped << "}";

                return ss.str();
        }

        /*
         * only private methods from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// this function is called to return the 'A' component of the quadratic
        /// equation for ray-cone intersection.
        ///
        /// A = ray-dir-x^2 - ray-dir-y^2 + ray-dir-z^2
        float cone::A(ray_t const& R) const
        {
                auto const& ray_dir = R.direction();

                return ((ray_dir.x() * ray_dir.x()) - (ray_dir.y() * ray_dir.y()) +
                        (ray_dir.z() * ray_dir.z()));
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the 'B' component of the quadratic
        /// equation for ray-cone intersection.
        ///
        /// B = 2.0 * (ray-origin-x * ray-dir-x - ray-origin-y * ray-dir-y + ray-origin-z * ray-dir-z)
        float cone::B(ray_t const& R) const
        {
                auto const& ray_dir    = R.direction();
                auto const& ray_origin = R.origin();

                return 2.0 * ((ray_origin.x() * ray_dir.x()) - (ray_origin.y() * ray_dir.y()) +
                              (ray_origin.z() * ray_dir.z()));
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the 'C' component of the quadratic
        /// equation for ray-cone intersection.
        ///
        /// B = ray-origin-x^2 - ray-origin-y^2 + ray-origin-z^2
        float cone::C(ray_t const& R) const
        {
                auto const& ray_origin = R.origin();
                return ((ray_origin.x() * ray_origin.x()) - (ray_origin.y() * ray_origin.y()) +
                        (ray_origin.z() * ray_origin.z()));
        }

        /// --------------------------------------------------------------------
        /// this function is called to check if the intersection of the ray 'R'
        /// at point 't' falls within the cone i.e. in [min_y_ .. max_y_]
        ///
        /// it returns true when y-intersection is within the range, false
        /// otherwise.
        bool cone::intersection_in_range(ray_t const& R, double t) const
        {
                auto y_xs = R.origin().y() + t * R.direction().y();
                return (min_y < y_xs) && (y_xs < max_y);
        }

        /// ------------------------------------------------------------
        /// actual workhorse for computing ray-cone intersections
        std::optional<intersection_records> cone::compute_intersections_(ray_t const& R) const
        {
                intersection_records retval;

                /// ------------------------------------------------------------
                /// coefficients of the quadratic equation for ray-cone
                /// intersection
                auto const a = A(R);
                auto const b = B(R);
                auto const c = C(R);

                /// ------------------------------------------------------------
                /// the ray, R,  will completely miss the cone when a &
                /// b are both zero.
                auto const a_is_zero = epsilon_equal(std::abs(a), 0.0);
                auto const b_is_zero = epsilon_equal(std::abs(b), 0.0);

                if (unlikely(a_is_zero && b_is_zero)) {
                        return std::nullopt;
                }

                if (a_is_zero && !b_is_zero) {
                        /// ----------------------------------------------------
                        /// single point of intersection
                        auto const xs_point = -c / (2.0 * b);
                        if (intersection_in_range(R, xs_point)) {
                                retval.push_back(intersection_record{xs_point, shared_from_this()});
                        }
                } else if (auto const roots = quadratic_real_roots(a, b, c)) {
                        auto root_1 = roots->first;
                        auto root_2 = roots->second;

                        if (root_1 > root_2)
                                std::swap(root_1, root_2);

                        /// ----------------------------------------------------
                        /// ok, so we got real roots. but we must ensure
                        /// that the y-coordinate at the point of
                        /// intersection, lies in the range [min_y
                        /// .. max_y] to be valid (otherwise, the ray
                        /// misses the cone)
                        auto const y_xs1_in_range = intersection_in_range(R, root_1);
                        auto const y_xs2_in_range = intersection_in_range(R, root_2);

                        if (y_xs1_in_range) {
                                retval.push_back(intersection_record{roots->first, shared_from_this()});
                        }

                        if (y_xs2_in_range) {
                                retval.push_back(intersection_record{roots->second, shared_from_this()});
                        }
                }

                /// ------------------------------------------------------------
                /// intersect the cone "caps"
                compute_caps_intersections_(R, retval);

                if (retval.size()) {
                        return retval;
                }

                return std::nullopt;
        }

        /// --------------------------------------------------------------------
        /// this function is called to update intersections that have been
        /// computed so far with possible intersections due to closed ends of
        /// this cone
        void cone::compute_caps_intersections_(ray_t const& R, intersection_records& xs) const
        {
                auto const ray_y_dir    = R.direction().y();
                auto const ray_y_origin = R.origin().y();

                /// ------------------------------------------------------------
                /// cone is not capped, and or, ray's y-dir is almost 0
                if (!capped || (std::abs(ray_y_dir) < EPSILON)) {
                        return;
                }

                /// ------------------------------------------------------------
                /// radius of a cone at a given y-value is the absolute value of
                /// that y
                auto cone_radius_at_y = [](float y) -> float { return std::abs(y); };

                /// ------------------------------------------------------------
                /// return true if the intersection at the point 't' is within
                /// the radius of cone i.e. 1 from the y-axis
                auto ensure_caps_intersection = [](ray_t const& R, double t, float r) -> bool {
                        auto const x = R.origin().x() + t * R.direction().x();
                        auto const z = R.origin().z() + t * R.direction().z();

                        return ((x * x + z * z) <= (r + EPSILON));
                };

                /// ------------------------------------------------------------
                /// now check for intersection with the cap i.e. plane at
                /// cone's min.
                auto cap_xs_min_pt = (min_y - ray_y_origin) / ray_y_dir;
                if (ensure_caps_intersection(R, cap_xs_min_pt, cone_radius_at_y(min_y))) {
                        xs.push_back(intersection_record{cap_xs_min_pt, shared_from_this()});
                }

                /// ------------------------------------------------------------
                /// now check for intersection with the cap i.e. plane at
                /// cone's max.
                auto cap_xs_max_pt = (max_y - ray_y_origin) / ray_y_dir;
                if (ensure_caps_intersection(R, cap_xs_max_pt, cone_radius_at_y(max_y))) {
                        xs.push_back(intersection_record{cap_xs_max_pt, shared_from_this()});
                }

                return;
        }

} // namespace raytracer
