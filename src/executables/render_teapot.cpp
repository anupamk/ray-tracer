/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "common/include/assert_utils.h"
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/obj_file_loader.hpp"
#include "io/obj_parse_result.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
#include "patterns/stock_materials.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "patterns/uv_mapper.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cylinder.hpp"
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
        auto dst_fname = "render-teapot.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok camera, render the scene
        auto render_params         = RT::config_render_params().antialias(true);
        auto const rendered_canvas = camera.render(world, render_params);
        rendered_canvas.write(dst_fname);

        return 0;
}

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// now create the world...
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        {
                auto l_01 = RT::point_light(RT::create_point(-5.0, 10.0, -40.0), RT::color_white());
                world.add(l_01);
        }

        /// --------------------------------------------------------------------
        /// a checkered floor
        {
                auto floor = std::make_shared<RT::plane>();
                world.add(floor);

                auto floor_mat = RT::material().set_specular(0.0).set_reflective(0.4);
                auto floor_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color(0.35, 0.35, 0.35),  /// color-a
                                                               RT::color(0.65, 0.65, 0.65)); /// color-b
                floor_mat.set_pattern(floor_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = RT_XFORM::create_roty_matrix(0.31415) *
                                   RT_XFORM::create_3d_translation_matrix(0.0, -20.0, 0.0);
                floor->transform(floor_xform);
        }

        /// --------------------------------------------------------------------
        /// glass ball with a hollow sphere
        {
                auto s_01 = std::make_shared<RT::sphere>(false);
                world.add(s_01);
                s_01->transform(RT_XFORM::create_3d_scaling_matrix(40.0, 40.0, 40.0));

                s_01->set_material(
                        RT::material()
                                .set_pattern(std::make_shared<RT::solid_pattern>(RT::color::RGB(200, 0, 0)))
                                .set_ambient(0.0)
                                .set_diffuse(0.0)
                                .set_specular(0.9)
                                .set_shininess(400)
                                .set_reflective(0.9)
                                .set_transparency(0.9)
                                .set_refractive_index(RT::material::RI_WATER));
        }

        /// ------------------------------------------------------------
        /// teapot
        {
                RT::obj_file_parser teapot_parser(RT::OBJ_ROOT + std::string("teapot-fine.obj"));
                auto parse_result = teapot_parser.parse();

                LOG_INFO("model-01 parsed, summary:'%s'", parse_result.summarize().c_str());

                auto model_xform = RT_XFORM::create_3d_scaling_matrix(7.0, 7.0, 7.0) *
                                   RT_XFORM::create_rotx_matrix(-0.2f * RT::PI);

                auto model_material = RT::create_material_matte(RT::color::RGB(200, 0, 0));

                /// ------------------------------------------------------------
                /// add default group and named groups into the world
                auto grp = parse_result.default_group_cref();
                if (grp != nullptr) {
                        world.add(grp);
                        grp->transform(model_xform);
                        grp->set_material(model_material);
                }

                auto grp_list = parse_result.group_list_cref();
                for (auto const& g_i : grp_list) {
                        world.add(g_i);
                        g_i->transform(model_xform);
                        g_i->set_material(model_material);
                }

                LOG_INFO("model-01 loaded");
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS, RT::canvas::Y_PIXELS, 2.0);
        auto look_from     = RT::create_point(-8.5, 5.0, -40.0);
        auto look_to       = RT::create_point(1.5, 1.5, 1.5);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
