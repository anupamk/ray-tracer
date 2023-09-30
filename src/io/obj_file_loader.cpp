/*
 * this file implements the OBJ file loader for the raytracer
 **/

/// c++ includes
#include <array>
#include <charconv>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

/// our includes
#include "common/include/logging.h"
#include "file_utils/mmapped_file_reader.hpp"
#include "io/obj_file_loader.hpp"
#include "io/obj_parse_result.hpp"
#include "primitives/tuple.hpp"
#include "shapes/triangle.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// is this token valid ?
        bool ascii_token_t::is_valid() const
        {
                return valid;
        }

        /// --------------------------------------------------------------------
        /// is this end-of-line ?
        bool ascii_token_t::is_eol() const
        {
                return eol;
        }

        /// --------------------------------------------------------------------
        /// is this end-of-file ?
        bool ascii_token_t::is_eof() const
        {
                return eof;
        }

        /// --------------------------------------------------------------------
        /// this function is called to stringify a token instance
        std::string ascii_token_t::stringify() const
        {
                std::stringstream ss("");

                if (is_valid()) {
                        ss << "ascii-token: {"
                           << "is_valid:'" << this->is_valid() << "', "
                           << "is_eof:'" << this->is_eof() << "', "
                           << "token-start-index:'" << this->ts << "', "
                           << "token-end-index:'" << this->te << "', "
                           << "token_value: '" << this->value << "'}";
                } else {
                        ss << "ascii-token: {"
                           << "is_valid:'" << this->is_valid() << "', "
                           << "is_eol:'" << this->is_eol() << "', "
                           << "is_eof:'" << this->is_eof() << "', "
                           << "token-start-index:'-', "
                           << "token-end-index:'-', "
                           << "token_value: '-'}";
                }

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// face vertex and vertex-normal indices
        face_vi_vni::face_vi_vni(int32_t v_i, int32_t vn_i)
            : v_i_(v_i)
            , vn_i_(vn_i)
        {
        }

        int32_t face_vi_vni::vi() const
        {
                return v_i_;
        }

        bool face_vi_vni::v_i_isvalid() const
        {
                return !(v_i_ == 0);
        }

        int32_t face_vi_vni::vni() const
        {
                return vn_i_;
        }

        bool face_vi_vni::vn_i_isvalid() const
        {
                return !(vn_i_ == 0);
        }

        /// --------------------------------------------------------------------
        /// obj-file parser implementation
        obj_file_parser::obj_file_parser(std::string file_name)
            : obj_file(file_name)
            , ri(0)
            , ei(obj_file.size())
        {
        }

        /// --------------------------------------------------------------------
        /// this function is called to parse a OBJ formatted file. it returns a
        /// 'result' instance which can then be loaded into a scene.
        obj_parse_result obj_file_parser::parse() const
        {
                char const* obj_file_data = reinterpret_cast<char const*>(obj_file.data());
                obj_parse_result result;
                bool continue_parsing = true;

                /// ------------------------------------------------------------
                /// continue parsing till we reach e.o.f or the parser indicates
                /// a failure (which it does as soon as it encounters an error)
                for (auto tok = get_next_token(obj_file_data); (continue_parsing && !tok.is_eof());
                     tok      = get_next_token(obj_file_data)) {
                        if (!tok.is_valid()) {
                                continue;
                        }

                        LOG_DEBUG("%s", tok.stringify().c_str());

                        continue_parsing = parse_obj_token(result, tok, obj_file_data);
                }

                return result;
        }

        /*
         * only private functions from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// this is the lexer, it returns an instance of ascii_token_t which
        /// contains a token that can be then be acted upon by the parser.
        ascii_token_t obj_file_parser::get_next_token(char const* data) const
        {
                ascii_token_t ret_val;
                bool tokenize_more = true;

                while ((ri < ei) && (tokenize_more == true)) {
                        auto cc = data[ri];

                        switch (cc) {
                        default: {
                                /// --------------------------------------------
                                /// handle non-whitespace character
                                tokenize_more = tokenizer_handle_non_ws_char(data, ret_val);
                        } break;

                        case ' ':
                        case '\f':
                        case '\r':
                        case '\t':
                        case '\v': {
                                /// --------------------------------------------
                                /// handle specific whitespace characters
                                tokenize_more = tokenizer_handle_ws_char(data, ret_val);
                        } break;

                        case '#': {
                                /// --------------------------------------------
                                /// handle comment
                                tokenize_more = tokenizer_handle_comment_char(data, ret_val);
                        } break;

                        case '\\': {
                                /// --------------------------------------------
                                /// handle line continuation
                                tokenize_more = tokenizer_handle_line_contd_char(data, ret_val);
                        } break;

                        case '\n': {
                                /// --------------------------------------------
                                /// handle eol
                                tokenize_more = tokenizer_handle_eol_char(data, ret_val);
                        } break;
                        }
                }

                /// ------------------------------------------------------------
                /// nothing more to do
                if (ri >= ei) {
                        ret_val.eof = true;
                }

                return ret_val;
        }

        /// --------------------------------------------------------------------
        /// this function is called to handle a non-whitespace character in the
        /// input stream.
        bool obj_file_parser::tokenizer_handle_non_ws_char(char const* data, ascii_token_t& tok) const
        {
                tok.ts = ri;

                /// ------------------------------------------------------------
                /// just continue consuming the input stream till a whitespace
                /// character is encountered. at which point in time, we have
                /// the complete token, which is the returned.
                do {
                        tok.te = ri;
                        ri += 1;
                } while ((ri < ei) && (!isspace(data[ri])));

                /// ------------------------------------------------------------
                /// set the token up
                auto const tok_len = tok.te - tok.ts + 1;
                tok.value          = std::string_view(&data[tok.ts], tok_len);
                tok.valid          = true;
                tok.eof            = false;

                /// don't continue tokenizing
                return false;
        }

        /// --------------------------------------------------------------------
        /// this function is called to handle a non-whitespace character in the
        /// input stream.
        bool obj_file_parser::tokenizer_handle_ws_char(char const* data, ascii_token_t& tok) const
        {
                tok = ascii_token_t{};

                /// ------------------------------------------------------------
                /// eat all whitespace characters except new-line
                do {
                        tok.te = ri;
                        ri += 1;
                } while ((ri < ei) && ((data[ri] == ' ') || (data[ri] == '\f') || (data[ri] == '\r') ||
                                       (data[ri] == '\t') || data[ri] == '\v'));

                /// continue tokenizing
                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to handle a comment character in the input
        /// stream. just eat everything till we reach e.o.l
        bool obj_file_parser::tokenizer_handle_comment_char(char const* data, ascii_token_t& tok) const
        {
                do {
                        ri += 1;
                } while ((ri < ei) && (data[ri] != '\n'));

                /// continue tokenizing
                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to handle a e.o.l i.e. a newline token
        bool obj_file_parser::tokenizer_handle_line_contd_char(char const* data, ascii_token_t& tok) const
        {
                tok       = ascii_token_t{};
                tok.valid = false;

                do {
                        ri += 1;
                } while ((ri < ei) && (!isspace(data[ri])));

                /// continue tokenizing
                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to handle a e.o.l i.e. a newline token
        bool obj_file_parser::tokenizer_handle_eol_char(char const* data, ascii_token_t& tok) const
        {
                tok = ascii_token_t{};
                ri += 1;

                /// setup the token
                tok.eol   = true;
                tok.valid = false;

                /// continue tokenizing
                return false;
        }

        /// --------------------------------------------------------------------
        /// this function is called to parse a token
        bool obj_file_parser::parse_obj_token(obj_parse_result& result, ascii_token_t const& tok,
                                              char const* data) const
        {
                if (!tok.is_valid()) {
                        return false;
                }

                if (tok.value == std::string_view("v")) {
                        return parse_vertex_data(result, data);
                } else if (tok.value == std::string_view("f")) {
                        return parse_face_data(result, data);
                } else if (tok.value == std::string_view("g")) {
                        return parse_named_group_data(result, data);
                } else if (tok.value == std::string_view("vn")) {
                        return parse_vertex_normal_data(result, data);
                } else {
                        result.unknown_tokens_ref() += 1;
                }

                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to parse data for a single vertex
        bool obj_file_parser::parse_vertex_data(obj_parse_result& result, char const* data) const
        {
                std::array<double, 3> vertex_data; /// 3 ∵ x, y, z coordinates

                /// ------------------------------------------------------------
                /// no point checking for e.o.l in vertex data. things will get
                /// sorted out when we continue parsing.
                for (uint8_t i = 0; i < 3; i++) {
                        ascii_token_t tok = {};

                        do {
                                tok = get_next_token(data);
                        } while (!tok.is_valid());

                        /// ----------------------------------------------------
                        /// whoopsie ! got an end-of-file in middle of parsing
                        /// vertex data. cannot do much.
                        if (tok.is_eof()) {
                                return false;
                        }

                        LOG_DEBUG("vertex-tok:'%s'", tok.stringify().c_str());

                        if (!tok.is_valid()) {
                                return false;
                        }

                        auto maybe_tok_data = parse_token_value_as_number<double>(tok.value);

                        /// ----------------------------------------------------
                        /// bad value
                        if (!maybe_tok_data.has_value()) {
                                return false;
                        }

                        vertex_data[i] = maybe_tok_data.value();
                }

                result.vertex_list_ref().emplace_back(create_point(vertex_data[0],   /// x
                                                                   vertex_data[1],   /// y
                                                                   vertex_data[2])); /// z

                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to parse data for a single vertex
        bool obj_file_parser::parse_face_data(obj_parse_result& result, char const* data) const
        {
                /// ------------------------------------------------------------
                /// a 'face' stanza can contain 3 or more vertices. this stanza
                /// terminates with an end-of-line character.
                ///
                /// '3' or more vertices because, when we have:
                ///    - '3' vertices, face describes a triangle
                ///    - '> 3' vertices, face describes a convex polygon
                std::vector<face_vi_vni> face_vertices;

                while (true) {
                        ascii_token_t tok = get_next_token(data);

                        /// ----------------------------------------------------
                        /// ok, we got an eol while parsing face command
                        /// i.e. there is no more vertices for this face. we are
                        /// done here.
                        if (tok.is_eol()) {
                                break;
                        }

                        /// ----------------------------------------------------
                        /// whoopsie ! got an end-of-file || invalid-token in
                        /// middle of parsing vertex data. cannot do much.
                        if ((tok.is_eof()) || (!tok.is_valid())) {
                                LOG_ERROR("invalid token:'%s'", tok.stringify().c_str());
                                return false;
                        }

                        /// ----------------------------------------------------
                        /// ok, so tok.value looks something like
                        ///     '1/123/452' or '1//3'
                        /// split the token up using '/' as delimiter, and parse
                        /// the values
                        auto face_indices = split(tok.value, "/");
                        if (face_indices.size() > 3) {
                                LOG_ERROR("bad face token:'%s'", std::string(tok.value).c_str());
                                return false;
                        }

                        /// ----------------------------------------------------
                        /// a face *must* have a vertex-index defined
                        auto maybe_face_v_i = parse_token_value_as_number<int32_t>(face_indices[0]);
                        if (!maybe_face_v_i.has_value()) {
                                return false;
                        }
                        int32_t v_i = maybe_face_v_i.value();

                        /// ----------------------------------------------------
                        /// face *may* have a vertex-normal-index defined
                        int32_t vn_i = 0;
                        if (face_indices.size() > 1) {
                                /// --------------------------------------------
                                /// vertex-normal-index will be the last index
                                /// always
                                auto vn_index = face_indices.size() - 1;
                                auto maybe_face_vn_i =
                                        parse_token_value_as_number<int32_t>(face_indices[vn_index]);
                                vn_i = maybe_face_vn_i.has_value() ? maybe_face_vn_i.value() : 0;
                        }

                        face_vertices.emplace_back(face_vi_vni(v_i, vn_i));
                }

                LOG_DEBUG("number of face vertices:'%zu'", face_vertices.size());

                /// ------------------------------------------------------------
                /// bad face data : number of vertices in a face is wrong
                if (face_vertices.size() < 3) {
                        return false;
                }

                /// ------------------------------------------------------------
                /// bad face data: invalid vertex || vertex-normal index
                for (auto const& vi_vni : face_vertices) {
                        if (!result.vertex_index_is_valid(vi_vni.vi())) {
                                return false;
                        }

                        if ((vi_vni.vni() != 0) && !result.vertex_normal_index_is_valid(vi_vni.vni())) {
                                return false;
                        }
                }

                /// ------------------------------------------------------------
                /// convex polygon
                return parse_face_polygon_data(result, face_vertices);
        }

        /// --------------------------------------------------------------------
        /// create a triangle instance either using vertices or both vertices
        /// and normals (on those vertices)
        std::shared_ptr<triangle>
        obj_file_parser::create_triangle_from_face_data(obj_parse_result const& result, /// vertex-values
                                                        face_vi_vni const& pt_1, face_vi_vni const& pt_2,
                                                        face_vi_vni const& pt_3) const

        {
                if (!pt_1.vn_i_isvalid() && !pt_2.vn_i_isvalid() && !pt_3.vn_i_isvalid()) {
                        return std::make_shared<triangle>(result.vertex(pt_1.vi()),  /// pt-1-vertex
                                                          result.vertex(pt_2.vi()),  /// pt-2-vertex
                                                          result.vertex(pt_3.vi())); /// pt-3-vertex
                }

                return std::make_shared<triangle>(result.vertex(pt_1.vi()),          /// pt-1-vertex
                                                  result.vertex(pt_2.vi()),          /// pt-2-vertex
                                                  result.vertex(pt_3.vi()),          /// pt-3-vertex
                                                  result.vertex_normal(pt_1.vni()),  /// pt-1-normal
                                                  result.vertex_normal(pt_2.vni()),  /// pt-2-normal
                                                  result.vertex_normal(pt_3.vni())); /// pt-3-normal
        }

        /// --------------------------------------------------------------------
        /// the face data that we got represents a convex polygon. triangulate
        /// it, and add to the default group.
        ///
        /// by the time we are here, we are sure that vertex-indexes describing
        /// this face are all valid.
        bool obj_file_parser::parse_face_polygon_data(obj_parse_result& result,
                                                      std::vector<face_vi_vni> const& face_vertices) const
        {
                auto recent_group_maybe = result.get_recent_group_ref();
                auto dst_group_ref      = recent_group_maybe.has_value() ? recent_group_maybe.value() :
                                                                           result.default_group_ref();

                /// ------------------------------------------------------------
                /// fan triangulation
                for (size_t i = 1; i < face_vertices.size() - 1; i++) {
                        auto tmp = create_triangle_from_face_data(result,
                                                                  face_vertices[0],      /// pinned
                                                                  face_vertices[i],      /// var-vertex-1
                                                                  face_vertices[i + 1]); /// var-vertex-2

                        dst_group_ref->add_child(tmp);
                }

                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to parse a named 'group' token from the
        /// input stream.
        bool obj_file_parser::parse_named_group_data(obj_parse_result& result, char const* data) const
        {
                ascii_token_t tok = get_next_token(data);

                /// ------------------------------------------------------------
                /// unexpected token here, cannot do much
                if (tok.is_eof() || tok.is_eol()) {
                        return false;
                }

                LOG_DEBUG("group-name token:'%s'", tok.stringify().c_str());

                result.group_list_ref().push_back(std::make_shared<group>());

                return true;
        }

        /// --------------------------------------------------------------------
        /// this function is called to vertex-normal data
        bool obj_file_parser::parse_vertex_normal_data(obj_parse_result& result, char const* data) const
        {
                std::array<double, 3> vertex_data; /// 3 ∵ x, y, z coordinates

                /// ------------------------------------------------------------
                /// no point checking for e.o.l in vertex data. things will get
                /// sorted out when we continue parsing.
                for (uint8_t i = 0; i < 3; i++) {
                        ascii_token_t tok = {};

                        do {
                                tok = get_next_token(data);
                        } while (!tok.is_valid());

                        /// ----------------------------------------------------
                        /// whoopsie ! got an end-of-file || invalid-token in
                        /// middle of parsing vertex data. cannot do much.
                        if ((tok.is_eof()) || (!tok.is_valid())) {
                                LOG_ERROR("invalid token:'%s'", tok.stringify().c_str());
                                return false;
                        }

                        auto maybe_tok_data = parse_token_value_as_number<double>(tok.value);

                        /// ----------------------------------------------------
                        /// bad value
                        if (!maybe_tok_data.has_value()) {
                                return false;
                        }

                        vertex_data[i] = maybe_tok_data.value();
                }

                result.vertex_normal_list_ref().emplace_back(create_vector(vertex_data[0],   /// x
                                                                           vertex_data[1],   /// y
                                                                           vertex_data[2])); /// z

                return true;
        }

        /// --------------------------------------------------------------------
        /// split a string on delimiters
        std::vector<std::string_view> obj_file_parser::split(std::string_view str,
                                                             std::string_view delims) const
        {
                std::vector<std::string_view> retval;

                for (auto first = str.data(), second = str.data(), last = first + str.size();
                     second != last && first != last; first = second + 1) {
                        second = std::find_first_of(first, last, std::cbegin(delims), std::cend(delims));

                        if (first != second)
                                retval.emplace_back(first, second - first);
                }

                return retval;
        }

        /// --------------------------------------------------------------------
        /// parse 'string_view' as a number (int/float/double/...)
        template <typename T>
        std::optional<T> obj_file_parser::parse_token_value_as_number(std::string_view const& tok_val) const
        {
                auto val_begin = tok_val.data();
                auto val_end   = tok_val.data() + tok_val.size();

                /// ------------------------------------------------------------
                /// when 'p' points to the 'val_end' it implies that all
                /// characters in the token [val_begin...val_end] were used
                /// to parse this value.
                T ret        = {};
                auto [p, ec] = std::from_chars(val_begin, val_end, ret);
                if (p != val_end) {
                        LOG_ERROR("bad token value:'%s'", std::string(tok_val).c_str());

                        return std::nullopt;
                }

                return ret;
        }

} // namespace raytracer
