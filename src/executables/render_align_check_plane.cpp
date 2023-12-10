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
#include "patterns/align_check_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_mapper.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/plane.hpp"
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
        auto dst_fname = "align-check-plane.ppm";

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
        /// create the floor with a blended pattern
        auto floor = std::make_shared<RT::plane>();
        {
                auto floor_pattern = std::make_shared<RT::align_check>(RT::color(1.0, 1.0, 1.0),  /// main
                                                                       RT::color(1.0, 0.0, 0.0),  /// ul
                                                                       RT::color(1.0, 1.0, 0.0),  /// ur
                                                                       RT::color(0.0, 1.0, 0.0),  /// bl
                                                                       RT::color(0.0, 1.0, 1.0)); /// br

                auto floor_texture = std::make_shared<RT::texture_2d_pattern>(floor_pattern, RT::planar_map);

                floor->set_material(RT::material()
                                            .set_pattern(floor_texture) /// pattern
                                            .set_ambient(0.1)           /// ambient
                                            .set_diffuse(0.8));         /// specular
        }

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-10.0, 10.0, 10.0), RT::color_white());

        /// --------------------------------------------------------------------
        /// now create the world...
        auto world = RT::world();
        world.add(world_light_01);
        world.add(floor);

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, 0.5);
        auto look_from     = RT::create_point(1.0, 2.0, -5.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
