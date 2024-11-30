/*
 * render a scene using cylinders
 **/

/// c++ includes
#include <memory>

#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/material.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_checkers.hpp"
#include "patterns/uv_mapper.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cylinder.hpp"
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
RT::world create_cylinder_world();
RT::camera create_cylinder_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_cylinder_world();
        auto camera    = create_cylinder_world_camera();
        auto dst_fname = "cylinder-texture-scene.ppm";

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

/*
 * only file specific functions from this point onwards
 **/

/// ----------------------------------------------------------------------------
/// this function is called to create a world which contains just cylinders as
/// primitive shapes
RT::world create_cylinder_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-10.0, 10.0, -10.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// add the cylinder
        {
                auto cs = std::make_shared<RT::cylinder>(false, /// cast-a-shadow
                                                         0.0,   /// min-y
                                                         1.0,   /// max-y
                                                         true); /// capped ?
                world.add(cs);

                cs->transform(RT_XFORM::create_3d_translation_matrix(0.0, -0.5, 0.0) *
                              RT_XFORM::create_3d_scaling_matrix(1.0, RT::PI, 1.0) *
                              RT_XFORM::create_rotx_matrix(RT::PI / 15.0));

                auto cs_uv_pattern = std::make_shared<RT::uv_checkers>(16,                       /// u-squares
                                                                       RT::color(0.0, 0.5, 0.0), /// u-color
                                                                       8,                        /// v-squares
                                                                       RT::color(1.0, 1.0, 1.0)); /// v-color
                auto cs_texture =
                        std::make_shared<RT::texture_2d_pattern>(cs_uv_pattern, RT::cylindrical_map);

                auto cs_material = RT::material()
                                           .set_ambient(0.1)         /// ambient
                                           .set_specular(0.6)        /// specular
                                           .set_shininess(15)        /// shininess
                                           .set_diffuse(0.8)         /// diffuse
                                           .set_pattern(cs_texture); /// pattern
                cs->set_material(cs_material);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_cylinder_world(...)
RT::camera create_cylinder_world_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS, RT::canvas::Y_PIXELS, 0.5);
        auto look_from     = RT::create_point(0.0, 0.0, -20.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
