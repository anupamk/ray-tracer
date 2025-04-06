/// c++ includes
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "io/obj_file_loader.hpp"
#include "io/obj_parse_result.hpp"
#include "platform_utils/mmapped_file_reader.hpp"
#include "primitives/tuple.hpp"
#include "shapes/group.hpp"
#include "shapes/shape_interface.hpp"
#include "shapes/triangle.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// a convenience alias
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// bunch of random tokens
TEST_CASE("obj_file_loader::parse(...) trivial")
{
        std::string_view obj_data = R"""(mary  ha d       a little
lamb with fleece as white as snow
and # some comment to spice things up
      wherever
   mary		went
the lamb \
	was sure to


         go...


)""";

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();
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

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
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

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
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

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex and polygon data
TEST_CASE("obj file with vertex and polygon data")
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

# v[5]
v 0 2 0

# 3 faces as follows:
#    f[0] : using v[1], v[2], v[3]
#    f[1] : using v[1], v[3], v[4]
#    f[2] : using v[1], v[4], v[5]
f 1 2 3 4 5
)""";

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
        CHECK(parse_result.unknown_tokens_cref() == 0);
        CHECK(parse_result.vertex_list_cref().size() == 5);

        /// --------------------------------------------------------------------
        /// vertex value checks
        CHECK(parse_result.vertex(1) == raytracer::create_point(-1.0, 1.0, 0.0));
        CHECK(parse_result.vertex(2) == raytracer::create_point(-1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(3) == raytracer::create_point(1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(4) == raytracer::create_point(1.0, 1.0, 0.0));
        CHECK(parse_result.vertex(5) == raytracer::create_point(0.0, 2.0, 0.0));

        /// --------------------------------------------------------------------
        /// bad vertex index check
        CHECK(parse_result.checked_vertex(0) == std::nullopt);
        CHECK(parse_result.checked_vertex(100) == std::nullopt);

        /// --------------------------------------------------------------------
        /// group geometry check
        auto group_shapes_cref = parse_result.default_group_cref()->child_shapes_cref();
        CHECK(group_shapes_cref.size() == 3);

        auto gs_1 = dynamic_cast<const RT::triangle*>(group_shapes_cref[0].get());
        CHECK(gs_1->p1() == parse_result.vertex(1));
        CHECK(gs_1->p2() == parse_result.vertex(2));
        CHECK(gs_1->p3() == parse_result.vertex(3));

        auto gs_2 = dynamic_cast<const RT::triangle*>(group_shapes_cref[1].get());
        CHECK(gs_2->p1() == parse_result.vertex(1));
        CHECK(gs_2->p2() == parse_result.vertex(3));
        CHECK(gs_2->p3() == parse_result.vertex(4));

        auto gs_3 = dynamic_cast<const RT::triangle*>(group_shapes_cref[2].get());
        CHECK(gs_3->p1() == parse_result.vertex(1));
        CHECK(gs_3->p2() == parse_result.vertex(4));
        CHECK(gs_3->p3() == parse_result.vertex(5));
}

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex and polygon data
TEST_CASE("obj file with vertex and polygon data")
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

# first group
g FirstGroup
f 1 2 3

# second group
g SecondGroup
f 1 3 4
)""";

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
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
        auto group_list = parse_result.group_list_cref();
        CHECK(group_list.size() == 2);

        /// group-1
        auto group_1_shapes = group_list[0]->child_shapes_cref();
        CHECK(group_1_shapes.size() == 1);

        auto group_1_shape_1 = dynamic_cast<const RT::triangle*>(group_1_shapes[0].get());
        CHECK(group_1_shape_1->p1() == parse_result.vertex(1));
        CHECK(group_1_shape_1->p2() == parse_result.vertex(2));
        CHECK(group_1_shape_1->p3() == parse_result.vertex(3));

        /// group-2
        auto group_2_shapes = group_list[1]->child_shapes_cref();
        CHECK(group_2_shapes.size() == 1);

        auto group_2_shape_1 = dynamic_cast<const RT::triangle*>(group_2_shapes[0].get());
        CHECK(group_2_shape_1->p1() == parse_result.vertex(1));
        CHECK(group_2_shape_1->p2() == parse_result.vertex(3));
        CHECK(group_2_shape_1->p3() == parse_result.vertex(4));
}

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex data
TEST_CASE("obj file with vertex-normal data")
{
        std::string_view obj_data = R"""(# obj file with vertex data
# vn[1]
vn 0 0 1

# vn[2] vertex data with continuation and a useless comment
# as well...
vn \
# this is a useless comment
0.707                                          \
	0.0 \
-0.707

# vn[3]
vn 1 2 3

# vn[4]
vn 1 1 0
)""";

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
        CHECK(parse_result.unknown_tokens_cref() == 0);
        CHECK(parse_result.vertex_normal_list_cref().size() == 4);

        /// --------------------------------------------------------------------
        /// vertex-normal value checks
        CHECK(parse_result.vertex_normal(1) == raytracer::create_vector(0.0, 0.0, 1.0));
        CHECK(parse_result.vertex_normal(2) == raytracer::create_vector(0.707, 0.0, -0.707));
        CHECK(parse_result.vertex_normal(3) == raytracer::create_vector(1.0, 2.0, 3.0));
        CHECK(parse_result.vertex_normal(4) == raytracer::create_vector(1.0, 1.0, 0.0));

        /// --------------------------------------------------------------------
        /// bad vertex-normal index check
        CHECK(parse_result.checked_vertex_normal(0) == std::nullopt);
        CHECK(parse_result.checked_vertex_normal(100) == std::nullopt);
}

