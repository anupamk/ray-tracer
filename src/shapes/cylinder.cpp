/*
 * implement raytracer shape cylinder
 **/

/// c++ includes
#include <algorithm>
#include <cmath>
#include <ios>
#include <sstream>
#include <string>

/// our includes
#include "primitives/intersection_record.hpp"
#include "shapes/cylinder.hpp"
#include "utils/constants.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        cylinder::cylinder()
            : cylinder(true,      /// cast-shadow
                       -INF, INF, /// y min+max
                       false)     /// capped
        {
        }

        cylinder::cylinder(bool cast_shadow, float min, float max, bool capped)
            : shape_interface(cast_shadow)
            , min_y(min)
            , max_y(max)
            , capped(capped)
        {
        }

        /// ------------------------------------------------------------
        /// compute intersection of a ray with the cylinder, just forward the
        /// invokation ot the workhorse routine
        std::optional<intersection_records> cylinder::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// compute normal vector at a given point on the cylinder surface
        tuple cylinder::normal_at_local(tuple const& P, intersection_record const&) const
        {
                auto const dist = P.x() * P.x() + P.z() * P.z();

                if ((dist < 1.0) && (P.y() >= (max_y - EPSILON))) {
                        return create_vector(0.0, 1.0, 0.0);
                } else if ((dist < 1.0) && (P.y() <= (min_y + EPSILON))) {
                        return create_vector(0.0, -1.0, 0.0);
                }

                return create_vector(P.x(), 0.0, P.z());
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff 'R' can intersect this sphere before 'distance'.
        ///
        /// return 'false' otherwise
        bool cylinder::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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
        /// stringified representation of a cube
        std::string cylinder::stringify() const
        {
                std::stringstream ss("");
                ss << "ray-tracer-cylinder: {";
                ss << "cast_shadow: " << (get_cast_shadow() ? "yes" : "no") << ", "
                   << "min_y: " << min_y << ", "
                   << "max_y: " << max_y << ", "
                   << "capped: " << capped << ", "
                   << "grouped: " << this->is_grouped() << "}";

                return ss.str();
        }

        /*
         * only private methods from this point onwards
         **/

        /// ------------------------------------------------------------
        /// compute intersection of a ray with the cylinder
        std::optional<intersection_records> cylinder::compute_intersections_(ray_t const& R) const
        {
                intersection_records retval;

                /// ------------------------------------------------------------
                /// intersect the cylinder "wall"
                {
                        auto const ray_dir_x    = R.direction().x();
                        auto const ray_dir_z    = R.direction().z();
                        auto const ray_origin_x = R.origin().x();
                        auto const ray_origin_z = R.origin().z();

                        /// ------------------------------------------------------------
                        /// co-efficients of the quadratic equation for
                        /// ray-cylinder intersection
                        auto const A = (ray_dir_x * ray_dir_x + ray_dir_z * ray_dir_z);
                        auto const B = 2.0 * (ray_origin_x * ray_dir_x + ray_origin_z * ray_dir_z);
                        auto const C = (ray_origin_x * ray_origin_x) + (ray_origin_z * ray_origin_z) - 1.0;

                        if (auto const roots = quadratic_real_roots(A, B, C)) {
                                auto root_1 = roots->first;
                                auto root_2 = roots->second;

                                if (root_1 > root_2)
                                        std::swap(root_1, root_2);

                                /// ----------------------------------------------------
                                /// ok, so we got real roots. but we must ensure that
                                /// the y-coordinate at the point of intersection, lies
                                /// in the range [min_y .. max_y] to be valid
                                /// (otherwise, the ray misses the cylinder)
                                auto const y_xs1_in_range = intersection_in_range(R, root_1);
                                auto const y_xs2_in_range = intersection_in_range(R, root_2);

                                if (y_xs1_in_range) {
                                        retval.push_back(
                                                intersection_record{roots->first, shared_from_this()});
                                }

                                if (y_xs2_in_range) {
                                        retval.push_back(
                                                intersection_record{roots->second, shared_from_this()});
                                }
                        }
                }

                /// ------------------------------------------------------------
                /// intersect the cylinder "caps"
                compute_caps_intersections_(R, retval);

                if (retval.size()) {
                        return retval;
                }

                return std::nullopt;
        }

        /// --------------------------------------------------------------------
        /// this function is called to check if the intersection of the ray 'R'
        /// at point 't' falls within the cylinder i.e. in [min_y_ .. max_y_]
        ///
        /// it returns true when y-intersection is within the range, false
        /// otherwise.
        bool cylinder::intersection_in_range(ray_t const& R, double t) const
        {
                auto y_xs = R.origin().y() + t * R.direction().y();
                return (min_y < y_xs) && (y_xs < max_y);
        }

        /// --------------------------------------------------------------------
        /// this function is called to update intersections that have been
        /// computed so far with possible intersections due to closed ends of
        /// this cylinder
        void cylinder::compute_caps_intersections_(ray_t const& R, intersection_records& xs) const
        {
                auto const ray_y_dir    = R.direction().y();
                auto const ray_y_origin = R.origin().y();

                /// ------------------------------------------------------------
                /// cylinder is not capped, and or, ray's y-dir is almost 0
                if (!capped || (std::abs(ray_y_dir) < EPSILON)) {
                        return;
                }

                /// ------------------------------------------------------------
                /// return true if the intersection at the point 't' is within
                /// the radius of cylinder i.e. 1 from the y-axis
                auto ensure_caps_intersection = [](ray_t const& R, double t) -> bool {
                        auto const x = R.origin().x() + t * R.direction().x();
                        auto const z = R.origin().z() + t * R.direction().z();

                        return ((x * x + z * z) <= (1.0 + EPSILON));
                };

                /// ------------------------------------------------------------
                /// now check for intersection with the cap i.e. plane at
                /// cylinder's min.
                auto cap_xs_min_pt = (min_y - ray_y_origin) / ray_y_dir;
                if (ensure_caps_intersection(R, cap_xs_min_pt)) {
                        xs.push_back(intersection_record{cap_xs_min_pt, shared_from_this()});
                }

                /// ------------------------------------------------------------
                /// now check for intersection with the cap i.e. plane at
                /// cylinder's max.
                auto cap_xs_max_pt = (max_y - ray_y_origin) / ray_y_dir;
                if (ensure_caps_intersection(R, cap_xs_max_pt)) {
                        xs.push_back(intersection_record{cap_xs_max_pt, shared_from_this()});
                }

                /// ------------------------------------------------------------
                /// remove duplicates, this might happen f.e. when a ray exits
                /// the cylinder at the point where the end-cap intersects the
                /// side of the cylinder.
                std::sort(xs.begin(), xs.end());
                xs.erase(std::unique(xs.begin(), xs.end()), xs.end());

                return;
        }
} // namespace raytracer
