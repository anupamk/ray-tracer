/// c++ includes
#include <memory>
#include <optional>
#include <vector>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "primitives/intersection_record.hpp"
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

TEST_CASE("Intersecting ray+group doesn't test children if box is missed")
{
        auto g_1 = std::make_shared<RT::group>();
        auto s_1 = std::make_shared<RT::sphere>();

        g_1->add_child(s_1);

        auto new_ray  = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 1.0, 0.0));
        auto group_xs = new_ray.intersect(g_1);

        CHECK(group_xs.has_value() == false);
}

TEST_CASE("Intersecting ray+group tests children if box is hit")
{
        auto g_1 = std::make_shared<RT::group>();
        auto s_1 = std::make_shared<RT::sphere>();

        g_1->add_child(s_1);

        auto new_ray  = RT::ray_t(RT::create_point(0.0, 0.0, -5.0), RT::create_vector(0.0, 0.0, 1.0));
        auto group_xs = new_ray.intersect(g_1);

        CHECK(group_xs.has_value() == true);
}

TEST_CASE("partitioning a group's children")
{
        auto g_1 = std::make_shared<RT::group>();

        /// add sphere s_1
        auto s_1 = std::make_shared<RT::sphere>();
        s_1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(-2.0, 0.0, 0.0));
        g_1->add_child(s_1);

        /// add sphere s_2
        auto s_2 = std::make_shared<RT::sphere>();
        s_2->transform(RT::matrix_transformations_t::create_3d_translation_matrix(2.0, 0.0, 0.0));
        g_1->add_child(s_2);

        /// add sphere s_3
        auto s_3 = std::make_shared<RT::sphere>();
        g_1->add_child(s_3);

        /// partition children
        const auto& [left_shapes, right_shapes] = g_1->partition_children();

        /*
         * some checks
         **/
        CHECK(g_1->includes(s_3) == true);
        CHECK(g_1->includes(s_1) == false);
        CHECK(g_1->includes(s_2) == false);

        CHECK(left_shapes.size() == 1);
        CHECK(left_shapes[0] == s_1);

        CHECK(right_shapes.size() == 1);
        CHECK(right_shapes[0] == s_2);
}

TEST_CASE("creating a sub-group from a list of children")
{
        auto g_1 = std::make_shared<RT::group>();

        /// --------------------------------------------------------------------
        /// create spheres s_1 and s_2, and add them as a subgroup to 'g_1'
        auto s_1 = std::make_shared<RT::sphere>();
        auto s_2 = std::make_shared<RT::sphere>();

        std::vector<std::shared_ptr<RT::shape_interface>> shape_list;
        shape_list.push_back(s_1);
        shape_list.push_back(s_2);

        g_1->make_subgroup(shape_list);

        /// --------------------------------------------------------------------
        /// group-check
        auto g_1_child_shapes = g_1->child_shapes_cref();
        CHECK(g_1_child_shapes.size() == 1);

        /// --------------------------------------------------------------------
        /// sub-group check
        std::shared_ptr<RT::group> g_1_sg = std::dynamic_pointer_cast<RT::group>(g_1_child_shapes[0]);

        auto sg_child_shapes = g_1_sg->child_shapes_cref();
        CHECK(sg_child_shapes.size() == 2);
        CHECK(g_1_sg->includes(s_1) == true);
        CHECK(g_1_sg->includes(s_2) == true);
}

TEST_CASE("Subdividing a group partitions its children")
{
        auto s_1 = std::make_shared<RT::sphere>();
        s_1->transform(RT::matrix_transformations_t::create_3d_translation_matrix(-2.0, -2.0, 0.0));

        auto s_2 = std::make_shared<RT::sphere>();
        s_2->transform(RT::matrix_transformations_t::create_3d_translation_matrix(-2.0, 2.0, 0.0));

        auto s_3 = std::make_shared<RT::sphere>();
        s_3->transform(RT::matrix_transformations_t::create_3d_scaling_matrix(4.0, 4.0, 4.0));

        /// --------------------------------------------------------------------
        /// a group with '3' spheres.
        auto g_1 = std::make_shared<RT::group>();
        g_1->add_child(s_1);
        g_1->add_child(s_2);
        g_1->add_child(s_3);

        g_1->divide(1);

        /// --------------------------------------------------------------------
        /// group-check
        auto g_1_child_shapes = g_1->child_shapes_cref();
        CHECK(g_1_child_shapes.size() == 2);

        /// --------------------------------------------------------------------
        /// 'g_1_child_shapes[0]' is a sphere i.e. 's_3'
        std::shared_ptr<RT::sphere> g_1_s_3 = std::dynamic_pointer_cast<RT::sphere>(g_1_child_shapes[0]);
        CHECK(g_1_s_3 == s_3);

        /// --------------------------------------------------------------------
        /// 'g_1_child_shapes[1]' is a subgroup with '2' groups
        std::shared_ptr<RT::group> g_1_sg_1 = std::dynamic_pointer_cast<RT::group>(g_1_child_shapes[1]);
        auto sg_1_child_shapes              = g_1_sg_1->child_shapes_cref();
        CHECK(sg_1_child_shapes.size() == 2);

        /// --------------------------------------------------------------------
        /// 1st-subgroup contains 's_1'
        std::shared_ptr<RT::group> g_1_sg_sg_1 = std::dynamic_pointer_cast<RT::group>(sg_1_child_shapes[0]);
        auto sg_sg_1_child_shapes              = g_1_sg_sg_1->child_shapes_cref();
        CHECK(sg_sg_1_child_shapes.size() == 1);
        CHECK(g_1_sg_sg_1->includes(s_1) == true);

        /// --------------------------------------------------------------------
        /// 2nd-subgroup contains 's_2'
        std::shared_ptr<RT::group> g_1_sg_sg_2 = std::dynamic_pointer_cast<RT::group>(sg_1_child_shapes[1]);
        auto sg_sg_2_child_shapes              = g_1_sg_sg_2->child_shapes_cref();
        CHECK(sg_sg_2_child_shapes.size() == 1);
        CHECK(g_1_sg_sg_2->includes(s_2) == true);
}
