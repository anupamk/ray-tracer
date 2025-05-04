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

                /// ------------------------------------------------------------
                /// update the bounding box encompassing this child shape.
                auto cs_aabb = new_shape->parent_space_bounds_of(new_shape);
                bounding_box_.add_box(cs_aabb);
        }

        /// --------------------------------------------------------------------
        /// return a const-reference to child shapes
        decltype(group::child_shapes_) const& group::child_shapes_cref() const
        {
                return child_shapes_;
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

} // namespace raytracer
