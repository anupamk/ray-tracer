/*
 * implement the 'xz-plane'
 **/
#include "shapes/plane.hpp"

/// c++ includes
#include <optional>
#include <sstream>
#include <string>
#include <vector>

/// our includes
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/aabb.hpp"
#include "utils/badge.hpp"
#include "utils/constants.hpp"

namespace raytracer
{
        plane::plane(bool cast_shadow)
            : shape_interface(cast_shadow)
        {
        }

        /// --------------------------------------------------------------------
        /// compute intersection of a ray with the plane
        std::optional<intersection_records> plane::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// normal vector at any point on the xz-plane is a constant i.e. along
        /// the y-axis
        tuple plane::normal_at_local(tuple const&, intersection_record const&) const
        {
                return create_vector(0.0, 1.0, 0.0);
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff 'R' can intersect the plane before
        /// 'distance'.
        ///
        /// return 'false' otherwise.
        bool plane::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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
        /// return the bounding box for this instance of the plane.
        aabb plane::bounds_of() const
        {
                auto min_pt = create_point(-INF, 0.0, -INF);
                auto max_pt = create_point(INF, 0.0, INF);

                return aabb(min_pt, max_pt);
        }

        /// --------------------------------------------------------------------
        /// 'divide' a plane into nothing ... nothing at all
        void plane::divide(size_t threshold)
        {
        }

        /// --------------------------------------------------------------------
        /// stringified representation of a plane
        std::string plane::stringify() const
        {
                std::stringstream ss("");

                ss << "XZ-PLANE";

                return ss.str();
        }

        /// ------------------------------------------------------------
        /// actual workhorse for computing ray-plane intersections
        std::optional<intersection_records> plane::compute_intersections_(ray_t const& R) const
        {
                auto const ray_y_dir = R.direction().y();

                if (std::abs(ray_y_dir) < EPSILON) {
                        return std::nullopt;
                }

                /// ------------------------------------------------------------
                /// a ray can intersect the plane at only a single
                /// point. co-planar rays are not interesting at all
                auto const xs_point = -R.origin().y() / ray_y_dir;

                return intersection_records{intersection_record(xs_point, shared_from_this())};
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted information of the sphere
        std::ostream& operator<<(std::ostream& os, plane const& P)
        {
                return os << P.stringify();
        }

} // namespace raytracer
