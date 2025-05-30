/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <memory>

/// our includes
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/render_params.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
#include "patterns/stock_materials.hpp"
#include "patterns/striped_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
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
        auto dst_fname = "render-with-t3r.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok camera, render the scene
        auto render_params   = RT::config_render_params().antialias(true);
        auto rendered_canvas = camera.render(world, render_params);
        rendered_canvas.write(dst_fname);

        return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// --------------------------------------------------------------------
        /// now create the world...
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-4.9, 4.9, -1.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// wall material
        auto wall_pattern = std::make_shared<RT::striped_pattern>(RT::color(0.45, 0.45, 0.45),  /// color-a
                                                                  RT::color(0.55, 0.55, 0.55)); /// color-b
        auto wall_xform =
                RT_XFORM::create_3d_scaling_matrix(0.25, 0.25, 0.25) * RT_XFORM::create_roty_matrix(1.5708);

        wall_pattern->transform(wall_xform);

        auto wall_material = RT::material()             /// canonical-material
                                     .set_ambient(0.0)  /// with
                                     .set_diffuse(0.4)  /// special
                                     .set_specular(0.0) /// properties
                                     .set_reflective(0.3)
                                     .set_pattern(wall_pattern);

        /// --------------------------------------------------------------------
        /// and now we describe the elements of the scene

        /// --------------------------------------------------------------------
        /// 01. checkered floor
        {
                auto floor     = std::make_shared<RT::plane>();
                auto floor_mat = RT::material().set_specular(0.0).set_reflective(0.4);
                auto floor_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color(0.35, 0.35, 0.35),  /// color-a
                                                               RT::color(0.65, 0.65, 0.65)); /// color-b
                floor_mat.set_pattern(floor_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = RT_XFORM::create_roty_matrix(0.31415);
                floor->transform(floor_xform);

                world.add(floor);
        }

        /// --------------------------------------------------------------------
        /// 02. the ceiling
        {
                auto ceiling         = std::make_shared<RT::plane>();
                auto ceiling_mat     = RT::material().set_ambient(0.3).set_specular(0.0);
                auto ceiling_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.8, 0.8, 0.8));

                ceiling_mat.set_pattern(ceiling_pattern);
                ceiling->set_material(ceiling_mat);

                auto ceiling_xform = RT_XFORM::create_3d_translation_matrix(0.0, 15.0, 0.0);
                ceiling->transform(ceiling_xform);

                world.add(ceiling);
        }

        /// --------------------------------------------------------------------
        /// 03. west-wall
        {
                auto west_wall       = std::make_shared<RT::plane>();
                auto west_wall_xform = RT_XFORM::create_3d_translation_matrix(-15.0, 0.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(1.5708) * /// rotate-to-vertical
                                       RT_XFORM::create_roty_matrix(1.5708);  /// orient-texture

                west_wall->set_material(wall_material);
                west_wall->transform(west_wall_xform);

                world.add(west_wall);
        }

        /// --------------------------------------------------------------------
        /// 04. east-wall
        {
                auto east_wall       = std::make_shared<RT::plane>();
                auto east_wall_xform = RT_XFORM::create_3d_translation_matrix(15.0, 0.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(1.5708) * /// rotate-to-vertical
                                       RT_XFORM::create_roty_matrix(1.5708);  /// orient-texture

                east_wall->set_material(wall_material);
                east_wall->transform(east_wall_xform);

                world.add(east_wall);
        }

        /// --------------------------------------------------------------------
        /// 05. north-wall
        {
                auto north_wall       = std::make_shared<RT::plane>();
                auto north_wall_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 15.0) *
                                        RT_XFORM::create_rotx_matrix(1.5708); /// rotate-to-vertical

                north_wall->set_material(wall_material);
                north_wall->transform(north_wall_xform);

                world.add(north_wall);
        }

        /// --------------------------------------------------------------------
        /// 05. south-wall
        {
                auto south_wall       = std::make_shared<RT::plane>();
                auto south_wall_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, -15.0) *
                                        RT_XFORM::create_rotx_matrix(1.5708); /// rotate-to-vertical

                south_wall->set_material(wall_material);
                south_wall->transform(south_wall_xform);

                world.add(south_wall);
        }

        /// --------------------------------------------------------------------
        /// background balls
        /// --------------------------------------------------------------------

        /// --------------------------------------------------------------------
        /// 01. bg_ball_01
        {
                auto bg_ball_01         = std::make_shared<RT::sphere>();
                auto bg_ball_01_mat     = RT::material().set_shininess(50);
                auto bg_ball_01_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.8, 0.5, 0.3));

                bg_ball_01_mat.set_pattern(bg_ball_01_pattern);
                bg_ball_01->set_material(bg_ball_01_mat);

                auto bg_ball_01_xform = RT_XFORM::create_3d_translation_matrix(4.0, 0.4, -2.0) *
                                        RT_XFORM::create_3d_scaling_matrix(0.4, 0.4, 0.4);

                bg_ball_01->transform(bg_ball_01_xform);

                world.add(bg_ball_01);
        }

        /// --------------------------------------------------------------------
        /// 02. bg_ball_02
        {
                auto bg_ball_02         = std::make_shared<RT::sphere>();
                auto bg_ball_02_mat     = RT::material().set_shininess(50);
                auto bg_ball_02_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.9, 0.4, 0.5));

                bg_ball_02_mat.set_pattern(bg_ball_02_pattern);
                bg_ball_02->set_material(bg_ball_02_mat);

                auto bg_ball_02_xform = RT_XFORM::create_3d_translation_matrix(4.7, 0.3, -0.4) *
                                        RT_XFORM::create_3d_scaling_matrix(0.3, 0.3, 0.3);

                bg_ball_02->transform(bg_ball_02_xform);

                world.add(bg_ball_02);
        }

        /// --------------------------------------------------------------------
        /// 03. ball_03
        {
                auto bg_ball_03         = std::make_shared<RT::sphere>();
                auto bg_ball_03_mat     = RT::material().set_shininess(50);
                auto bg_ball_03_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.4, 0.9, 0.6));

                bg_ball_03_mat.set_pattern(bg_ball_03_pattern);
                bg_ball_03->set_material(bg_ball_03_mat);

                auto bg_ball_03_xform = RT_XFORM::create_3d_translation_matrix(-1.0, 0.5, 4.5) *
                                        RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5);

                bg_ball_03->transform(bg_ball_03_xform);

                world.add(bg_ball_03);
        }

        /// --------------------------------------------------------------------
        /// 04. bg_ball_04
        {
                auto bg_ball_04         = std::make_shared<RT::sphere>();
                auto bg_ball_04_mat     = RT::material().set_shininess(50);
                auto bg_ball_04_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.4, 0.6, 0.9));

                bg_ball_04_mat.set_pattern(bg_ball_04_pattern);
                bg_ball_04->set_material(bg_ball_04_mat);

                auto bg_ball_04_xform = RT_XFORM::create_3d_translation_matrix(-1.7, 0.3, 4.7) *
                                        RT_XFORM::create_3d_scaling_matrix(0.3, 0.3, 0.3);

                bg_ball_04->transform(bg_ball_04_xform);

                world.add(bg_ball_04);
        }

        /// --------------------------------------------------------------------
        /// foreground balls
        /// --------------------------------------------------------------------

        /// --------------------------------------------------------------------
        /// 01. fg_ball_01
        {
                auto fg_ball_01         = std::make_shared<RT::sphere>();
                auto fg_ball_01_mat     = RT::material().set_shininess(5).set_specular(0.4);
                auto fg_ball_01_pattern = std::make_shared<RT::solid_pattern>(RT::color(1, 0.3, 0.2));

                fg_ball_01_mat.set_pattern(fg_ball_01_pattern);
                fg_ball_01->set_material(fg_ball_01_mat);

                auto fg_ball_01_xform = RT_XFORM::create_3d_translation_matrix(-0.6, 1.0, 0.6);
                fg_ball_01->transform(fg_ball_01_xform);

                world.add(fg_ball_01);
        }

        /// --------------------------------------------------------------------
        /// 02. fg_ball_02
        {
                /// blue-glass
                auto blue_color = RT::color(0.0, 0.0, 0.2);
                auto fg_ball_02 = std::make_shared<RT::sphere>();
                fg_ball_02->set_material(create_material_colored_glass(blue_color));

                auto fg_ball_02_xform = RT_XFORM::create_3d_translation_matrix(0.6, 0.7, -0.6) *
                                        RT_XFORM::create_3d_scaling_matrix(0.7, 0.7, 0.7);

                fg_ball_02->transform(fg_ball_02_xform);

                world.add(fg_ball_02);
        }

        /// --------------------------------------------------------------------
        /// 03. fg_ball_03
        {
                auto green_color = RT::color(0.0, 0.2, 0.0);
                auto fg_ball_03  = std::make_shared<RT::sphere>();
                fg_ball_03->set_material(create_material_colored_glass(green_color));

                auto fg_ball_03_xform = RT_XFORM::create_3d_translation_matrix(-0.7, 1.5, -0.8) *
                                        RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5);

                fg_ball_03->transform(fg_ball_03_xform);

                world.add(fg_ball_03);
        }

        /// --------------------------------------------------------------------
        /// 04. fg_ball_04
        {
                auto red_color  = RT::color(1.0, 0.0, 0.0);
                auto fg_ball_04 = std::make_shared<RT::sphere>();
                fg_ball_04->set_material(create_material_colored_glass(red_color));

                auto fg_ball_04_xform = RT_XFORM::create_3d_translation_matrix(3.2, 1.5, 1.2) *
                                        RT_XFORM::create_3d_scaling_matrix(1.5, 1.5, 1.5);

                fg_ball_04->transform(fg_ball_04_xform);

                world.add(fg_ball_04);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, 1.152);
        auto look_from     = RT::create_point(-5.6, 2.5, -8.9);
        auto look_to       = RT::create_point(-0.6, 1.0, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
