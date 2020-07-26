/*
 * implement the raytracer ray
**/

/// c++ includes
#include "tuple.hpp"
#include <algorithm>
#include <ostream>
#include <sstream>
#include <string>

/// our includes
#include "ray.hpp"
#include "intersection_record.hpp"

namespace raytracer
{
        ray_t::ray_t(tuple origin, tuple direction)
            : origin_(origin)
            , direction_(direction)
        {
        }

        tuple ray_t::origin() const
        {
                return this->origin_;
        }

        tuple ray_t::direction() const
        {
                return this->direction_;
        }

        ///
        /// compute the position of a point at a distance 't' (from origin) on
        /// this ray
        ///
        tuple ray_t::position(double t) const
        {
                return this->origin() + this->direction() * t;
        }

        ///
        /// this function is called to apply a transformation matrix on a ray,
        /// and it returns a new ray instance
        ///
        ray_t ray_t::transform(fsize_dense2d_matrix_t const& M) const
        {
                auto const new_origin	 = M * this->origin();
                auto const new_direction = M * this->direction();

                return ray_t(new_origin, new_direction);
        }

        ///
        /// stringified representation of a ray
        ///
        std::string ray_t::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << "origin: ("    << this->origin().stringify()    << "), "
                   << "direction: (" << this->direction().stringify() << ")";
                // clang-format on

                return ss.str();
        }

        ///
        /// this function is called to return the result of a ray intersecting a
        /// shape
        ///
        std::optional<intersection_records> ray_t::intersect(std::shared_ptr<const shape_interface> const& S) const
        {
                const auto inv_ray = this->transform(S->inv_transform());
                return S->intersect({}, inv_ray);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return some information about the intersection
        intersection_info_t ray_t::prepare_computations(intersection_record xs_data) const
        {
                intersection_info_t retval;

                /// set some trivial values
                retval.point(xs_data.where())
                        .what_object(xs_data.what_object())
                        .position(position(xs_data.where()))
                        .eye_vector(-direction());

                /// compute normal at intersection
                auto const normal_at_xs = xs_data.what_object()->normal_at_world(retval.position());

                /// intersection is inside or outside ?
                if (raytracer::dot(normal_at_xs, retval.eye_vector()) < 0) {
                        retval.inside(true).normal_vector(-normal_at_xs);
                } else {
                        retval.inside(false).normal_vector(normal_at_xs);
                }

                /// over-point is slightly above the actual point of
                /// intersection
                auto const over_point = retval.position() + retval.normal_vector() * EPSILON;
                retval.over_position(over_point);

                return retval;
        }

        ///
        /// compare two rays, and return true iff both origin and direction of
        /// the rays are same. false otherwise
        ///
        bool operator==(ray_t const& lhs, ray_t const& rhs)
        {
                // clang-format off
                return ((lhs.origin() == rhs.origin()) &&
                        (lhs.direction() == rhs.direction()));
                // clang-format on
        }

        ///
        /// 'reasonably' formatted output for the ray
        ///
        std::ostream& operator<<(std::ostream& os, ray_t const& R)
        {
                return os << R.stringify();
        }

} // namespace raytracer
