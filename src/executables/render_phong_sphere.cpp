/*
 * single-threaded as well as multi-threaded rendering of a phong sphere.
 **/

/// c++ includes
#include <memory>

/// our includes
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
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

/// --------------------------------------------------------------------------------
/// file specific function declarations
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        auto const camera = create_camera();
        auto const world  = create_world();
        auto dst_fname    = "phong-sphere.ppm";

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
 * file specific functions
 **/

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// just a sphere
        {
                auto sphere_01         = std::make_shared<RT::sphere>();
                auto sphere_01_pattern = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.2, 1.0));
                sphere_01->set_material(RT::material().set_pattern(sphere_01_pattern).set_specular(0.2));
                sphere_01->transform(RT_XFORM::create_3d_translation_matrix(0.0, -0.40, 0.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.1, 2.1, 2.1));

                world.add(sphere_01);
        }

        /// --------------------------------------------------------------------
        /// and a light
        {
                auto world_light_01 =
                        RT::point_light(RT::create_point(-20.0, 20.0, -20.0), RT::color_white());

                world.add(world_light_01);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, RT::PI_BY_2F);
        auto look_from     = RT::create_point(0.0, 0.0, -5.0);
        auto look_to       = RT::create_point(0.0, -1.0, 5.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
