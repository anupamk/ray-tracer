/*
 * render a scene using cylinders
 **/

/// c++ includes
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/// our includes
#include "common/include/benchmark.hpp"
#include "blended_pattern.hpp"
#include "camera.hpp"
#include "checkers_pattern.hpp"
#include "color.hpp"
#include "constants.hpp"
#include "cube.hpp"
#include "cylinder.hpp"
#include "gradient_pattern.hpp"
#include "gradient_perlin_noise_pattern.hpp"
#include "gradient_ring_pattern.hpp"
#include "common/include/logging.h"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "perlin_noise_pattern.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "raytracer_renderer.hpp"
#include "ring_pattern.hpp"
#include "shape_interface.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "striped_pattern.hpp"
#include "texture_2d_pattern.hpp"
#include "tuple.hpp"
#include "uv_checkers.hpp"
#include "uv_image_pattern.hpp"
#include "uv_mapper.hpp"
#include "uv_noise_texture.hpp"
#include "world.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// file specific functions
RT::world create_table_top_world();
RT::camera create_table_top_world_camera();

RT::world create_cylinder_world();
RT::camera create_cylinder_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_cylinder_world();
        auto camera    = create_cylinder_world_camera();
        auto dst_fname = "cylinder-scene.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// benchmark the render with 'num_iterations' renders performed, and
        /// throwing away the results from 'num_discards' of them
        auto const num_iterations = 1;
        auto const num_discards   = 0;
        Benchmark<> render_bm(num_iterations, num_discards);
        LOG_INFO("render benchmark info: '%s'", render_bm.stringify().c_str());

        /// --------------------------------------------------------------------
        /// just use the first [0] result only please
        auto rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
        rendered_canvas.write(dst_fname);

        /// --------------------------------------------------------------------
        /// show what we got
        LOG_INFO("render benchmark results : {mean (ms): '%05zu', standard-deviation (ms): '%05zu'}",
                 render_bm.mean(),                /// mean-usec
                 render_bm.standard_deviation()); /// stddev-usec

        return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ----------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
