/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include "patterns/solid_pattern.hpp"
#include "shapes/aabb.hpp"
#include "shapes/cube.hpp"
#include "shapes/cylinder.hpp"
#include <memory>
#include <random>
#include <string>
#include <vector>

/// our includes
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/obj_file_loader.hpp"
#include "io/obj_parse_result.hpp"
#include "io/render_params.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/group.hpp"
#include "shapes/plane.hpp"
#include "shapes/sphere.hpp"
#include "utils/constants.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// file specific functions
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_camera();
        auto dst_fname = "dragon.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok camera, render the scene
        auto render_params   = RT::config_render_params();
        auto rendered_canvas = camera.render(world, render_params);
        rendered_canvas.write(dst_fname);

        return 0;
}

static RT::obj_parse_result parse_dragon_obj_file(void)
{
        RT::obj_file_parser dragon_obj_parser(RT::OBJ_ROOT + std::string("dragon.obj"));
        auto dragon_parse_result = dragon_obj_parser.parse();

        LOG_INFO("dragon parsed. summary:'%s'", dragon_parse_result.summarize().c_str());

        /// ------------------------------------------------------------
        /// add the default group and named groups into the world
        auto dragon = dragon_parse_result.default_group_ref();
        if (dragon != nullptr) {
                dragon->set_cast_shadow(true);
                dragon->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.1217, 0.0) *
                                  RT_XFORM::create_3d_scaling_matrix(0.268, 0.268, 0.268));

                /// ----------------------------------------------------
                /// let's divide the group with a '200' threshold.
                dragon->divide(100);
        }

        LOG_INFO("dragon loaded.");
        return dragon_parse_result;
}

/*
 * create a box instance
 **/
static std::shared_ptr<RT::cube> create_generic_bbox(void)
{
        auto bbox         = std::make_shared<RT::cube>(false);
        auto second_xform = RT_XFORM::create_3d_translation_matrix(-3.9863, -0.1217, -1.1820) *
                            RT_XFORM::create_3d_scaling_matrix(3.73335, 2.5845, 1.6283) *
                            RT_XFORM::create_3d_translation_matrix(1.0, 1.0, 1.0);

        auto first_xform = RT_XFORM::create_3d_translation_matrix(0, 0.1217, 0.0) *
                           RT_XFORM::create_3d_scaling_matrix(0.268, 0.268, 0.268);

        bbox->transform(first_xform * second_xform);
        return bbox;
}

/*
 * create a pedestal instance
 **/
