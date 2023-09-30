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
#include "intersection_record.hpp"
#include "ray.hpp"

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

        /// --------------------------------------------------------------------
        /// compute the position of a point at a distance 't' (from origin) on
        /// this ray
        tuple ray_t::position(double t) const
        {
                return this->origin() + this->direction() * t;
        }

        /// --------------------------------------------------------------------
        /// this function is called to apply a transformation matrix on a ray,
        /// and it returns a new ray instance
        ray_t ray_t::transform(fsize_dense2d_matrix_t const& M) const
        {
                auto const new_origin    = M * this->origin();
                auto const new_direction = M * this->direction();

                return ray_t(new_origin, new_direction);
        }

        /// --------------------------------------------------------------------
        /// stringified representation of a ray
        std::string ray_t::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << "origin: ("    << this->origin().stringify()    << "), "
                   << "direction: (" << this->direction().stringify() << ")";
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the result of a ray intersecting a
        /// shape
        std::optional<intersection_records>
        ray_t::intersect(std::shared_ptr<shape_interface const> const& S) const
        {
                PROFILE_SCOPE;

                return S->intersect({}, this->transform(S->inv_transform()));
        }

        /// --------------------------------------------------------------------
        /// this function is called to return meta-information about a specific
        /// intersection.
        intersection_info_t ray_t::prepare_computations(intersection_records const& xs_data,
                                                        size_t index) const
        {
                PROFILE_SCOPE;

                intersection_info_t retval;
                auto const& current_xs = xs_data[index];

                /// set some trivial values
                retval.point(current_xs.where())
                        .what_object(current_xs.what_object())
                        .position(position(current_xs.where()))
                        .eye_vector(-direction());

                /// ------------------------------------------------------------
                /// compute normal at intersection
                auto normal_at_xs = current_xs.what_object()->normal_at(retval.position(), current_xs);

                /// intersection is inside or outside ?
                if (raytracer::dot(normal_at_xs, retval.eye_vector()) < 0) {
                        retval.inside(true).normal_vector(-normal_at_xs);
                } else {
                        retval.inside(false).normal_vector(std::move(normal_at_xs));
                }

                /// ------------------------------------------------------------
                /// over-point and under-point are epsilon above and below the
                /// intersection respectively.
                auto over_point  = retval.position() + retval.normal_vector() * EPSILON;
                auto under_point = retval.position() - retval.normal_vector() * EPSILON;
                retval.over_position(std::move(over_point)).under_position(std::move(under_point));

                /// ------------------------------------------------------------
                /// compute reflection vector
                auto refl_vec = reflect(this->direction(), retval.normal_vector());
                retval.reflection_vector(std::move(refl_vec));

                /// ------------------------------------------------------------
                /// for a given intersection, find the refractive index of the
                /// material the ray is passing from (n1) and the refractive
                /// index of the material the ray is passing into (n2)
                std::vector<std::shared_ptr<shape_interface const>> shape_list;
                for (auto const& xs_i : xs_data) {
                        auto const hit_current = (xs_i == current_xs);
                        auto const xs_i_obj    = xs_i.what_object();

                        /// --------------------------------------------
                        /// step-01
                        if (hit_current) {
                                auto n1_val =
                                        shape_list.empty() ?
                                                material::RI_VACCUM :
                                                shape_list.back()->get_material().get_refractive_index();

                                retval.n1(n1_val);
                        }

                        /// --------------------------------------------
                        /// step-02
                        auto iter = std::find(shape_list.begin(), shape_list.end(), xs_i_obj);

                        if (iter != shape_list.end()) {
                                shape_list.erase(iter);
                        } else {
                                shape_list.push_back(xs_i_obj);
                        }

                        /// --------------------------------------------
                        /// step-03
                        if (hit_current) {
                                auto n2_val =
                                        shape_list.empty() ?
                                                material::RI_VACCUM :
                                                shape_list.back()->get_material().get_refractive_index();

                                retval.n2(n2_val);
                                break;
                        }
                }

                return retval;
        }

        /// ------------------------------------------------------------
        /// returns 'true' if this ray intersects an object from the list of
        /// world-objects before 'distance'.
        ///
        /// returns 'false' otherwise.
        bool ray_t::has_intersection_before(
                std::vector<std::shared_ptr<shape_interface const>> const& world_object_list,
                double distance) const
        {
                for (auto const& obj : world_object_list) {
                        if (!obj->get_cast_shadow()) {
                                continue;
                        }

                        auto const inv_ray = transform(obj->inv_transform());
                        if (obj->has_intersection_before({}, inv_ray, distance)) {
                                return true;
                        }
                }

                return false;
        }

        /// --------------------------------------------------------------------
        /// compare two rays, and return true iff both origin and direction of
        /// the rays are same. false otherwise
        bool operator==(ray_t const& lhs, ray_t const& rhs)
        {
                // clang-format off
                return ((lhs.origin() == rhs.origin()) &&
                        (lhs.direction() == rhs.direction()));
                // clang-format on
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted output for the ray
        std::ostream& operator<<(std::ostream& os, ray_t const& R)
        {
                return os << R.stringify();
        }

} // namespace raytracer
