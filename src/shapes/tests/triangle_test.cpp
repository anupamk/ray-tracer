/// c++ includes
#include <memory>
#include <optional>
#include <vector>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "primitives/intersection_info.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/triangle.hpp"
#include "utils/utils.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("constructing a triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);

        CHECK(t->p1() == pt_1);
        CHECK(t->p2() == pt_2);
        CHECK(t->p3() == pt_3);
        CHECK(t->e1() == RT::create_vector(-1.0, -1.0, 0.0));
        CHECK(t->e2() == RT::create_vector(1.0, -1.0, 0.0));
        CHECK(t->n1() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(t->n2() == RT::create_vector(0.0, 0.0, -1.0));
        CHECK(t->n3() == RT::create_vector(0.0, 0.0, -1.0));
}

TEST_CASE("finding normal on a triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);

        auto const n1 = t->normal_at(RT::create_point(0.0, 0.5, 0.0));
        auto const n2 = t->normal_at(RT::create_point(-0.5, 0.75, 0.0));
        auto const n3 = t->normal_at(RT::create_point(0.5, 0.25, 0.0));

        CHECK(n1 == t->n1());
        CHECK(n2 == t->n1());
        CHECK(n3 == t->n1());
}

TEST_CASE("intersecting a ray parallel to the triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);
        auto const r = RT::ray_t(RT::create_point(0.0, -1.0, -2.0), RT::create_vector(0.0, 1.0, 0.0));

        auto const xs_list = r.intersect(t);
        CHECK(xs_list.has_value() == false);
}

TEST_CASE("ray misses the p1-p3 edge")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);
        auto const r = RT::ray_t(RT::create_point(1.0, 1.0, -2.0), RT::create_vector(0.0, 0.0, 1.0));

        auto const xs_list = r.intersect(t);
        CHECK(xs_list.has_value() == false);
}

TEST_CASE("ray misses the p1-p2 edge")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);
        auto const r = RT::ray_t(RT::create_point(-1.0, 1.0, -2.0), RT::create_vector(0.0, 0.0, 1.0));

        auto const xs_list = r.intersect(t);
        CHECK(xs_list.has_value() == false);
}

TEST_CASE("ray misses the p2-p3 edge")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);
        auto const r = RT::ray_t(RT::create_point(0.0, -1.0, -2.0), RT::create_vector(0.0, 0.0, 1.0));

        auto const xs_list = r.intersect(t);
        CHECK(xs_list.has_value() == false);
}

TEST_CASE("ray strikes the triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);

        auto const t = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3);
        auto const r = RT::ray_t(RT::create_point(0.0, 0.5, -2.0), RT::create_vector(0.0, 0.0, 1.0));

        auto const xs_list = r.intersect(t);
        CHECK(xs_list.has_value() == true);

        auto const xs_value = xs_list.value();
        CHECK(xs_value.size() == 1);
        CHECK(xs_value[0].where() == 2.0);
}

TEST_CASE("constructing a smooth triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);
        auto n1   = RT::create_vector(0.0, 1.0, 0.0);
        auto n2   = RT::create_vector(-1.0, 0.0, 0.0);
        auto n3   = RT::create_vector(1.0, 0.0, 0.0);

        auto tri = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3, n1, n2, n3);
        CHECK(tri->p1() == pt_1);
        CHECK(tri->p2() == pt_2);
        CHECK(tri->p3() == pt_3);
        CHECK(tri->n1() == n1);
        CHECK(tri->n2() == n2);
        CHECK(tri->n3() == n3);
}

TEST_CASE("an intersection with smooth triangle stores u/v")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);
        auto n1   = RT::create_vector(0.0, 1.0, 0.0);
        auto n2   = RT::create_vector(-1.0, 0.0, 0.0);
        auto n3   = RT::create_vector(1.0, 0.0, 0.0);

        auto tri = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3, n1, n2, n3);
        auto r   = RT::ray_t(RT::create_point(-0.2, 0.3, -2.0), RT::create_vector(0.0, 0.0, 1.0));

        auto xs_list = r.intersect(tri);
        CHECK(xs_list.has_value() == true);

        auto xs_value = xs_list.value();
        CHECK(xs_value.size() == 1);

        CHECK(RT::epsilon_equal(xs_value[0].u(), 0.45));
        CHECK(RT::epsilon_equal(xs_value[0].v(), 0.25));
}

TEST_CASE("an intersection with smooth triangle stores u/v")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);
        auto n1   = RT::create_vector(0.0, 1.0, 0.0);
        auto n2   = RT::create_vector(-1.0, 0.0, 0.0);
        auto n3   = RT::create_vector(1.0, 0.0, 0.0);

        auto tri       = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3, n1, n2, n3);
        auto xs_record = RT::intersection_record(1.0, tri, 0.45, 0.25);
        auto n         = tri->normal_at(RT::create_point(0.0, 0.0, 0.0), xs_record);

        CHECK(n == RT::create_vector(-0.5547, 0.83205, 0.0));
}

TEST_CASE("preparing the normal on a smooth triangle")
{
        auto pt_1 = RT::create_point(0.0, 1.0, 0.0);
        auto pt_2 = RT::create_point(-1.0, 0.0, 0.0);
        auto pt_3 = RT::create_point(1.0, 0.0, 0.0);
        auto n1   = RT::create_vector(0.0, 1.0, 0.0);
        auto n2   = RT::create_vector(-1.0, 0.0, 0.0);
        auto n3   = RT::create_vector(1.0, 0.0, 0.0);
        auto tri  = std::make_shared<RT::triangle>(pt_1, pt_2, pt_3, n1, n2, n3);

        auto r       = RT::ray_t(RT::create_point(-0.2, 0.3, -2.0), RT::create_vector(0.0, 0.0, 1.0));
        auto xs_01   = RT::intersection_record(1.0, tri, 0.45, 0.25);
        auto xs_list = RT::intersection_records{xs_01};
        auto comps   = r.prepare_computations(xs_list);

        CHECK(comps.normal_vector() == RT::create_vector(-0.5547, 0.83205, 0.0));
}