static std::shared_ptr<RT::cylinder> create_pedestal(void)
{
        auto pedestal          = std::make_shared<RT::cylinder>(false, -0.15, 0.0, true);
        auto pedestal_color    = std::make_shared<RT::solid_pattern>(RT::color(0.2, 0.2, 0.2));
        auto pedestal_material = RT::material()
                                         .set_ambient(0.0)
                                         .set_diffuse(0.8)
                                         .set_specular(0.0)
                                         .set_reflective(0.2)
                                         .set_pattern(pedestal_color);

        pedestal->set_material(pedestal_material);

        return pedestal;
}

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// --------------------------------------------------------------------
        /// now create the world
        auto world = RT::world();

        /*
         * let there be light(s)
         **/
        {
                auto l_01 = RT::point_light(RT::create_point(-10.0, 100.0, -100.0), RT::color(1.0, 1.0, 1.0));
                world.add(l_01);

                auto l_02 = RT::point_light(RT::create_point(0.0, 100.0, 0.0), RT::color(0.1, 0.1, 0.1));
                world.add(l_02);

                auto l_03 = RT::point_light(RT::create_point(100.0, 10.0, -25.0), RT::color(0.2, 0.2, 0.2));
                world.add(l_03);

                auto l_04 = RT::point_light(RT::create_point(-100.0, 10.0, -25.0), RT::color(0.2, 0.2, 0.2));
                world.add(l_04);
        }

        /*
         * and now the scene using the primitives defined above.
         **/
        {
                /// ------------------------------------------------------------
                /// g_1
                {
                        /// ----------------------------------------------------
                        /// outer group
                        auto g_1 = std::make_shared<RT::group>();
                        g_1->transform(RT_XFORM::create_3d_translation_matrix(0.0, 2.0, 0.0));

                        /// ----------------------------------------------------
                        /// inner group
                        auto g_1_1 = std::make_shared<RT::group>();
                        {
                                /// ----------------------------------------------------
                                /// inner 'dragon'
                                auto dragon = parse_dragon_obj_file().default_group_ref();
                                auto dragon_color =
                                        std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.0, 0.1));
                                auto dragon_material = RT::material()
                                                               .set_ambient(0.1)
                                                               .set_diffuse(0.6)
                                                               .set_specular(0.3)
                                                               .set_shininess(15.0)
                                                               .set_pattern(dragon_color);
                                dragon->set_material(dragon_material);
                                g_1_1->add_child(dragon);

                                /// ----------------------------------------------------
                                /// inner box
                                auto bbox          = create_generic_bbox();
                                auto bbox_material = RT::material()
                                                             .set_ambient(0.0)
                                                             .set_diffuse(0.4)
                                                             .set_specular(0.0)
                                                             .set_transparency(0.6)
                                                             .set_refractive_index(1.0);
                                bbox->set_material(bbox_material);
                                /// g_1_1->add_child(bbox);
                        }

                        /// ----------------------------------------------------
                        /// finally add g_1 to the 'world'
                        g_1->add_child(create_pedestal());
                        g_1->add_child(g_1_1);
                        world.add(g_1);
                }

                /// ------------------------------------------------------------
                /// g_2
                {
                        auto g_2 = std::make_shared<RT::group>();
                        g_2->transform(RT_XFORM::create_3d_translation_matrix(2.0, 1.0, -1.0));

                        /// ----------------------------------------------------
                        /// inner group
                        auto g_2_1 = std::make_shared<RT::group>();
                        {
                                g_2_1->transform(RT_XFORM::create_roty_matrix(4.0) *
                                                 RT_XFORM::create_3d_scaling_matrix(0.75, 0.75, 0.75));

                                /// --------------------------------------------
                                /// new group contains a 'dragon'
                                auto dragon = parse_dragon_obj_file().default_group_ref();
                                auto dragon_color =
                                        std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.5, 0.1));
                                auto dragon_material = RT::material()
                                                               .set_ambient(0.1)
                                                               .set_diffuse(0.6)
                                                               .set_specular(0.3)
                                                               .set_shininess(15.0)
                                                               .set_pattern(dragon_color);
                                dragon->set_material(dragon_material);
                                g_2_1->add_child(dragon);

                                /// --------------------------------------------
                                /// new group contains a bbox
                                auto bbox          = create_generic_bbox();
                                auto bbox_material = RT::material()
                                                             .set_ambient(0.0)
                                                             .set_diffuse(0.2)
                                                             .set_specular(0.0)
                                                             .set_transparency(0.8)
                                                             .set_refractive_index(1.0);
                                bbox->set_material(bbox_material);
                                /// g_2_1->add_child(bbox);
                        }

                        /// ----------------------------------------------------
                        /// finally add g_2 to the world
                        g_2->add_child(create_pedestal());
                        g_2->add_child(g_2_1);
                        world.add(g_2);
                }

                /// ------------------------------------------------------------
                /// g_3
                {
                        auto g_3 = std::make_shared<RT::group>();
                        g_3->transform(RT_XFORM::create_3d_translation_matrix(-2.0, 0.75, -1.0));

                        /// ----------------------------------------------------
                        /// inner group
                        auto g_3_1 = std::make_shared<RT::group>();
                        {
                                g_3_1->transform(RT_XFORM::create_roty_matrix(-0.4) *
                                                 RT_XFORM::create_3d_scaling_matrix(0.75, 0.75, 0.75));

                                /// --------------------------------------------
                                /// new group contains a 'dragon'
                                auto dragon = parse_dragon_obj_file().default_group_ref();
                                auto dragon_color =
                                        std::make_shared<RT::solid_pattern>(RT::color(0.9, 0.5, 0.1));
                                auto dragon_material = RT::material()
                                                               .set_ambient(0.1)
                                                               .set_diffuse(0.6)
                                                               .set_specular(0.3)
                                                               .set_shininess(15.0)
                                                               .set_pattern(dragon_color);
                                dragon->set_material(dragon_material);
                                g_3_1->add_child(dragon);

                                /// --------------------------------------------
                                /// new group contains a bbox
                                auto bbox          = create_generic_bbox();
                                auto bbox_material = RT::material()
                                                             .set_ambient(0.0)
                                                             .set_diffuse(0.2)
                                                             .set_specular(0.0)
                                                             .set_transparency(0.8)
                                                             .set_refractive_index(1.0);
                                bbox->set_material(bbox_material);
                                /// g_3_1->add_child(bbox);
                        }

                        /// ----------------------------------------------------
                        /// finally add the child-group to the parent and the
                        /// parent group to the world
                        g_3->add_child(create_pedestal());
                        g_3->add_child(g_3_1);
                        world.add(g_3);
                }

                /// ------------------------------------------------------------
                /// g_4
                {
                        auto g_4 = std::make_shared<RT::group>();
                        g_4->transform(RT_XFORM::create_3d_translation_matrix(-4.0, 0.0, -2.0));

                        /// ----------------------------------------------------
                        /// inner group
                        auto g_4_1 = std::make_shared<RT::group>();
                        {
                                g_4_1->transform(RT_XFORM::create_roty_matrix(-0.2) *
                                                 RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5));

                                /// --------------------------------------------
                                /// new group contains a 'dragon'
                                auto dragon = parse_dragon_obj_file().default_group_ref();
                                auto dragon_color =
                                        std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.9, 0.1));
                                auto dragon_material = RT::material()
                                                               .set_ambient(0.1)
                                                               .set_diffuse(0.6)
                                                               .set_specular(0.3)
                                                               .set_shininess(15.0)
                                                               .set_pattern(dragon_color);
                                dragon->set_material(dragon_material);
                                g_4_1->add_child(dragon);

                                /// --------------------------------------------
                                /// new group contains a bbox
                                auto bbox          = create_generic_bbox();
                                auto bbox_material = RT::material()
                                                             .set_ambient(0.0)
                                                             .set_diffuse(0.1)
                                                             .set_specular(0.0)
                                                             .set_transparency(0.9)
                                                             .set_refractive_index(1.0);
                                bbox->set_material(bbox_material);
                                /// g_4_1->add_child(bbox);
                        }

                        /// ----------------------------------------------------
                        /// finally add g_4 to the world
                        g_4->add_child(create_pedestal());
                        g_4->add_child(g_4_1);
                        world.add(g_4);
                }

                /// ------------------------------------------------------------
                /// g_5
                {
                        auto g_5 = std::make_shared<RT::group>();
                        g_5->transform(RT_XFORM::create_3d_translation_matrix(4.0, 0.0, -2.0));

                        /// ----------------------------------------------------
                        /// inner group
                        auto g_5_1 = std::make_shared<RT::group>();
                        {
                                g_5_1->transform(RT_XFORM::create_roty_matrix(3.3) *
                                                 RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5));

                                /// --------------------------------------------
                                /// new group contains a 'dragon'
                                auto dragon = parse_dragon_obj_file().default_group_ref();
                                auto dragon_color =
                                        std::make_shared<RT::solid_pattern>(RT::color(0.9, 1.0, 0.1));
                                auto dragon_material = RT::material()
                                                               .set_ambient(0.1)
                                                               .set_diffuse(0.6)
                                                               .set_specular(0.3)
                                                               .set_shininess(15.0)
                                                               .set_pattern(dragon_color);
                                dragon->set_material(dragon_material);
                                g_5_1->add_child(dragon);

                                /// --------------------------------------------
                                /// new group contains a bbox
                                auto bbox          = create_generic_bbox();
                                auto bbox_material = RT::material()
                                                             .set_ambient(0.0)
                                                             .set_diffuse(0.1)
                                                             .set_specular(0.0)
                                                             .set_transparency(0.9)
                                                             .set_refractive_index(1.0);
                                bbox->set_material(bbox_material);
                                /// g_5_1->add_child(bbox);
                        }

                        /// ----------------------------------------------------
                        /// finally add g_5 to the world
                        g_5->add_child(create_pedestal());
                        g_5->add_child(g_5_1);
                        world.add(g_5);
                }

                /// ------------------------------------------------------------
                /// g_6
                {
                        auto g_6 = std::make_shared<RT::group>();
                        g_6->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.5, -4.0));

                        auto pedestal = create_pedestal();
                        g_6->add_child(pedestal);

                        /// ----------------------------------------------------
                        /// new group contains a 'dragon'
                        auto dragon          = parse_dragon_obj_file().default_group_ref();
                        auto dragon_color    = std::make_shared<RT::solid_pattern>(RT::color(1.0, 1.0, 1.0));
                        auto dragon_material = RT::material()
                                                       .set_ambient(0.1)
                                                       .set_diffuse(0.6)
                                                       .set_specular(0.3)
                                                       .set_shininess(15.0)
                                                       .set_pattern(dragon_color);
                        dragon->set_material(dragon_material);
                        dragon->transform(RT_XFORM::create_roty_matrix(3.1415) * dragon->transform());
                        g_6->add_child(dragon);

                        /// ----------------------------------------------------
                        /// finally add the group to the world
                        world.add(g_6);
                }
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01 = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, 1.2);
        camera_01.transform(RT_XFORM::create_view_transform(RT::create_point(0.0, 2.5, -10.0),  /// look-from
                                                            RT::create_point(0.0, 1.0, 0.0),    /// look-to
                                                            RT::create_vector(0.0, 1.0, 0.0))); /// up-vector

        return camera_01;
}
