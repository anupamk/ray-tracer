/*
 * this file implements the result of parsing a wavefront obj file.
 **/

#include "io/obj_parse_result.hpp"

/// c++ includes
#include <bits/std_abs.h>
#include <stdlib.h>
#include <functional>
#include <limits>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

/// our includes
#include "shapes/group.hpp"

namespace raytracer
{
        obj_parse_result::obj_parse_result()
            : default_group_(std::make_shared<group>())
        {
        }

        uint32_t& obj_parse_result::unknown_tokens_ref()
        {
                return this->unknown_tokens_;
        }

        uint32_t const& obj_parse_result::unknown_tokens_cref() const
        {
                return this->unknown_tokens_;
        }

        uint32_t& obj_parse_result::bad_faces_ref()
        {
                return this->num_bad_faces_;
        }

        uint32_t const& obj_parse_result::bad_faces_cref() const
        {
                return this->num_bad_faces_;
        }

        std::vector<tuple>& obj_parse_result::vertex_list_ref()
        {
                return this->vertex_list_;
        }

        std::vector<tuple> const& obj_parse_result::vertex_list_cref() const
        {
                return this->vertex_list_;
        }

        std::shared_ptr<group>& obj_parse_result::default_group_ref()
        {
                return this->default_group_;
        }

        std::shared_ptr<group> const& obj_parse_result::default_group_cref() const
        {
                return this->default_group_;
        }

        decltype(obj_parse_result::group_list_)& obj_parse_result::group_list_ref()
        {
                return this->group_list_;
        }

        decltype(obj_parse_result::group_list_) const& obj_parse_result::group_list_cref() const
        {
                return this->group_list_;
        }

        std::vector<tuple>& obj_parse_result::vertex_normal_list_ref()
        {
                return this->vertex_normal_list_;
        }

        std::vector<tuple> const& obj_parse_result::vertex_normal_list_cref() const
        {
                return this->vertex_normal_list_;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the reference to the most recent
        /// group that was added
        std::optional<std::shared_ptr<group>> obj_parse_result::get_recent_group_ref()
        {
                if (this->group_list_.empty()) {
                        return std::nullopt;
                }

                return *(this->group_list_.rbegin());
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a vertex instance from the list of
        /// vertices parsed from an input file. index is not validated.
        ///
        /// indexing into the vertex list is '1' based i.e. 1st vertex is at
        /// index == 1, 2nd at index == 2 etc.
        tuple const& obj_parse_result::vertex(int32_t i) const
        {
                i = get_1_based_index(i);
                return vertex_list_[i];
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a vertex instance from the list of
        /// vertices parsed from an input file.
        ///
        /// indexing into the vertex list is '1' based i.e. 1st vertex is at
        /// index == 1, 2nd at index == 2 etc.
        std::optional<std::reference_wrapper<tuple const>> obj_parse_result::checked_vertex(int32_t i) const
        {
                uint32_t k = get_1_based_index(i);

                if (!vertex_index_is_valid(k)) {
                        return std::nullopt;
                }

                return std::cref(vertex_list_[k]);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a vertex instance from the list of
        /// vertices parsed from an input file. index is not validated.
        ///
        /// indexing into the vertex list is '1' based i.e. 1st vertex is at
        /// index == 1, 2nd at index == 2 etc.
        tuple const& obj_parse_result::vertex_normal(int32_t i) const
        {
                i = get_1_based_index(i);
                return vertex_normal_list_[i];
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a vertex instance from the list of
        /// vertices parsed from an input file.
        ///
        /// indexing into the vertex list is '1' based i.e. 1st vertex is at
        /// index == 1, 2nd at index == 2 etc.
        std::optional<std::reference_wrapper<tuple const>>
        obj_parse_result::checked_vertex_normal(int32_t i) const
        {
                uint32_t k = get_1_based_index(i);

                if (!vertex_index_is_valid(k)) {
                        return std::nullopt;
                }

                return std::cref(vertex_normal_list_[k]);
        }

        /// --------------------------------------------------------------------
        /// add support for negative indexes as well
        bool obj_parse_result::vertex_index_is_valid(int32_t i) const
        {
                uint32_t non_neg_i = get_1_based_index(i);

                if (non_neg_i > this->vertex_list_.size()) {
                        return false;
                }

                return true;
        }

        /// --------------------------------------------------------------------
        /// add support for negative indexes as well
        bool obj_parse_result::vertex_normal_index_is_valid(int32_t i) const
        {
                uint32_t non_neg_i = get_1_based_index(i);

                if (non_neg_i > this->vertex_normal_list_.size()) {
                        return false;
                }

                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to summarize the result of parsing an OBJ
        /// file.
        std::string obj_parse_result::summarize() const
        {
                std::stringstream ss("");

                ss << "{"
                   << "unknown-tokens:'" << unknown_tokens_ << "', "
                   << "bad-faces:'" << num_bad_faces_ << "', "
                   << "vertex-list size:'" << vertex_list_.size() << "', "
                   << "default-group size:'" << default_group_->child_shapes_cref().size() << "', "
                   << "named-group size:'" << group_list_.size() << "', "
                   << "vertex-normal-list size:'" << vertex_normal_list_.size() << "'"
                   << "}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// return '1' based vertex-index, negative indices are from the end of
        /// *current* vertex_list.
        uint32_t obj_parse_result::get_1_based_index(int32_t i) const
        {
                /// ------------------------------------------------------------
                /// most common case
                if (i >= 1) {
                        return i - 1;
                }

                /// ------------------------------------------------------------
                /// next common
                if (i < 0) {
                        return (this->vertex_list_.size() - std::abs(i));
                }

                /// i == 0 is least common ∵ 1 based index
                return std::numeric_limits<int32_t>::max();
        }

} // namespace raytracer
