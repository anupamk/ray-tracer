/// c++ includes
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest.h"

/// our includes
#include "constants.hpp"
#include "intersection_record.hpp"
#include "material.hpp"
#include "matrix.hpp"
#include "matrix_transformations.hpp"
#include "plane.hpp"
#include "ray.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "tuple.hpp"
#include "world.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// ----------------------------------------------------------------------------
/// define a trivial test pattern
class test_pattern final : public RT::pattern_interface
{
    public:
        RT::color color_at_point(RT::tuple const& pt) const override
        {
                return RT::color(pt.x(), pt.y(), pt.z());
        }
};

/// ----------------------------------------------------------------------------
/// ensure that default material has no reflectivity
TEST_CASE("scenario: reflectivity attribute for default material")
{
        auto m = RT::material();
        CHECK(m.get_reflective() == 0.0);
}

/// ----------------------------------------------------------------------------
/// ensure that 'ray::prepare_computations(...)' pre-computes the reflectv
/// vector (correctly !)
TEST_CASE("scenario: precompute reflectv vector in ray::prepare_computations(...)")
{
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 1.0, -1.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// create the plane and setup the intersection
        auto const the_xz_plane = std::make_shared<RT::plane>();
        auto const xs_01        = RT::intersection_record(RT::SQRT_2, the_xz_plane);
        auto const xs_list      = RT::intersection_records{xs_01};

        /// get information about the intersection
        auto const xs_info = the_ray.prepare_computations(xs_list);

        /// hopefully it all works out.
        auto const expected_reflectv = RT::create_vector(0.0, RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        CHECK(xs_info.reflection_vector() == expected_reflectv);
}

/// ----------------------------------------------------------------------------
/// ensure correct color due to reflection from non-reflective surface
TEST_CASE("scenario: ensure correct color due to reflection from non-reflective surface")
{
        auto W = RT::world::create_default_world();

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, 0.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// adjust ambient property for 2nd shape in the world
        auto shape_02          = std::const_pointer_cast<RT::shape_interface>(W.shapes()[1]);
        auto shape_02_material = shape_02->get_material();
        shape_02_material.set_ambient(1.0);
        shape_02->set_material(shape_02_material);

        /// setup the intersection and get information about intersections
        auto const xs_01   = RT::intersection_record(1.0, shape_02);
        auto const xs_list = RT::intersection_records{xs_01};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        /// lets check the color that we get
        auto const got_rc      = W.reflected_color(xs_info);
        auto const expected_rc = RT::color_black();

        CHECK(got_rc == expected_rc);
}

/// ----------------------------------------------------------------------------
/// ensure correct color due to reflection from reflective surface
TEST_CASE("scenario: ensure correct color due to reflection from reflective surface")
{
        auto W = RT::world::create_default_world();

        /// create a plane and add it to the world
        auto xz_plane     = std::make_shared<RT::plane>();
        auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
        xz_plane->set_material(RT::material().set_reflective(0.5));
        xz_plane->transform(xlate_matrix);

        W.add(xz_plane);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -3.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// setup the intersection
        auto const xs_01   = RT::intersection_record(RT::SQRT_2, xz_plane);
        auto const xs_list = RT::intersection_records{xs_01};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        /// lets check the color we got
        auto const got_rc      = W.reflected_color(xs_info);
        auto const expected_rc = RT::color(0.190332, 0.237915, 0.142749);

        CHECK(got_rc == expected_rc);
}

/// ----------------------------------------------------------------------------
/// ensure that reflected-color is incorporated into final color
TEST_CASE("scenario: ensure that reflected-color is incorporated into final color")
{
        using RT_XFORM = RT::matrix_transformations_t;

        auto W = RT::world::create_default_world();

        /// create a plane and add it to the world
        auto xz_plane     = std::make_shared<RT::plane>();
        auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
        xz_plane->set_material(RT::material().set_reflective(0.5));
        xz_plane->transform(xlate_matrix);

        W.add(xz_plane);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -3.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// setup the intersection
        auto const xs_01   = RT::intersection_record(RT::SQRT_2, xz_plane);
        auto const xs_list = RT::intersection_records{xs_01};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        /// lets check the color we got
        auto const got_rc      = W.shade_hit(xs_info);
        auto const expected_rc = RT::color(0.876757, 0.92434, 0.829174);

        CHECK(got_rc == expected_rc);
}

/// ----------------------------------------------------------------------------
/// ensure that there is no infinite recursion for rays reflected between
/// parallel surfaces
TEST_CASE("scenario: ensure no infinite recursion between parallel reflected surfaces")
{
        using RT_XFORM = RT::matrix_transformations_t;

        auto create_reflective_plane_at = [](RT::tuple where) -> std::shared_ptr<RT::shape_interface> {
                auto p   = std::make_shared<RT::plane>();
                auto mat = RT::material().set_reflective(1.0);
                p->set_material(mat);

                auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(where.x(), where.y(), where.z());
                p->transform(xlate_matrix);

                return p;
        };

        /// create a world, with 2 parallel planes with reflective surfaces
        auto W           = RT::world();
        auto upper_plane = create_reflective_plane_at(RT::create_point(0.0, -1.0, 0.0));
        auto lower_plane = create_reflective_plane_at(RT::create_point(0.0, 1.0, 0.0));

        W.add(upper_plane);
        W.add(lower_plane);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, 0.0);
        auto const ray_direction = RT::create_vector(0.0, 1.0, 0.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// compute color due to ray
        auto const got_rc      = W.color_at(the_ray);
        auto const expected_rc = RT::color_black();

        CHECK(got_rc == expected_rc);
}

/// ----------------------------------------------------------------------------
/// ensure that reflected-color at limits of recursive threshold is black
TEST_CASE("scenario: ensure that reflected-color at limits of recursive threshold is black")
{
        using RT_XFORM = RT::matrix_transformations_t;

        auto W = RT::world::create_default_world();

        /// create a plane and add it to the world
        auto xz_plane     = std::make_shared<RT::plane>();
        auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
        xz_plane->set_material(RT::material().set_reflective(0.5));
        xz_plane->transform(xlate_matrix);

        W.add(xz_plane);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -3.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// setup the intersection
        auto const xs_01   = RT::intersection_record(RT::SQRT_2, xz_plane);
        auto const xs_list = RT::intersection_records{xs_01};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        /// lets check the color we got
        auto const got_rc      = W.reflected_color(xs_info, 0);
        auto const expected_rc = RT::color_black();

        CHECK(got_rc == expected_rc);
}

/// ----------------------------------------------------------------------------
/// ensure default material refractive-index + transparency properties are ok.
TEST_CASE("scenario: refractive-index + transparency attribute for default material")
{
        auto m = RT::material();
        CHECK(m.get_refractive_index() == RT::material::RI_VACCUM);
        CHECK(m.get_transparency() == 0.0);
}

/// ----------------------------------------------------------------------------
/// ensure glassy sphere properties
TEST_CASE("scenario: ensure glassy sphere has correct properties")
{
        auto gs          = RT::glass_sphere();
        auto gs_material = gs->get_material();

        CHECK(gs_material.get_transparency() == 1.0);
        CHECK(RT::epsilon_equal(gs_material.get_refractive_index(), RT::material::RI_GLASS));
        CHECK(gs->transform() == RT::fsize_dense2d_matrix_t::create_identity_matrix(4));
}

/// ----------------------------------------------------------------------------
/// finding n1 and n2 at various intersection points
TEST_CASE("scenario: find n1 and n2 values at various intersections")
{
        /// --------------------------------------------------------------------
        /// glass-sphere-01
        auto gs_01       = RT::glass_sphere();
        auto gs_01_xform = RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0);
        gs_01->set_material(RT::material().set_refractive_index(1.5));
        gs_01->transform(gs_01_xform);

        /// --------------------------------------------------------------------
        /// glass-sphere-02
        auto gs_02       = RT::glass_sphere();
        auto gs_02_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, -0.25);
        gs_02->set_material(RT::material().set_refractive_index(2.0));
        gs_02->transform(gs_02_xform);

        /// --------------------------------------------------------------------
        /// glass-sphere-03
        auto gs_03       = RT::glass_sphere();
        auto gs_03_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 0.25);
        gs_03->set_material(RT::material().set_refractive_index(2.5));
        gs_03->transform(gs_03_xform);

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -4.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// create a bunch of intersections i.e. intersection-records
        RT::intersection_records xs_list = {
                {2.0, gs_01}, {2.75, gs_02}, {3.25, gs_03}, {4.75, gs_02}, {5.25, gs_03}, {6.0, gs_01},
        };

        struct {
                size_t index;
                double exp_n1;
                double exp_n2;
        } const all_tc[] = {
                {0, 1.0, 1.5}, /// 0
                {1, 1.5, 2.0}, /// 1
                {2, 2.0, 2.5}, /// 2
                {3, 2.5, 2.5}, /// 3
                {4, 2.5, 1.5}, /// 4
                {5, 1.5, 1.0}, /// 5
        };

        for (auto tc : all_tc) {
                auto xs_info = the_ray.prepare_computations(xs_list, tc.index);
                CHECK(xs_info.n1() == tc.exp_n1);
                CHECK(xs_info.n2() == tc.exp_n2);
        }
}