/// ----------------------------------------------------------------------------
/// an OBJ file with vertex data
TEST_CASE("obj file with vertex, vertex-normal and face data")
{
        std::string_view obj_data = R"""(# obj file with vertex, vertex-normal and face
# vertex data
# v[1]
v 0 1 0

# v[2]
v -1 0 0

# v[3]
v 1 0 0

# vertex-normal data
# vn[1]
vn -1 0 0

# vn[2]
vn 1 0 0

# vn[3]
vn 0 1 0

# face with vertex normal data
# f[1]
f 1//3 2//1 3//2

# f[2]
f 1/0/3 2/102/1 3/14/2
)""";

        std::string fname = platform_utils::fill_file_with_data(obj_data);

        RT::obj_file_parser p1(fname);
        auto parse_result = p1.parse();

        /// --------------------------------------------------------------------
        /// rudimentary checks
        CHECK(parse_result.unknown_tokens_cref() == 0);
        CHECK(parse_result.vertex_list_cref().size() == 3);
        CHECK(parse_result.vertex_normal_list_cref().size() == 3);

        /// --------------------------------------------------------------------
        /// vertex value checks
        CHECK(parse_result.vertex(1) == raytracer::create_point(0.0, 1.0, 0.0));
        CHECK(parse_result.vertex(2) == raytracer::create_point(-1.0, 0.0, 0.0));
        CHECK(parse_result.vertex(3) == raytracer::create_point(1.0, 0.0, 0.0));
        CHECK(parse_result.checked_vertex(0) == std::nullopt);
        CHECK(parse_result.checked_vertex(100) == std::nullopt);

        /// --------------------------------------------------------------------
        /// vertex-normal value checks
        CHECK(parse_result.vertex_normal(1) == raytracer::create_vector(-1.0, 0.0, 0.0));
        CHECK(parse_result.vertex_normal(2) == raytracer::create_vector(1.0, 0.0, 0.0));
        CHECK(parse_result.vertex_normal(3) == raytracer::create_vector(0.0, 1.0, 0.0));
        CHECK(parse_result.checked_vertex_normal(0) == std::nullopt);
        CHECK(parse_result.checked_vertex_normal(100) == std::nullopt);

        /// --------------------------------------------------------------------
        /// geometry checks
        auto group_shapes_cref = parse_result.default_group_cref()->child_shapes_cref();
        CHECK(group_shapes_cref.size() == 2);

        auto gs_1 = dynamic_cast<const RT::triangle*>(group_shapes_cref[0].get());
        CHECK(gs_1->p1() == parse_result.vertex(1));
        CHECK(gs_1->p2() == parse_result.vertex(2));
        CHECK(gs_1->p3() == parse_result.vertex(3));
        CHECK(gs_1->n1() == parse_result.vertex_normal(3));
        CHECK(gs_1->n2() == parse_result.vertex_normal(1));
        CHECK(gs_1->n3() == parse_result.vertex_normal(2));

        auto gs_2 = dynamic_cast<const RT::triangle*>(group_shapes_cref[1].get());
        CHECK(gs_1->p1() == parse_result.vertex(1));
        CHECK(gs_1->p2() == parse_result.vertex(2));
        CHECK(gs_1->p3() == parse_result.vertex(3));
        CHECK(gs_1->n1() == parse_result.vertex_normal(3));
        CHECK(gs_1->n2() == parse_result.vertex_normal(1));
        CHECK(gs_1->n3() == parse_result.vertex_normal(2));

        CHECK(*gs_1 == *gs_2);
}
