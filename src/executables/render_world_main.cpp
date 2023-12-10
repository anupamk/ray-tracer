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
#include "io/world.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
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

/// file specific functions
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        auto const world  = create_world();
        auto const camera = create_camera();
        auto dst_fname    = "render-world-simple.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok camera, render the scene
        auto const rendered_canvas = camera.render(world);
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
        /// wall material
        auto const wall_pattern  = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.9, 0.9));
        auto const wall_material = RT::material().set_pattern(wall_pattern).set_specular(0.0);

        /// --------------------------------------------------------------------
        /// 01: floor sphere
        auto floor = std::make_shared<RT::sphere>();
        floor->transform(RT_XFORM::create_3d_scaling_matrix(100.0, 0.01, 100.0));
        floor->set_material(wall_material);

        /// --------------------------------------------------------------------
        /// 02: left wall
        auto left_wall = std::make_shared<RT::sphere>();
        left_wall->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 5.0) *
                             RT_XFORM::create_roty_matrix(-RT::PI_BY_4F) *
                             RT_XFORM::create_rotx_matrix(RT::PI_BY_2F) *
                             RT_XFORM::create_3d_scaling_matrix(100.0, 0.01, 100.0));
        left_wall->set_material(wall_material);

        /// --------------------------------------------------------------------
        /// 03: right wall
        auto right_wall = std::make_shared<RT::sphere>();
        right_wall->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 5.0) *
                              RT_XFORM::create_roty_matrix(RT::PI_BY_4F) *
                              RT_XFORM::create_rotx_matrix(RT::PI_BY_2F) *
                              RT_XFORM::create_3d_scaling_matrix(100.0, 0.01, 100.0));
        right_wall->set_material(wall_material);

        /// --------------------------------------------------------------------
        /// sphere-01
        auto sphere_01         = std::make_shared<RT::sphere>();
        auto sphere_01_pattern = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.0, 0.0));

        sphere_01->transform(RT_XFORM::create_3d_translation_matrix(-3.0, 3.0, -2.0) *
                             RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

        // clang-format off
        sphere_01->set_material(RT::material()
                                .set_pattern(sphere_01_pattern)
                                .set_diffuse(0.8)
                                .set_specular(0.3));
        // clang-format on

        /// --------------------------------------------------------------------
        /// sphere-02
        auto sphere_02         = std::make_shared<RT::sphere>();
        auto sphere_02_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.0, 1.0, 0.0));

        sphere_02->transform(RT_XFORM::create_3d_translation_matrix(-10.0, 3.0, -12.0) *
                             RT_XFORM::create_3d_scaling_matrix(2.5, 2.5, 2.5));

        // clang-format off
        sphere_02->set_material(RT::material()
                                .set_pattern(sphere_02_pattern)
                                .set_diffuse(0.7)
                                .set_specular(0.3));
        // clang-format on

        /// --------------------------------------------------------------------
        /// sphere-03
        auto sphere_03         = std::make_shared<RT::sphere>();
        auto sphere_03_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.0, 0.0, 1.0));

        sphere_03->transform(RT_XFORM::create_3d_translation_matrix(5.0, 4.0, -7.0) *
                             RT_XFORM::create_3d_scaling_matrix(2.5, 2.5, 2.5));

        // clang-format off
        sphere_03->set_material(RT::material()
                                .set_pattern(sphere_03_pattern)
                                .set_diffuse(0.7)
                                .set_specular(0.3));
        // clang-format on

        /// --------------------------------------------------------------------
        /// 04: sphere-04
        auto sphere_04         = std::make_shared<RT::sphere>();
        auto sphere_04_pattern = std::make_shared<RT::solid_pattern>(RT::color(225.0 / 255.0, /// red
                                                                               213.0 / 255.0, /// green
                                                                               0.0 / 255.0)); /// blue

        sphere_04->transform(RT_XFORM::create_3d_translation_matrix(-5.5, 2.0, -18.0) *
                             RT_XFORM::create_3d_scaling_matrix(1.5, 1.5, 1.5));

        // clang-format off
        sphere_04->set_material(RT::material()
                                .set_pattern(sphere_04_pattern)
                                .set_diffuse(0.7)
                                .set_specular(0.3));
        // clang-format on

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-20.0, 10.0, -20.0), RT::color_white());

        /// --------------------------------------------------------------------
        /// now create the world...
        auto world = RT::world();
        world.add(world_light_01);

        world.add(floor);
        world.add(left_wall);
        world.add(right_wall);
        world.add(sphere_01);
        world.add(sphere_02);
        world.add(sphere_03);
        world.add(sphere_04);

        LOG_DEBUG("world details:\n%s\n", world.stringify().c_str());

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_2F);
        auto look_from     = RT::create_point(-7.5, 5.0, -25.0);
        auto look_to       = RT::create_point(0.0, -1.0, 5.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