/// ----------------------------------------------------------------------------
/// the under-point is offset below the surface
TEST_CASE("scenario: ensure that under-point is offset below the surface")
{
        auto gs_01       = RT::glass_sphere();
        auto gs_01_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 1.0);
        gs_01->transform(gs_01_xform);

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -5.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// create a intersection list, of 1 record and find intersection info
        RT::intersection_records xs_list = {{5.0, gs_01}};

        auto xs_info      = the_ray.prepare_computations(xs_list);
        auto xs_under_pos = xs_info.under_position();

        /// --------------------------------------------------------------------
        /// half-epsilon to ensure that adjustment is in correct direction
        CHECK(xs_under_pos.z() > RT::EPSILON_BY_2F);
        CHECK(xs_info.position().z() < xs_under_pos.z());
}

/// ----------------------------------------------------------------------------
/// find refracted color of an opaque object
TEST_CASE("scenario: find refracted color of an opaque object")
{
        /// first shape in a default world
        auto W              = RT::world::create_default_world();
        auto world_shape_01 = W.shapes()[0];

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -5.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// intersections
        auto xs_list = RT::intersection_records{
                {4.0, world_shape_01}, /// intersection-01
                {6.0, world_shape_01}, /// intersection-02
        };

        auto xs_info             = the_ray.prepare_computations(xs_list);
        auto got_refracted_color = W.refracted_color(xs_info);
        auto exp_refracted_color = RT::color_black();

        CHECK(got_refracted_color == exp_refracted_color);
}

