/*
 * implement the raytracer csg shape
 **/

/// ----------------------------------------------------------------------------
/// c++ includes
#include "common/include/assert_utils.h"
#include <memory>
#include <optional>
#include <sstream>
#include <utility>

/// ----------------------------------------------------------------------------
/// our includes
#include "badge.hpp"
#include "csg.hpp"
#include "intersection_record.hpp"
#include "ray.hpp"

namespace raytracer
{
        /*
         * csg-operation : union
         **/
        std::string csg_union::stringify() const
        {
                return "union";
        }

        bool csg_union::intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                             bool hit_inside_right_shape) const
        {
                return ((left_shape_hit && !hit_inside_right_shape) ||
                        (!left_shape_hit && !hit_inside_left_shape));
        }

        /*
         * csg-operation : intersection
         **/
        std::string csg_intersection::stringify() const
        {
                return "intersection";
        }

        bool csg_intersection::intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                                    bool hit_inside_right_shape) const
        {
                return ((left_shape_hit && hit_inside_right_shape) ||
                        (!left_shape_hit && hit_inside_left_shape));
        }

        /*
         * csg-operation : difference
         **/
        std::string csg_difference::stringify() const
        {
                return "difference";
        }

        bool csg_difference::intersection_allowed(bool left_shape_hit, bool hit_inside_left_shape,
                                                  bool hit_inside_right_shape) const
        {
                return ((left_shape_hit && !hit_inside_right_shape) ||
                        (!left_shape_hit && hit_inside_left_shape));
        }

        std::shared_ptr<csg_shape> csg_shape::create_csg(std::shared_ptr<shape_interface> left,
                                                         std::shared_ptr<csg_operation> csg_op,
                                                         std::shared_ptr<shape_interface> right,
                                                         bool cast_shadow)
        {
                auto new_csg_shape = new csg_shape(left, csg_op, right, cast_shadow);
                ASSERT(new_csg_shape != nullptr);

                std::shared_ptr<csg_shape> csg_sp(new_csg_shape);

                left->set_parent(csg_sp);
                right->set_parent(csg_sp);

                return csg_sp;
        }

        /*
         * csg-shape interface related routines
         **/
        csg_shape::csg_shape(std::shared_ptr<shape_interface> left, std::shared_ptr<csg_operation> csg_op,
                             std::shared_ptr<shape_interface> right, bool cast_shadow)
            : shape_interface(cast_shadow)
            , l_shape(left)
            , csg_op(csg_op)
            , r_shape(right)
        {
        }

        std::shared_ptr<shape_interface> csg_shape::left() const
        {
                return l_shape;
        }

        std::shared_ptr<shape_interface> csg_shape::right() const
        {
                return r_shape;
        }

        std::shared_ptr<csg_operation> csg_shape::operation() const
        {
                return csg_op;
        }

        tuple csg_shape::normal_at_local(tuple const&, intersection_record const&) const
        {
                ASSERT_FAIL("csg_shape does not have normal !");
        }

        /// --------------------------------------------------------------------
        /// stringified representation of the csg operation
        std::string csg_shape::stringify() const
        {
                std::stringstream ss("");

                ss << "ray-tracer-csg-shape: {"
                   << "left-shape: " << this->l_shape->stringify() << ", "
                   << "right-shape: " << this->r_shape->stringify() << ", "
                   << "operation: " << this->csg_op->stringify() << "}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// compute intersections of a ray
        std::optional<intersection_records> csg_shape::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff 'R' can intersect this csg-shape before
        /// 'distance'.
        ///
        /// return 'false' otherwise
        bool csg_shape::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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
        /// does this shape include the other ? returns 'true' if it does,
        /// 'false' otherwise.
        bool csg_shape::includes(std::shared_ptr<const shape_interface> const& other) const
        {
                return l_shape->includes(other) || r_shape->includes(other);
        }

        /// --------------------------------------------------------------------
        /// from a given list of intersections, produce a subset of
        /// intersections that conform to the csg operation.
        std::optional<intersection_records>
        csg_shape::filter_intersections(intersection_records const& xs_list) const
        {
                intersection_records filtered_xs;
                filtered_xs.reserve(xs_list.size());

                bool in_left  = false;
                bool in_right = false;

                for (const auto& xs_i : xs_list) {
                        auto xs_i_obj = xs_i.what_object();
                        bool left_hit = l_shape->includes(xs_i_obj);

                        if (csg_op->intersection_allowed(left_hit, in_left, in_right)) {
                                filtered_xs.push_back(xs_i);
                        }

                        if (left_hit) {
                                in_left = !in_left;
                        } else {
                                in_right = !in_right;
                        }
                }

                if (likely(filtered_xs.size() > 0)) {
                        return filtered_xs;
                }

                return std::nullopt;
        }

        /*
         * only private methods from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// compute ray-csg shape intersections
        std::optional<intersection_records> csg_shape::compute_intersections_(ray_t const& R) const
        {
                auto l_xs = R.intersect(this->l_shape);
                auto r_xs = R.intersect(this->r_shape);

                /// ------------------------------------------------------------
                /// early exit for no intersections at all
                if (unlikely(!l_xs.has_value() && !r_xs.has_value())) {
                        return std::nullopt;
                }

                auto xs_list = merge_intersection_records(std::move(l_xs.value_or(intersection_records{})),
                                                          std::move(r_xs.value_or(intersection_records{})));

                return filter_intersections(xs_list);
        }

} // namespace raytracer
