/**
 * this file implements the notion of combining multiple shapes into a single
 * unit via the notion of object groups.
 *
 * this allows for operations f.e. scaling, rotation, translations etc. etc. on
 * a group of shapes.
 **/

#include "shapes/group.hpp"

/// c++ includes
#include <algorithm>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

/// our includes
#include "common/include/assert_utils.h"
#include "patterns/material.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "shapes/aabb.hpp"
#include "shapes/shape_interface.hpp"
#include "utils/badge.hpp"
#include "utils/constants.hpp"
#include <utility>

namespace raytracer
{
        group::group(bool cast_shadow)
            : shape_interface(cast_shadow)
            , child_shapes_({})
        {
        }

        std::shared_ptr<shape_interface> group::get_ptr()
        {
                return shared_from_this();
        }

        /// --------------------------------------------------------------------
        /// this function is called to compute the intersection of the ray with
        /// group
        std::optional<intersection_records> group::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// compute normal at a given point for the group
        tuple group::normal_at_local(tuple const&, intersection_record const&) const
        {
                ASSERT_FAIL("groups don't have normal !");
                return create_point(0.0, 0.0, 0.0);
        }

        /// --------------------------------------------------------------------
        /// stringified representation of this shape
        std::string group::stringify() const
        {
                std::stringstream ss("");
                ss << "ray-tracer-group: {";
                ss << "total-shapes: '" << child_shapes_.size() << "'}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// just push the material properties to all children of a group.
        ///
        /// this might be a slow operation for deeply nested large groups,
        /// because it happens during 'world creation' and not rendering, so we
        /// should be ok with it...
        void group::set_material(material const& M)
        {
                std::transform(this->child_shapes_.begin(), /// start
                               this->child_shapes_.end(),   /// end
                               this->child_shapes_.begin(), /// put results here
                               [=](auto cs) {
                                       auto modifiable_cs = const_cast<shape_interface*>(cs.get());
                                       modifiable_cs->set_material(M);

                                       return cs;
                               });
        }

        /// --------------------------------------------------------------------
        /// this function is called to compute the intersection of the ray with
        /// group
        bool group::has_intersection_before(the_badge<ray_t>, ray_t const& R, double distance) const
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
        /// return the bounding box for this instance of a group of shapes.
        aabb group::bounds_of() const
        {
                return bounding_box_;
        }

        /// --------------------------------------------------------------------
        /// this function is called to see if the group is empty or not. it
        /// returns 'true' if the group is empty, 'false' otherwise.
        bool group::is_empty() const
        {
                return child_shapes_.empty();
        }

        /// --------------------------------------------------------------------
        /// this function is called to check if the specified shape is present
        /// in the group.
        bool group::includes(std::shared_ptr<shape_interface const> const& the_shape) const
        {
                return std::find_if(child_shapes_.cbegin(), child_shapes_.cend(),
                                    /// ----------------------------------------
                                    /// a child shape must include 'the_shape'
                                    [&](auto const& cs) { return cs->includes(the_shape); }) !=
                       child_shapes_.cend();
        }

        /// --------------------------------------------------------------------
        /// this function is called to add a child shape to a group
        void group::add_child(std::shared_ptr<shape_interface> new_shape)
        {
                new_shape->set_parent(get_ptr());
                child_shapes_.push_back(new_shape);
                update_aabb(new_shape);
        }

        /// --------------------------------------------------------------------
        /// return a const-reference to child shapes
        decltype(group::child_shapes_) const& group::child_shapes_cref() const
        {
                return child_shapes_;
        }

        /// --------------------------------------------------------------------
        /// returns two lists, or "buckets", containing the children that fit
        /// into the corresponding halves of the group's bounding box.
        std::pair<std::vector<std::shared_ptr<shape_interface>>, std::vector<std::shared_ptr<shape_interface>>>
        group::partition_children()
        {
                std::vector<std::shared_ptr<shape_interface>> left_shapes;
                std::vector<std::shared_ptr<shape_interface>> right_shapes;

                auto [left_aabb, right_aabb] = bounding_box_.split_bounds();
                bool recompute_aabb          = false;

                for (auto cs_i_iter = child_shapes_.begin(); cs_i_iter != child_shapes_.end();) {
                        auto cs_i_bounds = (*cs_i_iter)->parent_space_bounds_of();

                        /// ----------------------------------------------------
                        /// add to the left
                        if (left_aabb.contains(cs_i_bounds)) {
                                left_shapes.push_back(*cs_i_iter);
                                cs_i_iter      = child_shapes_.erase(cs_i_iter);
                                recompute_aabb = true;
                                continue;
                        }

                        /// ----------------------------------------------------
                        /// add to the right
                        if (right_aabb.contains(cs_i_bounds)) {
                                right_shapes.push_back(*cs_i_iter);
                                cs_i_iter      = child_shapes_.erase(cs_i_iter);
                                recompute_aabb = true;
                                continue;
                        }

                        /*
                         * if the i'th child-shape does not belong to either the
                         * left/right shapes, just leave them alone.
                         **/
                        ++cs_i_iter;
                }

                /// ------------------------------------------------------------
                /// the cached bounding box has become stale, let's recompute
                /// it.
                if (recompute_aabb == true) {
                        bounding_box_ = {};
                        for (const auto& cs_i : child_shapes_) {
                                update_aabb(cs_i);
                        }
                }

                return {left_shapes, right_shapes};
        }

        /// --------------------------------------------------------------------
        /// 'divide' a group into sub-groups if number of child shapes that it
        /// contains is than the 'threshold'.
        ///
        /// ofcourse, this needs to be performed recursively till the entire
        /// group is 'chopped' up.
        void group::divide(size_t threshold)
        {
                if (child_shapes_.size() > threshold) {
                        auto [left, right] = partition_children();

                        if (left.empty() == false) {
                                make_subgroup(left);
                        }

                        if (right.empty() == false) {
                                make_subgroup(right);
                        }

                        /// ----------------------------------------------------
                        /// invoke it on each of the children as well.
                        for (const auto& cs_i : child_shapes_) {
                                cs_i->divide(threshold);
                        }
                }
        }

        /// --------------------------------------------------------------------
        /// create a sub-group from a list of children.
        void group::make_subgroup(std::vector<std::shared_ptr<shape_interface>> shape_list)
        {
                auto new_subgroup = std::make_shared<group>();
                for (auto& cs_i : shape_list) {
                        new_subgroup->add_child(cs_i);
                }

                add_child(new_subgroup);
        }

        /*
         * only private functions from this point onwards
         **/
        std::optional<intersection_records> group::compute_intersections_(ray_t const& R) const
        {
                if (bounding_box_.intersects(R) == false) {
                        return std::nullopt;
                }

                intersection_records xs_result;
                xs_result.reserve(2 * child_shapes_.size());

                for (auto const& cs : child_shapes_) {
                        auto cs_xs_record = R.intersect(cs);

                        if (!cs_xs_record) {
                                continue;
                        }

                        auto cs_xs_list = cs_xs_record.value();
                        for (auto& cs_xs : cs_xs_list) {
                                xs_result.push_back(cs_xs);
                        }
                }

                /// ------------------------------------------------------------
                /// required otherwise, we return std::optional with an empty
                /// value, which is not very useful...
                if (xs_result.empty()) {
                        return std::nullopt;
                }

                std::sort(xs_result.begin(), xs_result.end());

                return xs_result;
        }

        /// --------------------------------------------------------------------
        /// update the bounding box encompassing this child shape.
        void group::update_aabb(std::shared_ptr<shape_interface const> new_shape)
        {
                auto cs_aabb = new_shape->parent_space_bounds_of();
                bounding_box_.add_box(cs_aabb);
        }

} // namespace raytracer