/// ----------------------------------------------------------------------------
/// refracted color at maximum recursion depth
TEST_CASE("scenario: check color at maximum recursion depth")
{
        auto W = RT::world::create_default_world();

        /// set properties for first shape in a default world
        auto world_shape_01    = std::const_pointer_cast<RT::shape_interface>(W.shapes()[0]);
        auto shape_01_material = world_shape_01->get_material();
        shape_01_material.set_transparency(1.0).set_refractive_index(1.5);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -5.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// intersections
        auto xs_list = RT::intersection_records{
                {4.0, world_shape_01}, /// intersection-01
                {6.0, world_shape_01}, /// intersection-02
        };

        auto xs_info             = the_ray.prepare_computations(xs_list);
        auto got_refracted_color = W.refracted_color(xs_info, 0);
        auto exp_refracted_color = RT::color_black();

        CHECK(got_refracted_color == exp_refracted_color);
}

/// ----------------------------------------------------------------------------
/// refracted color under total internal reflection
TEST_CASE("scenario: refracted color under total internal reflection")
{
        auto W = RT::world::create_default_world();

        /// set properties for first shape in a default world
        auto world_shape_01    = std::const_pointer_cast<RT::shape_interface>(W.shapes()[0]);
        auto shape_01_material = world_shape_01->get_material();
        shape_01_material.set_transparency(1.0).set_refractive_index(1.5);

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, RT::SQRT_2_BY_2F);
        auto const ray_direction = RT::create_vector(0.0, 1.0, 0.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// intersections
        auto xs_list = RT::intersection_records{
                {-RT::SQRT_2_BY_2F, world_shape_01}, /// intersection-01
                {RT::SQRT_2_BY_2F, world_shape_01},  /// intersection-02
        };

        /// --------------------------------------------------------------------
        /// we are inside the sphere, so need to look at 2nd intersection
        auto xs_info             = the_ray.prepare_computations(xs_list, 1);
        auto got_refracted_color = W.refracted_color(xs_info, 5);
        auto exp_refracted_color = RT::color_black();

        CHECK(got_refracted_color == exp_refracted_color);
}

