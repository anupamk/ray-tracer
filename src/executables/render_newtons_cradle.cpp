/*
 * render a scene using cylinders
 **/

/// c++ includes
#include <memory>
#include <random>

/// our includes
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
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
RT::world create_world();
RT::camera create_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_world_camera();
        auto dst_fname = "newtons-cradle-scene.ppm";

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

static std::shared_ptr<RT::group> create_pendulum(RT::color sphere_color)
{
        auto pendulum = std::make_shared<RT::group>();

        /// a sphere
        {
                auto sphere = std::make_shared<RT::sphere>();
                pendulum->add_child(sphere);

                auto sphere_xform = RT_XFORM::create_3d_scaling_matrix(0.3, 0.3, 0.3);
                sphere->transform(sphere_xform);

                auto s_color = std::make_shared<RT::solid_pattern>(sphere_color);
                sphere->set_material(RT::material()
                                             .set_pattern(s_color)
                                             .set_ambient(0.1)
                                             .set_diffuse(0.6)
                                             .set_specular(0.2)
                                             .set_shininess(10.0)
                                             .set_reflective(0.1));
        }

        /// something where the wire is attached
        {
                auto wire_support = std::make_shared<RT::cylinder>(true,  /// cast-shadow
                                                                   0.0,   /// min
                                                                   1.0,   /// max
                                                                   true); /// capped
                pendulum->add_child(wire_support);

                auto wire_support_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.3, 0.0) *
                                          RT_XFORM::create_3d_scaling_matrix(0.05, 0.04, 0.05);
                wire_support->transform(wire_support_xform);

                auto ws_color = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0xCC, 0xFF));
                wire_support->set_material(RT::material().set_pattern(ws_color));
        }

        /// a wire
        {
                auto wire = std::make_shared<RT::cylinder>(true,   /// cast-shadow
                                                           0.0,    /// min
                                                           1.0,    /// max
                                                           false); /// capped
                pendulum->add_child(wire);

                auto wire_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.3, 0.0) *
                                  RT_XFORM::create_3d_scaling_matrix(0.01, 5.0, 0.01);
                wire->transform(wire_xform);

                auto w_color = std::make_shared<RT::solid_pattern>(RT::color(0.9, 0.9, 0.9));
                wire->set_material(RT::material()
                                           .set_pattern(w_color)
                                           .set_ambient(0.1)
                                           .set_diffuse(0.6)
                                           .set_specular(0.3)
                                           .set_shininess(10.0)
                                           .set_reflective(0.1));
        }

        return pendulum;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a world which contains just cylinders as
/// primitive shapes
RT::world create_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        /// auto world_light_01 = RT::point_light(RT::create_point(-3.0, 15.0, -2.0), RT::color_white());
        auto world_light_01 = RT::point_light(RT::create_point(-4.0, 5.0, -20.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// x,y,z axes
        {
                auto cylinder_xform = RT_XFORM::create_3d_scaling_matrix(0.01, 0.01, 0.01);
                auto axes_pattern   = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xff, 0x45, 0x00));

                auto x_axis = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(x_axis);

                x_axis->transform(cylinder_xform * RT_XFORM::create_rotz_matrix(RT::PI_BY_2F));
                x_axis->set_material(RT::material().set_pattern(axes_pattern).set_reflective(0.1));

                auto z_axes = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(z_axes);

                z_axes->transform(cylinder_xform * RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));
                z_axes->set_material(RT::material().set_pattern(axes_pattern).set_reflective(0.1));
        }

        /// --------------------------------------------------------------------
        /// a checkered floor
        {
                auto floor = std::make_shared<RT::plane>();
                world.add(floor);

                auto floor_mat     = RT::material().set_specular(0.0).set_reflective(0.4);
                auto cloud_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0x19, 0x19, 0x70),         /// color-a
                        RT::color::RGB(0xDC, 0xDC, 0xDC),         /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                cloud_pattern->transform(RT_XFORM::create_3d_scaling_matrix(1.2, 3.0, 1.0));

                floor_mat.set_pattern(cloud_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = RT_XFORM::create_roty_matrix(0.31415) *
                                   RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0);
                floor->transform(floor_xform);
        }

        /// --------------------------------------------------------------------
        {
                auto pendulum_01 = create_pendulum(RT::color::RGB(0xFF, 0xC4, 0xC4));
                world.add(pendulum_01);

                auto pendulum_02 = create_pendulum(RT::color::RGB(0xC4, 0xFF, 0xC4));
                world.add(pendulum_02);
                auto p_02_xform = RT_XFORM::create_3d_translation_matrix(0.65, 0.0, 0.0);
                pendulum_02->transform(p_02_xform);

                auto pendulum_03 = create_pendulum(RT::color::RGB(0xC4, 0xC4, 0xFF));
                world.add(pendulum_03);
                auto p_03_xform = RT_XFORM::create_3d_translation_matrix(1.33, 0.0, 0.0);
                pendulum_03->transform(p_03_xform);

                auto pendulum_04 = create_pendulum(RT::color::RGB(0xC4, 0xC4, 0xC4));
                world.add(pendulum_04);
                auto p_04_xform = RT_XFORM::create_3d_translation_matrix(2.0, 0.0, 0.0);
                pendulum_04->transform(p_04_xform);

                auto pendulum_05 = create_pendulum(RT::color::RGB(0xD4, 0xD4, 0xD4));
                world.add(pendulum_05);
                auto p_05_xform = RT_XFORM::create_3d_translation_matrix(-0.65, 0.1, 0.0) *
                                  RT_XFORM::create_rotz_matrix(-RT::PI / 19.0);
                pendulum_05->transform(p_05_xform);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_world(...)
RT::camera create_world_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS, RT::canvas::Y_PIXELS, RT::PI_BY_2F);
        auto look_from     = RT::create_point(-1.2, 0.5, -2.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
