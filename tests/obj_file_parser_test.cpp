/// c includes
#include <memory>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/// c++ includes
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

/// 3rdparty testing library
#include "doctest/doctest.h"

/// our own thing
#include "common/include/logging.h"
#include "mmapped_file_reader.hpp"
#include "obj_file_loader.hpp"
#include "triangle.hpp"
#include "tuple.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// a convenience alias
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// bunch of random tokens
TEST_CASE("obj_file_loader::load_from_file(...) trivial")
{
        std::string_view obj_data = R"""(mary  ha d       a little
lamb with fleece as white as snow
and # some comment to spice things up
      wherever
   mary 	went
the lamb \
	was sure to


         go...


)""";

        std::string fname = file_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto maybe_obj_result = p1.load_from_file(fname);

        CHECK(maybe_obj_result.has_value() == true);
        auto const parse_result = maybe_obj_result.value();
        CHECK(parse_result.unknown_tokens_cref() == 22);
        CHECK(parse_result.vertex_list_cref().size() == 0);
}

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex data
TEST_CASE("obj file with vertex data")
{
        std::string_view obj_data = R"""(# obj file with vertex data
# v[1]
v -1 1 0

# v[2] vertex data with continuation and a useless comment
# as well...
v \
# this is a useless comment
-1.000                                          \
	0.50000 \
0.0000

# v[3]
v 1 0 0

# v[4]
v 1 1 0
)""";

        std::string fname = file_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto maybe_obj_result = p1.load_from_file(fname);

        /// --------------------------------------------------------------------
        /// rudimentary checks
        CHECK(maybe_obj_result.has_value() == true);
        auto const parse_result = maybe_obj_result.value();
        CHECK(parse_result.unknown_tokens_cref() == 0);
        CHECK(parse_result.vertex_list_cref().size() == 4);

        /// --------------------------------------------------------------------
        /// vertex value checks
        CHECK(parse_result.vertex(1) == raytracer::create_point(-1.0, 1.0, 0.0));
        CHECK(parse_result.vertex(2) == raytracer::create_point(-1.0, 0.5, 0.0));
        CHECK(parse_result.vertex(3) == raytracer::create_point(1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(4) == raytracer::create_point(1.0, 1.0, 0.0));

        /// --------------------------------------------------------------------
        /// bad vertex index check
        CHECK(parse_result.checked_vertex(0) == std::nullopt);
        CHECK(parse_result.checked_vertex(100) == std::nullopt);
}

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex and face (describing triangles) data
TEST_CASE("obj file with vertex and face data")
{
        std::string_view obj_data = R"""(# obj file with vertex and face data
# v[1]
v -1 1 0

# v[2]
v -1 0 0

# v[3]
v 1 0 0

# v[4]
v 1 1 0

# f[1] : using v[1], v[2] and v[3]
f 1 2 3

# f[2] : using v[1], v[3] and v[4]
f 1 3 4
)""";

        std::string fname = file_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto maybe_obj_result = p1.load_from_file(fname);

        /// --------------------------------------------------------------------
        /// rudimentary checks
        CHECK(maybe_obj_result.has_value() == true);
        auto const parse_result = maybe_obj_result.value();
        CHECK(parse_result.unknown_tokens_cref() == 0);
        CHECK(parse_result.vertex_list_cref().size() == 4);

        /// --------------------------------------------------------------------
        /// vertex value checks
        CHECK(parse_result.vertex(1) == raytracer::create_point(-1.0, 1.0, 0.0));
        CHECK(parse_result.vertex(2) == raytracer::create_point(-1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(3) == raytracer::create_point(1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(4) == raytracer::create_point(1.0, 1.0, 0.0));

        /// --------------------------------------------------------------------
        /// bad vertex index check
        CHECK(parse_result.checked_vertex(0) == std::nullopt);
        CHECK(parse_result.checked_vertex(100) == std::nullopt);

        /// --------------------------------------------------------------------
        /// group geometry check
        auto group_shapes_cref = parse_result.default_group_cref()->child_shapes_cref();
        CHECK(group_shapes_cref.size() == 2);

        auto gs_1 = dynamic_cast<const RT::triangle*>(group_shapes_cref[0].get());
        CHECK(gs_1->p1() == parse_result.vertex(1));
        CHECK(gs_1->p2() == parse_result.vertex(2));
        CHECK(gs_1->p3() == parse_result.vertex(3));

        auto gs_2 = dynamic_cast<const RT::triangle*>(group_shapes_cref[1].get());
        CHECK(gs_2->p1() == parse_result.vertex(1));
        CHECK(gs_2->p2() == parse_result.vertex(3));
        CHECK(gs_2->p3() == parse_result.vertex(4));
}