/// ----------------------------------------------------------------------------
/// refracted color for a refracted ray
TEST_CASE("scenario: refracted color for a refracted ray")
{
        auto W = RT::world::create_default_world();

        /// set properties for first shape in a default world
        auto world_shape_01 = std::const_pointer_cast<RT::shape_interface>(W.shapes()[0]);
        {
                auto shape_01_mat = RT::material()            /// default-material
                                            .set_ambient(1.0) /// with specific
                                            .set_pattern(std::make_shared<test_pattern>()); /// properties

                world_shape_01->set_material(shape_01_mat);
        }

        /// set properties for second shape in a default world
        auto world_shape_02 = std::const_pointer_cast<RT::shape_interface>(W.shapes()[1]);
        {
                auto shape_02_mat = RT::material()                      /// default material
                                            .set_transparency(1.0)      /// with specific
                                            .set_refractive_index(1.5); /// properties

                world_shape_02->set_material(shape_02_mat);
        }

        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, 0.1);
        auto const ray_direction = RT::create_vector(0.0, 1.0, 0.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// intersections
        auto xs_list = RT::intersection_records{
                {-0.9899, world_shape_01}, /// intersection-01
                {-0.4899, world_shape_02}, /// intersection-02
                {0.4899, world_shape_02},  /// intersection-03
                {0.9899, world_shape_01},  /// intersection-04
        };

        auto xs_info             = the_ray.prepare_computations(xs_list, 2);
        auto got_refracted_color = W.refracted_color(xs_info, 5);
        auto exp_refracted_color = RT::color(0.0, 0.998875, 0.047219);

        CHECK(got_refracted_color == exp_refracted_color);
}

/// ----------------------------------------------------------------------------
/// ensure that world.shade_hit(...) uses refracted color as well
TEST_CASE("scenario: handle refracted color in world::shade_hit(...)")
{
        auto W = RT::world::create_default_world();

        /// --------------------------------------------------------------------
        /// create a glassy floor
        auto const floor = std::make_shared<RT::plane>();
        {
                auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
                floor->transform(xlate_matrix);

                auto floor_material = RT::material().set_transparency(0.5).set_refractive_index(1.5);
                floor->set_material(floor_material);
        }
        W.add(floor);

        /// --------------------------------------------------------------------
        /// colored ball below the floor
        auto const ball = std::make_shared<RT::sphere>();
        {
                auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -3.5, -0.5);
                ball->transform(xlate_matrix);

                auto ball_color = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.0, 0.0));
                auto ball_mat   = RT::material().set_pattern(ball_color).set_ambient(0.5);
                ball->set_material(ball_mat);
        }
        W.add(ball);

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -3.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// and specify the intersections and compute the color
        auto const xs_list = RT::intersection_records{{RT::SQRT_2, floor}};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        auto const got_color      = W.shade_hit(xs_info, 5);
        auto const expected_color = RT::color(0.93642, 0.68642, 0.68642);
        CHECK(got_color == expected_color);
}

