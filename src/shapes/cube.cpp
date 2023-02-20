/*
 * implement raytracer shape cube
 **/

/// c++ includes
#include <cmath>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>

/// our includes
#include "primitives/intersection_record.hpp"
#include "shapes/cube.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        cube::cube(bool cast_shadow)
            : shape_interface(cast_shadow)
        {
        }

        /// --------------------------------------------------------------------
        /// this function is called to compute the intersection of a ray with
        /// the plane.
        std::optional<intersection_records> cube::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// normal vector at a given point on the cube (in local coordinates)
        tuple cube::normal_at_local(tuple const& pt, intersection_record const&) const
        {
                auto const abs_x = std::abs(pt.x());
                auto const abs_y = std::abs(pt.y());
                auto const abs_z = std::abs(pt.z());
                auto const max_c = std::max(abs_x, std::max(abs_y, abs_z));

                if (max_c == abs_x) {
                        return create_vector(pt.x(), 0.0, 0.0);
                } else if (max_c == abs_y) {
                        return create_vector(0.0, pt.y(), 0.0);
                }

                return create_vector(0.0, 0.0, pt.z());
        }

        /// --------------------------------------------------------------------
        /// this function is called to see iff the intersection of the ray 'R'
        /// with this cube happens before 'distance'.
        ///
        /// return 'true' if it does, 'false' otherwise.
        bool cube::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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

        /// ------------------------------------------------------------
        /// stringified representation of a cube
        std::string cube::stringify() const
        {
                std::stringstream ss("");
                ss << "ray-tracer-cube";
                return ss.str();
        }

        /*
         * only private member functions from this point onwards
         **/

        /// ------------------------------------------------------------
        /// actual workhorse for computing ray-cube intersections
        std::optional<intersection_records> cube::compute_intersections_(ray_t const& R) const
        {
                auto const [x_tmin, x_tmax] = check_axes_(R.origin().x(), R.direction().x());
                auto const [y_tmin, y_tmax] = check_axes_(R.origin().y(), R.direction().y());
                auto const [z_tmin, z_tmax] = check_axes_(R.origin().z(), R.direction().z());

                double const t_min = std::max(x_tmin, std::max(y_tmin, z_tmin));
                double const t_max = std::min(x_tmax, std::min(y_tmax, z_tmax));

                if (t_min <= t_max) {
                        return intersection_records{intersection_record(t_min, shared_from_this()),
                                                    intersection_record(t_max, shared_from_this())};
                }

                return std::nullopt;
        }

        /// --------------------------------------------------------------------
        /// this function is called to check where a ray intersects an
        /// axis-aligned-plane.
        std::tuple<double, double> cube::check_axes_(double origin, double direction) const
        {
                double const one_by_direction = 1.0 / direction;
                double tmin                   = (-1.0 - origin) * one_by_direction;
                double tmax                   = (1.0 - origin) * one_by_direction;

                if (tmin > tmax)
                        return std::tie(tmax, tmin);

                return std::tie(tmin, tmax);
        }

} // namespace raytracer
