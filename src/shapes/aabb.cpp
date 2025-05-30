/*
 * implement the raytracer axis-aligned-bounding-box
 **/
#include "shapes/aabb.hpp"

/// c++ includes
#include <algorithm>
#include <cmath>
#include <cstdlib>

/// our includes
#include "primitives/matrix.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// create an aabb instance with defaults min+max dimensions
        aabb::aabb()
        {
        }

        /// --------------------------------------------------------------------
        /// create an aabb instance with specific min+max points.
        aabb::aabb(tuple min_point, tuple max_point)
            : min_(min_point)
            , max_(max_point)
        {
        }

        /// --------------------------------------------------------------------
        /// get this aabb instance 'min' point
        tuple aabb::min() const
        {
                return min_;
        }

        /// --------------------------------------------------------------------
        /// get this aabb instance 'max' point
        tuple aabb::max() const
        {
                return max_;
        }

        /// --------------------------------------------------------------------
        /// add a point 'p' to this aabb instance
        void aabb::add_point(tuple const& p)
        {
                auto tmp_min_x = std::min(p.x(), min_.x());
                auto tmp_min_y = std::min(p.y(), min_.y());
                auto tmp_min_z = std::min(p.z(), min_.z());

                auto tmp_max_x = std::max(p.x(), max_.x());
                auto tmp_max_y = std::max(p.y(), max_.y());
                auto tmp_max_z = std::max(p.z(), max_.z());

                /// ------------------------------------------------------------
                /// update the 'min' and 'max' to enclose the point 'p' within
                /// this instance of the bounding-box
                min_ = create_point(tmp_min_x, tmp_min_y, tmp_min_z);
                max_ = create_point(tmp_max_x, tmp_max_y, tmp_max_z);
        }

        /// --------------------------------------------------------------------
        /// add a new bounding-box to this aabb instance.
        void aabb::add_box(aabb const& new_box)
        {
                add_point(new_box.min());
                add_point(new_box.max());
        }

        /// --------------------------------------------------------------------
        /// does this bounding-box instance contain the point 'p' ?
        bool aabb::contains(tuple const& p) const
        {
                /// ------------------------------------------------------------
                /// returns 'true' when 'x' is between 'v1' and 'v2', 'false'
                /// otherwise.
                auto is_in_range = [](double v1, double v2, double x) -> bool {
                        return ((std::min(v1, v2) <= x) && (std::max(v1, v2) >= x));
                };

                return (is_in_range(min_.x(), max_.x(), p.x()) && /// pt.x
                        is_in_range(min_.y(), max_.y(), p.y()) && /// pt.y
                        is_in_range(min_.z(), max_.z(), p.z()));  /// pt.z
        }

        /// --------------------------------------------------------------------
        /// does this bounding-box instance contain the 'other' bounding-box ?
        bool aabb::contains(aabb const& other) const
        {
                return contains(other.min()) && contains(other.max());
        }

        /// --------------------------------------------------------------------
        /// apply a transformation matrix to bounding box, which creates a new
        /// axis-aligned bounding-box
        aabb aabb::transform(fsize_dense2d_matrix_t const& mat) const
        {
                tuple all_pts[] = {
                        min_,                                       /// 1
                        create_point(min_.x(), min_.y(), max_.z()), /// 2
                        create_point(min_.x(), max_.y(), min_.z()), /// 3
                        create_point(min_.x(), max_.y(), max_.z()), /// 4
                        create_point(max_.x(), min_.y(), min_.z()), /// 5
                        create_point(max_.x(), min_.y(), max_.z()), /// 6
                        create_point(max_.x(), max_.y(), min_.z()), /// 7
                        max_,                                       /// 8
                };

                aabb xformed_bb{};
                for (auto pt_i : all_pts) {
                        xformed_bb.add_point(mat * pt_i);
                }

                return xformed_bb;
        }

        /// --------------------------------------------------------------------
        /// a predicate to compute if a ray intersects a bounding box
        bool aabb::intersects(ray_t const& R) const
        {
                auto const [x_tmin, x_tmax] =
                        check_axes_(R.origin().x(), R.direction().x(), min_.x(), max_.x());

                auto const [y_tmin, y_tmax] =
                        check_axes_(R.origin().y(), R.direction().y(), min_.y(), max_.y());

                auto const [z_tmin, z_tmax] =
                        check_axes_(R.origin().z(), R.direction().z(), min_.z(), max_.z());

                double const t_min = std::max(x_tmin, std::max(y_tmin, z_tmin));
                double const t_max = std::min(x_tmax, std::min(y_tmax, z_tmax));

                return (t_min <= t_max);
        }

        /// --------------------------------------------------------------------
        /// split a bounding box into two halves such that they cover the same
        /// volume as the original bounding box.
        ///
        /// splitting is performed in the simplest possible fashion by choosing
        /// an axis that is the largest.
        std::pair<aabb, aabb> aabb::split_bounds() const
        {
                auto x0 = min_.x();
                auto x1 = max_.x();
                auto y0 = min_.y();
                auto y1 = max_.y();
                auto z0 = min_.z();
                auto z1 = max_.z();

                /// ------------------------------------------------------------
                /// find largest of the 3 axes along which the box lies...
                auto aabb_x_size   = std::abs(x0 - x1);
                auto aabb_y_size   = std::abs(y0 - y1);
                auto aabb_z_size   = std::abs(z0 - z1);
                auto aabb_max_size = std::max({aabb_x_size, aabb_y_size, aabb_z_size});

                /// ------------------------------------------------------------
                /// ... and split along that.
                if (aabb_x_size == aabb_max_size) {
                        x1 = x0 + aabb_x_size / 2.0;
                        x0 = x1;
                } else if (aabb_y_size == aabb_max_size) {
                        y1 = y0 + aabb_y_size / 2.0;
                        y0 = y1;
                } else {
                        z1 = z0 + aabb_z_size / 2.0;
                        z0 = z1;
                }

                auto mid_min = create_point(x0, y0, z0);
                auto mid_max = create_point(x1, y1, z1);

                auto left_aabb  = aabb(min_, mid_max);
                auto right_aabb = aabb(mid_min, max_);

                return {left_aabb, right_aabb};
        }

        /*
         * only private member functions from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// this function is called to check where a ray intersects an
        /// axis-aligned-plane.
        std::tuple<double, double> aabb::check_axes_(double origin, double direction, double min,
                                                     double max) const
        {
                double const one_by_direction = 1.0 / direction;
                double tmin                   = (min - origin) * one_by_direction;
                double tmax                   = (max - origin) * one_by_direction;

                if (tmin > tmax)
                        return std::tie(tmax, tmin);

                return std::tie(tmin, tmax);
        }

} // namespace raytracer
