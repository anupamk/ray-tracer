#pragma once

/*
 * this file implements the result of parsing an OBJ file. it was better to
 * organize parsed results into a separate file for various reasons.
 **/

/// c++ includes
#include <stdint.h>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/// our includes
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class group;

        /// --------------------------------------------------------------------
        /// this describes the result of parsing an obj file
        class obj_parse_result final
        {
            private:
                /// ------------------------------------------------------------
                /// number of unknown tokens that we got
                uint32_t unknown_tokens_ = 0;

                /// ------------------------------------------------------------
                /// number of malformed face stanzas
                uint32_t num_bad_faces_ = 0;

                /// ------------------------------------------------------------
                /// vertex list
                std::vector<tuple> vertex_list_ = {};

                /// ------------------------------------------------------------
                /// the default group which receives all the geometries
                std::shared_ptr<group> default_group_ = nullptr;

                /// ------------------------------------------------------------
                /// group instances, other than the default group
                std::vector<std::shared_ptr<group>> group_list_ = {};

                /// ------------------------------------------------------------
                /// vertex-normal list
                std::vector<tuple> vertex_normal_list_ = {};

            public:
                obj_parse_result();

            public:
                /// ------------------------------------------------------------
                /// ref and const-ref for unknown_tokens
                uint32_t& unknown_tokens_ref();
                uint32_t const& unknown_tokens_cref() const;

                /// ------------------------------------------------------------
                /// ref and const-ref for bad-faces
                uint32_t& bad_faces_ref();
                uint32_t const& bad_faces_cref() const;

                /// ------------------------------------------------------------
                /// ref and const-ref for vertex_list
                std::vector<tuple>& vertex_list_ref();
                std::vector<tuple> const& vertex_list_cref() const;

                /// ------------------------------------------------------------
                /// ref and const-ref for default_group
                std::shared_ptr<group>& default_group_ref();
                std::shared_ptr<group> const& default_group_cref() const;

                /// ------------------------------------------------------------
                /// ref and const-ref for group-list
                decltype(group_list_)& group_list_ref();
                decltype(group_list_) const& group_list_cref() const;

                /// ------------------------------------------------------------
                /// get the most recent group that was added
                std::optional<std::shared_ptr<group>> get_recent_group_ref();

                /// ------------------------------------------------------------
                /// a new instance of default group is returned. each invokation
                /// returns a different instance of the default group.
                std::shared_ptr<group> clone_default_group() const;

                /// ------------------------------------------------------------
                /// a new instance of group list is returned. each invokation
                /// returns a different instance of the group list.
                decltype(group_list_) clone_group_list() const;

                /// ------------------------------------------------------------
                /// ref and const-ref for vertex_list
                std::vector<tuple>& vertex_normal_list_ref();
                std::vector<tuple> const& vertex_normal_list_cref() const;

                /// ------------------------------------------------------------
                /// return 'ith' vertex from the vertex-list. this is a '1'
                /// based index, so first vertex is at index 'i == 1', second
                /// one at index 'i == 2' etc. etc.
                ///
                /// note that we cannot return a std::optional<T const&> so, and
                /// despite equivalence, we don't want to return a
                /// std::optional<T const*>, so we do the next best thing i.e. a
                /// reference_wrapper.
                ///
                /// both checked and unchecked versions of these is provided.
                tuple const& vertex(int32_t i) const;
                std::optional<std::reference_wrapper<const tuple>> checked_vertex(int32_t i) const;

                /// ------------------------------------------------------------
                /// return 'ith' vertex from the vertex-normal list. this is a
                /// '1' based index, so first vertex-normal is at index 'i ==
                /// 1', second one at index 'i == 2' etc. etc.
                ///
                /// note that we cannot return a std::optional<T const&> so, and
                /// despite equivalence, we don't want to return a
                /// std::optional<T const*>, so we do the next best thing i.e. a
                /// reference_wrapper.
                ///
                /// both checked and unchecked versions of these is provided.
                tuple const& vertex_normal(int32_t i) const;
                std::optional<std::reference_wrapper<tuple const>> checked_vertex_normal(int32_t i) const;

                /// ------------------------------------------------------------
                /// is the vertex-index 'i' valid for vertex_list that we have
                /// so far ?
                ///
                /// returns 'true' if vertex-index is valid, 'false' otherwise.
                bool vertex_index_is_valid(int32_t i) const;
                bool vertex_normal_index_is_valid(int32_t i) const;

                /// ------------------------------------------------------------
                /// generate a summary / stats of this result instance
                std::string summarize() const;

            private:
                /// ------------------------------------------------------------
                /// return '1' based vertex index. useful for reading actual
                /// vertex values
                uint32_t get_1_based_index(int32_t i) const;
        };

} // namespace raytracer
