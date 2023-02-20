/// c++ includes
#include <iostream>
#include <memory>

/// 3rdparty testing library
#include "doctest/doctest.h"

/// our includes
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "shapes/group.hpp"
#include "shapes/sphere.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("creating a new group")
{
        auto const new_group        = std::make_shared<RT::group>();
        auto const grp_xform_matrix = new_group->transform();

        CHECK(grp_xform_matrix == RT::fsize_dense2d_matrix_t::create_identity_matrix(4));
        CHECK(new_group->is_empty() == true);
}

TEST_CASE("every shape has a parent attribute")
{
        auto const new_group = std::make_shared<RT::group>();
        CHECK(new_group->get_parent() == nullptr);
}

TEST_CASE("adding a child (shape) to a group")
{
        auto new_group = std::make_shared<RT::group>();

        auto new_sphere = std::make_shared<RT::sphere>();
        new_group->add_child(new_sphere);

        CHECK(new_group->is_empty() == false);
        CHECK(new_sphere->get_parent() == new_group);
        CHECK(new_group->includes(new_sphere) == true);
}

TEST_CASE("intersecting a ray with an empty group")
{
        auto new_group = std::make_shared<RT::group>();
        auto new_ray   = RT::ray_t(RT::create_point(0.0, 0.0, 0.0), RT::create_vector(0.0, 0.0, 1.0));
        auto group_xs  = new_ray.intersect(new_group);

        CHECK(group_xs.has_value() == false);
}

TEST_CASE("intersecting a non-empty group with a ray")
{
        auto new_group = std::make_shared<RT::group>();
        auto s1        = std::make_shared<RT::sphere>();

        auto s2 = std::make_shared<RT::sphere>();
        s2->transform(RT::matrix_transformations_t::create_3d_translation_matrix(0.0, 0.0, -3.0));

        auto s3 = std::make_shared<RT::sphere>();
        s3->transform(RT::matrix_transformations_t::create_3d_translation_matrix(5.0, 0.0, 0.0));

        new_group->add_child(s1);
        new_group->add_child(s2);
        new_group->add_child(s3);

        auto new_ray  = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
        auto group_xs = new_ray.intersect(new_group);

        CHECK(group_xs.has_value() == true);

        auto group_xs_value = group_xs.value();
        CHECK(group_xs_value.size() == 4);

        CHECK(group_xs_value[0].what_object() == s2);
        CHECK(group_xs_value[1].what_object() == s2);
        CHECK(group_xs_value[2].what_object() == s1);
        CHECK(group_xs_value[3].what_object() == s1);
}

TEST_CASE("intersecting a transformed group")
{
        auto new_group = std::make_shared<RT::group>();
        new_group->transform(RT::matrix_transformations_t::create_3d_scaling_matrix(2.0, 2.0, 2.0));

        auto s1 = std::make_shared<RT::sphere>();
        s1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(5.0, 0.0, 0.0));
        new_group->add_child(s1);

        auto r1 = RT::ray_t(RT::create_point(10.0, 0.0, -10.0), RT::create_vector(0.0, 0.0, 1.0));

        auto group_xs = r1.intersect(new_group);
        CHECK(group_xs.has_value() == true);
        CHECK(group_xs.value().size() == 2);
}

TEST_CASE("converting a point from world to object space")
{
        auto g_1 = std::make_shared<RT::group>();
        g_1->transform(RT::matrix_transformations_t::create_roty_matrix(raytracer::PI_BY_2F));

        auto g_2 = std::make_shared<RT::group>();
        g_2->transform(RT::matrix_transformations_t::create_3d_scaling_matrix(2.0, 2.0, 2.0));

        g_1->add_child(g_2);

        auto g_2_s_1 = std::make_shared<RT::sphere>();
        g_2_s_1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(5.0, 0.0, 0.0));
        g_2->add_child(g_2_s_1);

        auto got_pt = g_2_s_1->world_to_local(RT::create_point(-2.0, 0.0, -10.0));
        auto exp_pt = RT::create_point(0.0, 0.0, -1.0);

        CHECK(got_pt == exp_pt);
}

TEST_CASE("converting a normal from object to world space")
{
        auto g_1 = std::make_shared<RT::group>();
        g_1->transform(RT::matrix_transformations_t::create_roty_matrix(RT::PI_BY_2F));

        auto g_2 = std::make_shared<RT::group>();
        g_2->transform(RT::matrix_transformations_t::create_3d_scaling_matrix(1.0, 2.0, 3.0));

        g_1->add_child(g_2);

        auto g_2_s_1 = std::make_shared<RT::sphere>();
        g_2_s_1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(5.0, 0.0, 0.0));
        g_2->add_child(g_2_s_1);

        auto pt_xyz     = RT::SQRT_3_BY_3F;
        auto got_normal = g_2_s_1->normal_at_world(RT::create_vector(pt_xyz, pt_xyz, pt_xyz));
        auto exp_normal = RT::create_vector(0.285714, 0.428571, -0.857143);

        CHECK(got_normal == exp_normal);
}

TEST_CASE("finding normal on a child object in a group")
{
        auto g_1 = std::make_shared<RT::group>();
        g_1->transform(RT::matrix_transformations_t::create_roty_matrix(RT::PI_BY_2F));

        auto g_2 = std::make_shared<RT::group>();
        g_2->transform(RT::matrix_transformations_t::create_3d_scaling_matrix(1.0, 2.0, 3.0));

        g_1->add_child(g_2);

        auto g_2_s_1 = std::make_shared<RT::sphere>();
        g_2_s_1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(5.0, 0.0, 0.0));
        g_2->add_child(g_2_s_1);

        auto pt_xyz     = RT::SQRT_3_BY_3F;
        auto got_normal = g_2_s_1->normal_at(RT::create_point(1.7321, 1.1547, -5.5774));
        auto exp_normal = RT::create_vector(0.285704, 0.428543, -0.857161);

        CHECK(got_normal == exp_normal);
}
