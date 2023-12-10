#pragma once

/*
 * this file implements rudimentary parsing of Wavefront's OBJ file format for
 * loading 3d models described by such files.
 **/

/// c++ includes
#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/// our includes
#include "file_utils/mmapped_file_reader.hpp"
#include "io/obj_parse_result.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class triangle;

        /// --------------------------------------------------------------------
        /// a token from an obj file. obj file tokens are simply character
        /// sequences separated by whitespaces
        class ascii_token_t final
        {
            public:
                static constexpr size_t INVALID_INDEX = ~0;

                /// ------------------------------------------------------------
                /// token-start and token-end index mark beginning and end of a
                /// token. the range is inclusive i.e. [ts..te]. the 'value'
                /// part contains the value of the token
                size_t ts = INVALID_INDEX;
                size_t te = INVALID_INDEX;
                std::string_view value;

                /// ------------------------------------------------------------
                /// token contains a valid value only when 'is_valid' is
                /// true.
                bool valid = true;

                /// ------------------------------------------------------------
                /// when the tokenizer returns a token with eol set, it implies
                /// that we are done with the current line of input.
                bool eol = false;

                /// ------------------------------------------------------------
                /// when the tokenizer returns a token with eof set, that
                /// implies we are done with parsing the input. ofcourse, when
                /// eof is set, valid cannot be 'true'
                bool eof = false;

            public:
                /// ------------------------------------------------------------
                /// is the token valid ?
                bool is_valid() const;

                /// ------------------------------------------------------------
                /// is the token end-of-line ?
                bool is_eol() const;

                /// ------------------------------------------------------------
                /// is the token end-of-file ?
                bool is_eof() const;

                /// ------------------------------------------------------------
                /// stringified representation of the token
                std::string stringify() const;
        };

        /// --------------------------------------------------------------------
        /// this structure represents both the vertex-index and
        /// vertex-normal-index for a given face instance.
        ///
        /// since these indices are '1' based, '0' is considered an invalid
        /// index.
        class face_vi_vni final
        {
            private:
                int32_t v_i_  = 0;
                int32_t vn_i_ = 0;

            public:
                face_vi_vni(int32_t v_i, int32_t vn_i);

            public:
                int32_t vi() const;
                bool v_i_isvalid() const;

                int32_t vni() const;
                bool vn_i_isvalid() const;
        };

        /*
         * this class implements parser for Wavefront's OBJ file
         **/
        class obj_file_parser final
        {
            private:
                file_utils::mmapped_file_reader obj_file;

                /// ------------------------------------------------------------
                /// imagine, that the whole input file is looong string, with
                /// 'ri' indicating the current read-index i.e. where we are
                /// reading from NOW.
                mutable size_t ri;

                /// ------------------------------------------------------------
                /// end-index: ri will always be less than this.
                size_t const ei;

            public:
                obj_file_parser(std::string file_name);

            public:
                obj_parse_result parse() const;

            private:
                /// ------------------------------------------------------------
                /// this is the lexer for OBJ formatted files. it extracts the
                /// next token, and returns it's particulars in the 'tok'
                /// instance.
                ///
                /// beginning of input data, is pointed to by 'data', with the
                /// current-read-index being 'ci' and 'ei' indicating
                /// end-of-data-index.
                ///
                /// each call to this function results in an ascii-token
                /// instance being populated. the function returns 'true' if
                /// more lexemes ^^) can be extracted from the input stream,
                /// 'false' otherwise.
                ///
                /// so essentially, this operates in lock-step with its
                /// counter-part i.e. parse_XXX(...) which consumes whatever is
                /// generated here.
                ascii_token_t get_next_token(char const* data) const;

                /// ------------------------------------------------------------
                /// these routines take care of individual pieces of tokenizing
                /// the input stream.
                ///
                /// when more elements in the input-stream need to be examined,
                /// these functions return 'true'. otherwise, no more input is
                /// used, and current value of the ascii_token_t is returned.
                bool tokenizer_handle_non_ws_char(char const* data, ascii_token_t&) const;
                bool tokenizer_handle_ws_char(char const* data, ascii_token_t&) const;
                bool tokenizer_handle_comment_char(char const* data, ascii_token_t&) const;
                bool tokenizer_handle_line_contd_char(char const* data, ascii_token_t&) const;
                bool tokenizer_handle_eol_char(char const* data, ascii_token_t&) const;

                /// ------------------------------------------------------------
                /// this is the top-level parser routine. it is responsible for
                /// parsing various OBJ commands.
                ///
                /// when it returns 'true', it implies that parsing last OBJ
                /// command succeeded, and we can continue parsing more tokens
                /// from the input stream.
                ///
                /// each successful invokation of this, updates the parse-result.
                bool parse_obj_token(obj_parse_result& result, ascii_token_t const& tok,
                                     char const* data) const;

                /*
                 * following parse_xxx(...) functions are responsible for
                 * parsing a specific OBJ command.
                 *
                 * the 'data' stream is also passed here because more tokens
                 * need to be generally extracted from the input stream to
                 * complete the parsing of a command.
                 *
                 * for example, a vertex command looks like this:
                 *
                 *    "v 1.23 4.567 8.90123"
                 *
                 * on encountering a 'v' in the input stream, we know that we
                 * are parsing vertex information. so 'parse_vertex_data(...)'
                 * is invoked. which, as part of its processing needs to extract
                 * 3 tokens to complete its task.
                 **/

                /// ------------------------------------------------------------
                /// parse vertex data
                bool parse_vertex_data(obj_parse_result& result, char const* data) const;

                /// ------------------------------------------------------------
                /// parse face data
                bool parse_face_data(obj_parse_result& result, char const* data) const;

                /// ------------------------------------------------------------
                /// create an instance of triangle from face vertex and
                /// vertex-normal index
                std::shared_ptr<triangle> create_triangle_from_face_data(obj_parse_result const& result,
                                                                         face_vi_vni const& pt_1,
                                                                         face_vi_vni const& pt_2,
                                                                         face_vi_vni const& pt_3) const;

                /// ------------------------------------------------------------
                /// parse face polygon data
                bool parse_face_polygon_data(obj_parse_result& result,
                                             std::vector<face_vi_vni> const& face_vertices) const;

                /// ------------------------------------------------------------
                /// parse named group data
                bool parse_named_group_data(obj_parse_result& result, char const* data) const;

                /// ------------------------------------------------------------
                /// parse vertex-normal data
                bool parse_vertex_normal_data(obj_parse_result& result, char const* data) const;

                /// ------------------------------------------------------------
                /// split a string_view instance according to delimiters
                std::vector<std::string_view> split(std::string_view str, std::string_view delims) const;

                /// ------------------------------------------------------------
                /// parse a string_view as a number (int/float/double/...)
                template <typename T>
                std::optional<T> parse_token_value_as_number(std::string_view const& tok_val) const;
        };

} // namespace raytracer