/// ----------------------------------------------------------------------------
/// determine reflectance under total-internal-reflection
TEST_CASE("scenario: Schlick Approximation under total internal reflection")
{
        auto gs = RT::glass_sphere();

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, RT::SQRT_2_BY_2F);
        auto const ray_direction = RT::create_vector(0.0, 1.0, 0.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// create a bunch of intersections i.e. intersection-records
        RT::intersection_records xs_list = {
                {-RT::SQRT_2_BY_2F, gs},
                {RT::SQRT_2_BY_2F, gs},
        };

        auto xs_info              = the_ray.prepare_computations(xs_list, 1);
        auto got_reflectance      = xs_info.schlick_approx();
        auto expected_reflectance = 1.0;

        CHECK(got_reflectance == expected_reflectance);
}

/// ----------------------------------------------------------------------------
/// determine reflectance for a perpendicular ray
TEST_CASE("scenario: Schlick Approximation for a perpendicular ray")
{
        auto gs = RT::glass_sphere();

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, 0.0);
        auto const ray_direction = RT::create_vector(0.0, 1.0, 0.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// create a bunch of intersections i.e. intersection-records
        RT::intersection_records xs_list = {
                {-1.0, gs},
                {1.0, gs},
        };

        auto xs_info              = the_ray.prepare_computations(xs_list, 1);
        auto got_reflectance      = xs_info.schlick_approx();
        auto expected_reflectance = 0.04;

        CHECK(RT::epsilon_equal(got_reflectance, expected_reflectance));
}

/// ----------------------------------------------------------------------------
/// determine reflectance when n2 > n1
TEST_CASE("scenario: determine reflectance when n2 > n1")
{
        auto gs = RT::glass_sphere();

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.99, -2.0);
        auto const ray_direction = RT::create_vector(0.0, 0.0, 1.0);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// create a bunch of intersections i.e. intersection-records
        RT::intersection_records xs_list = {
                {1.8589, gs},
        };

        auto xs_info              = the_ray.prepare_computations(xs_list);
        auto got_reflectance      = xs_info.schlick_approx();
        auto expected_reflectance = 0.48873;

        std::cout << "got_reflectance: " << got_reflectance << ", "
                  << "expected_reflectance: " << expected_reflectance << std::endl;

        CHECK(RT::epsilon_equal(got_reflectance, expected_reflectance));
}

/// ----------------------------------------------------------------------------
/// ensure that world.shade_hit(...) uses refracted color as well
TEST_CASE("scenario: handle refracted color in world::shade_hit(...)")
{
        auto W = RT::world::create_default_world();

        /// --------------------------------------------------------------------
        /// create a glassy floor
        auto const floor = std::make_shared<RT::plane>();
        {
                auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
                floor->transform(xlate_matrix);

                auto floor_material =
                        RT::material().set_transparency(0.5).set_refractive_index(1.5).set_reflective(0.5);

                floor->set_material(floor_material);
        }
        W.add(floor);

        /// --------------------------------------------------------------------
        /// colored ball below the floor
        auto const ball = std::make_shared<RT::sphere>();
        {
                auto xlate_matrix = RT_XFORM::create_3d_translation_matrix(0.0, -3.5, -0.5);
                ball->transform(xlate_matrix);

                auto ball_color = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.0, 0.0));
                auto ball_mat   = RT::material().set_pattern(ball_color).set_ambient(0.5);
                ball->set_material(ball_mat);
        }
        W.add(ball);

        /// --------------------------------------------------------------------
        /// create a ray
        auto const ray_origin    = RT::create_point(0.0, 0.0, -3.0);
        auto const ray_direction = RT::create_vector(0.0, -RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F);
        auto const the_ray       = RT::ray_t(ray_origin, ray_direction);

        /// --------------------------------------------------------------------
        /// and specify the intersections and compute the color
        auto const xs_list = RT::intersection_records{{RT::SQRT_2, floor}};
        auto const xs_info = the_ray.prepare_computations(xs_list);

        auto const got_color      = W.shade_hit(xs_info, 5);
        auto const expected_color = RT::color(0.93391, 0.69643, 0.69243);

        CHECK(got_color == expected_color);
}