RT::world create_table_top_world()
{
        auto world          = RT::world();
        auto light_position = RT::create_point(0.0, 18.0, 0.0);

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(light_position, RT::color::RGB(255, 255, 255));
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// mirror
        {
                auto mirror_cube = std::make_shared<RT::cube>(false);
                world.add(mirror_cube);

                mirror_cube->transform(RT_XFORM::create_3d_translation_matrix(-29.9, 10.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(RT::PI_BY_2F) *
                                       RT_XFORM::create_3d_scaling_matrix(6.0, 0.1, 20.0));

                auto mirror_pattern = std::make_shared<RT::solid_pattern>(RT::color_black());

                mirror_cube->set_material(RT::material()
                                                  .set_pattern(mirror_pattern) /// pattern
                                                  .set_diffuse(0.9)            /// diffuse
                                                  .set_specular(0.9)           /// specular
                                                  .set_shininess(300.0)        /// shininess
                                                  .set_reflective(1.0));       /// reflective
        }

        /// --------------------------------------------------------------------
        /// floor + roof
        {
                auto floor_and_roof = std::make_shared<RT::cube>();
                world.add(floor_and_roof);

                floor_and_roof->transform(RT_XFORM::create_3d_translation_matrix(0.0, 10.0, 0.0) *
                                          RT_XFORM::create_3d_scaling_matrix(100.0, 10.0, 100.0));

                auto floor_and_roof_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color_black(), RT::color(0.6, 0.6, 0.6));
                floor_and_roof_pattern->transform(RT_XFORM::create_3d_scaling_matrix(0.2, 0.2, 0.2));

                floor_and_roof->set_material(RT::material()
                                                     .set_pattern(floor_and_roof_pattern)
                                                     .set_specular(0.1)
                                                     .set_ambient(0.25)
                                                     .set_shininess(2.0));
        }

        /// --------------------------------------------------------------------
        /// walls
        {
                auto walls = std::make_shared<RT::cube>();
                world.add(walls);

                walls->transform(RT_XFORM::create_3d_translation_matrix(0.0, 10.0, 0.0) *
                                 RT_XFORM::create_3d_scaling_matrix(30.0, 100.0, 30.0));

                auto walls_pattern = std::make_shared<RT::striped_pattern>(RT::color(0.6, 0.3, 0.1),
                                                                           RT::color(0.5, 0.2, 0.05));
                walls_pattern->transform(RT_XFORM::create_3d_scaling_matrix(0.1, 0.1, 0.1));

                walls->set_material(
                        RT::material().set_pattern(walls_pattern).set_specular(0.1).set_ambient(0.25));
        }

        /// --------------------------------------------------------------------
        /// lamp
        {
                auto lamp = std::make_shared<RT::cylinder>(false, /// cast-shadow
                                                           -1.0,  /// min
                                                           0.0,   /// max
                                                           true); /// capped
                world.add(lamp);

                lamp->transform(RT_XFORM::create_3d_translation_matrix(
                                        light_position.x(), light_position.y() + 1.0, light_position.z()) *
                                RT_XFORM::create_3d_scaling_matrix(2.0, 0.5, 2.0));

                auto lamp_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0xCC, 0x00));
                lamp->set_material(RT::material().set_pattern(lamp_pattern).set_reflective(0.1));
        }

        /// --------------------------------------------------------------------
        /// bulb
        {
                auto lamp = std::make_shared<RT::sphere>(false); /// no shadows
                world.add(lamp);

                lamp->transform(RT_XFORM::create_3d_translation_matrix(light_position.x(), light_position.y(),
                                                                       light_position.z()) *
                                RT_XFORM::create_3d_scaling_matrix(0.4, 0.6, 0.4));

                auto lamp_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0xCC, 0x00));
                lamp->set_material(RT::material().set_pattern(lamp_pattern).set_ambient(1.0));
        }

        /// --------------------------------------------------------------------
        /// table
        {
                auto const leg_positions = std::vector<std::pair<float, float>>{
                        {-10.0, -8.0},
                        {0.0, -8.0},
                        {0.0, 8.0},
                        {-10.0, 8.0},
                };

                for (auto leg_i_pos : leg_positions) {
                        auto leg_i = std::make_shared<RT::cube>();
                        world.add(leg_i);

                        // clang-format off
                        leg_i->transform(RT_XFORM::create_3d_translation_matrix(leg_i_pos.first, 3.0, leg_i_pos.second) *
                                         RT_XFORM::create_3d_scaling_matrix(0.25, 3.0, 0.25));
                        // clang-format on
                        auto leg_i_pattern = std::make_shared<RT::solid_pattern>(RT::color_red());

                        leg_i->set_material(RT::material().set_pattern(leg_i_pattern).set_ambient(0.25));
                }

                /// ------------------------------------------------------------
                /// top
                auto table_top = std::make_shared<RT::cube>();
                world.add(table_top);

                table_top->transform(RT_XFORM::create_3d_translation_matrix(-5.0, 6.25, 0.0) *
                                     RT_XFORM::create_3d_scaling_matrix(5.25, 0.5, 8.25));

                auto table_top_pattern = std::make_shared<RT::solid_pattern>(RT::color_red());
                table_top->set_material(RT::material().set_pattern(table_top_pattern).set_ambient(0.25));
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
RT::camera create_table_top_world_camera()
{
        auto camera_01 = RT::camera(1280, 1024, RT::PI_BY_3F);

        auto look_from     = RT::create_point(29.0, 10.0, -29.0);
        auto look_to       = RT::create_point(-5.0, 9.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a world which contains just cylinders as
/// primitive shapes
RT::world create_cylinder_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-3.0, 15.0, -2.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// create the floor
        {
                auto floor = std::make_shared<RT::plane>();
                /// world.add(floor);

                /// ------------------------------------------------------------
                /// make patterns on the floor
                auto fp_01 = std::make_shared<RT::striped_pattern>(RT::color(0.8, 0.8, 0.8),
                                                                   RT::color(0.6, 0.6, 0.6));

                fp_01->transform(RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
                                 RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 5.5));

                auto fp_02 = std::make_shared<RT::striped_pattern>(RT::color(0.8, 0.8, 0.8),
                                                                   RT::color(0.6, 0.6, 0.6));

                auto floor_pattern_01 = std::make_shared<RT::blended_pattern>(fp_01, fp_02);

                floor_pattern_01->transform(RT_XFORM::create_roty_matrix(-RT::PI_BY_2F) *
                                            RT_XFORM::create_3d_scaling_matrix(0.6, 1.0, 1.0));

                floor->set_material(RT::material().set_pattern(floor_pattern_01).set_specular(0.1));
        }

        auto cylinder_xform = RT_XFORM::create_3d_scaling_matrix(0.1, 0.1, 0.1);

        /// --------------------------------------------------------------------
        /// create a cylinder on the y-axis
        {
                auto cylinder_y = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(cylinder_y);

                cylinder_y->transform(cylinder_xform);

                auto cylinder_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0x00, 0xFF));
                cylinder_y->set_material(RT::material().set_pattern(cylinder_pattern).set_reflective(0.1));
        }

        /// --------------------------------------------------------------------
        /// create a cylinder on the x-axis
        {
                auto cylinder_x = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(cylinder_x);

                cylinder_x->transform(cylinder_xform * RT_XFORM::create_rotz_matrix(RT::PI_BY_2F));

                auto cylinder_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0x00, 0xFF));
                cylinder_x->set_material(RT::material().set_pattern(cylinder_pattern).set_reflective(0.1));
        }

        /// --------------------------------------------------------------------
        /// create a cylinder on the z-axis
        {
                auto cylinder_z = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(cylinder_z);

                cylinder_z->transform(cylinder_xform * RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));

                auto cylinder_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0x00, 0xFF));
                cylinder_z->set_material(RT::material().set_pattern(cylinder_pattern).set_reflective(0.1));
        }

        /// --------------------------------------------------------------------
        /// draw a y = sin(x) curve with cylinders...
        if (1) {
                constexpr auto x_begin     = -3.0 * RT::PI;
                constexpr auto x_end       = 3.0 * RT::PI;
                constexpr auto step_size   = 0.1;
                constexpr auto segment_len = 20.0; /// length-of-each-segment
                auto y_start               = 0.0;
                auto iter                  = 0;

                for (auto x = x_begin; x <= x_end; x += step_size, iter += 1) {
                        auto cs = std::make_shared<RT::cylinder>(false, 0.0, segment_len, false);
                        world.add(cs);

                        auto const segment_xform = RT_XFORM::create_3d_translation_matrix(x, y_start, 0.0) *
                                                   RT_XFORM::create_rotz_matrix(-std::cos(x)) *
                                                   RT_XFORM::create_3d_scaling_matrix(0.01, 0.01, 0.01);
                        cs->transform(segment_xform);

                        auto cp = std::make_shared<RT::solid_pattern>(RT::color::RGB(255, 140, 66 + iter));
                        cs->set_material(RT::material().set_pattern(cp).set_reflective(0.1));

                        /// next == current
                        y_start = std::sin(x);
                }
        }

        /// --------------------------------------------------------------------
        /// draw a y = sin(x) curve with cylinders...
        if (0) {
                auto cs = std::make_shared<RT::cylinder>(false, 0.0, 10.0, false);
                world.add(cs);

                auto const segment_xform = RT_XFORM::create_rotz_matrix(-RT::PI_BY_4F) *
                                           RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 0.0);
                cs->transform(segment_xform * cylinder_xform);

                auto cp = std::make_shared<RT::solid_pattern>(RT::color::RGB(255, 140, 66));
                cs->set_material(RT::material().set_pattern(cp).set_reflective(0.1));
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_cylinder_world(...)
RT::camera create_cylinder_world_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_3F);
        auto look_from     = RT::create_point(0.0, 0.5, -10.0);
        auto look_to       = RT::create_point(0.0, 1.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
