/*
 * render a scene using cubes as predominant shape
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
#include "blended_pattern.hpp"
#include "camera.hpp"
#include "checkers_pattern.hpp"
#include "color.hpp"
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "constants.hpp"
#include "cube.hpp"
#include "gradient_pattern.hpp"
#include "gradient_perlin_noise_pattern.hpp"
#include "gradient_ring_pattern.hpp"
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
RT::world create_table_world();
RT::camera create_table_world_camera();

RT::world create_earth_world();
RT::camera create_earth_world_camera();

RT::world create_simple_world();
RT::camera create_simple_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_simple_world();
        auto camera    = create_simple_world_camera();
        auto dst_fname = "cube-scene.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok render the scene
        Benchmark<> render_bm("ST render");

        auto const rendered_canvas = render_bm.benchmark(RT::single_threaded_renderer, world, camera)[0];
        rendered_canvas.write(dst_fname);
        render_bm.show_stats();

        return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
RT::world create_table_world()
{
        auto world          = RT::world();
        auto light_position = RT::create_point(0.0, 19.0, 0.0);

        /// --------------------------------------------------------------------
        /// lights
        {
                auto l_01 = RT::point_light(light_position, RT::color_white());
                world.add(std::move(l_01));
        }

        /// --------------------------------------------------------------------
        /// mirror
        {
                auto mirror = std::make_shared<RT::cube>();
                world.add(mirror);

                mirror->transform(RT_XFORM::create_3d_translation_matrix(-29.0, 10.0, 0.0) *
                                  RT_XFORM::create_rotx_matrix(RT::PI_BY_2F) *
                                  RT_XFORM::create_3d_scaling_matrix(6.0, 0.1, 20.0));

                auto mirror_pattern = std::make_shared<RT::solid_pattern>(RT::color_black());
                mirror->set_material(RT::material()
                                             .set_pattern(mirror_pattern)
                                             .set_diffuse(0.9)
                                             .set_specular(0.9)
                                             .set_shininess(300.0)
                                             .set_reflective(1.0));
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
                auto lamp = std::make_shared<RT::sphere>(false); /// no shadows
                world.add(lamp);

                lamp->transform(RT_XFORM::create_3d_translation_matrix(
                                        light_position.x(), light_position.y() + 1.0, light_position.z()) *
                                RT_XFORM::create_3d_scaling_matrix(2.0, 0.5, 2.0));

                auto lamp_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xFF, 0xCC, 0x00));
                lamp->set_material(RT::material()
                                           .set_pattern(lamp_pattern)
                                           .set_ambient(1.0)
                                           .set_specular(0.0)
                                           .set_diffuse(0.0));
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
RT::camera create_table_world_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_3F);
        auto look_from     = RT::create_point(29.0, 10.0, -29.0);
        auto look_to       = RT::create_point(-5.0, 9.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
RT::world create_earth_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_xyz01 = RT::point_light(RT::create_point(-10.0, 3.0, -9.5), RT::color_white());
        world.add(world_light_xyz01);

        auto world_light_xyz03 = RT::point_light(RT::create_point(12.0, 5.0, -5.5), RT::color_white());
        world.add(world_light_xyz03);

        /// --------------------------------------------------------------------
        /// simple-cloud-cube
        {
                auto cloud_cube = std::make_shared<RT::cube>(false);
                world.add(cloud_cube);

                cloud_cube->transform(RT_XFORM::create_3d_scaling_matrix(4.3, 2.0, 0.5) *
                                      RT_XFORM::create_3d_translation_matrix(0.5, 2.4, 0.4));

                /// auto cloud_cube_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                /// 	RT::color::RGB(0, 0, 90),                 /// color-a
                /// 	RT::color::RGB(100, 100, 100),            /// color-a
                /// 	std::default_random_engine::default_seed, /// seed
                /// 	16);

                /// cloud_cube_pattern->transform(RT_XFORM::create_rotx_matrix(-RT::PI_BY_6F));

                auto cloud_cube_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.1, 0.1, 0.1));
                cloud_cube->set_material(RT::material()
                                                 .set_pattern(cloud_cube_pattern)
                                                 .set_ambient(0.0)
                                                 .set_diffuse(0.4)
                                                 .set_specular(0.9)
                                                 .set_shininess(300)
                                                 .set_reflective(0.9)
                                                 .set_transparency(0.9)
                                                 .set_refractive_index(1.1));
        }

        /// --------------------------------------------------------------------
        /// red-sphere
        auto red_sphere = std::make_shared<RT::sphere>(false);
        {
                /// world.add(red_sphere);

                auto fuzzy_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color(0.5, 0.0, 0.0),                 /// u
                        RT::color(1.0, 0.6, 0.6),                 /// v
                        std::default_random_engine::default_seed, /// seed
                        16);                                      /// octaves

                red_sphere->transform(RT_XFORM::create_3d_translation_matrix(4.5, 6.0, 10.0) *
                                      RT_XFORM::create_3d_scaling_matrix(4.0, 4.0, 4.0) *
                                      RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
                                      RT_XFORM::create_rotz_matrix(-RT::PI));

                red_sphere->set_material(RT::material()
                                                 .set_pattern(fuzzy_pattern) /// pattern
                                                 .set_ambient(0.1)           /// ambient
                                                 .set_diffuse(0.9)           /// diffuse
                                                 .set_shininess(10.0)        /// shininess
                                                 .set_specular(0.1));        /// specular
        }

        /// --------------------------------------------------------------------
        /// red-sphere
        auto earth_sphere = std::make_shared<RT::sphere>(false);
        {
                world.add(earth_sphere);

                auto maybe_earth_map_canvas = RT::canvas::load_from_file("../textures/nasa-blue-marble.ppm");
                ASSERT(maybe_earth_map_canvas.has_value());

                auto earth_map_canvas = maybe_earth_map_canvas.value();
                auto earth_texture    = std::make_shared<RT::uv_image>(earth_map_canvas);
                auto earth_pattern =
                        std::make_shared<RT::texture_2d_pattern>(earth_texture, RT::spherical_map);

                earth_sphere->transform(RT_XFORM::create_3d_translation_matrix(3.5, 5.0, 10.0) *
                                        RT_XFORM::create_3d_scaling_matrix(4.5, 4.5, 4.5) *
                                        RT_XFORM::create_rotz_matrix(RT::PI_BY_6F) *
                                        RT_XFORM::create_rotx_matrix(-RT::PI / 8.0));

                earth_sphere->set_material(RT::material()
                                                   .set_pattern(earth_pattern) /// pattern
                                                   .set_ambient(0.1)           /// ambient
                                                   .set_diffuse(0.9)           /// diffuse
                                                   .set_shininess(10.0)        /// shininess
                                                   .set_specular(0.1));        /// specular
        }

        /// 01. solid-glassy floor
        auto floor = std::make_shared<RT::plane>();
        {
                /// world.add(floor);

                auto floor_mat = RT::material()
                                         .set_ambient(0.1)
                                         .set_diffuse(0.9)
                                         .set_specular(0.9)
                                         .set_shininess(300)
                                         .set_reflective(0.1)
                                         .set_transparency(1.0)
                                         .set_refractive_index(RT::material::RI_GLASS);

                auto floor_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.1, 0.1, 0.1));

                floor_mat.set_pattern(floor_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = (RT_XFORM::create_roty_matrix(0.31415) *
                                    RT_XFORM::create_3d_translation_matrix(0.0, -2.0, 0.0));
                floor->transform(floor_xform);
        }

        /// 01. cloudy-glassy floor
        auto floor_01 = std::make_shared<RT::plane>();
        {
                world.add(floor_01);

                auto floor_mat     = RT::material().set_specular(0.0).set_reflective(0.9).set_shininess(100);
                auto floor_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(150, 150, 150),            /// color-a
                        RT::color::RGB(200, 200, 200),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                /// cloud_cube_pattern->transform(RT_XFORM::create_rotx_matrix(-RT::PI_BY_6F));

                /// auto floor_mat = RT::material().set_specular(0.0).set_reflective(0.1);
                /// auto floor_pattern =
                /// 	std::make_shared<RT::checkers_pattern>(RT::color(0.35, 0.35, 0.35),  /// color-a
                ///                                                RT::color(0.65, 0.65, 0.65)); /// color-b

                floor_mat.set_pattern(floor_pattern);
                floor_01->set_material(floor_mat);

                auto floor_xform = (RT_XFORM::create_roty_matrix(0.31415) *
                                    RT_XFORM::create_3d_translation_matrix(0.0, -2.5, 0.0));
                floor_01->transform(floor_xform);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
RT::camera create_earth_world_camera()
{
        auto camera_01     = RT::camera(320, 256, 0.8);
        auto look_from     = RT::create_point(-2.6, 4.5, -20.0);
        auto look_to       = RT::create_point(-0.6, 2.5, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
RT::world create_simple_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 =
                RT::point_light(RT::create_point(-100.0, 3.0, -9.5), RT::color::RGB(255, 255, 255));
        world.add(world_light_01);

        auto world_light_02 = RT::point_light(RT::create_point(0.0, 6.0, 2.0), RT::color::RGB(255, 255, 255));
        world.add(world_light_02);

        /// --------------------------------------------------------------------
        /// step-01: an earth sphere
        auto earth_sphere = std::make_shared<RT::sphere>(false);
        {
                world.add(earth_sphere);

                auto maybe_earth_map_canvas = RT::canvas::load_from_file("../textures/nasa-blue-marble.ppm");
                ASSERT(maybe_earth_map_canvas.has_value());

                auto earth_map_canvas = maybe_earth_map_canvas.value();
                auto earth_texture    = std::make_shared<RT::uv_image>(earth_map_canvas);
                auto earth_pattern =
                        std::make_shared<RT::texture_2d_pattern>(earth_texture, RT::spherical_map);

                earth_sphere->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.5, 0.0) *
                                        RT_XFORM::create_3d_scaling_matrix(4.9, 4.9, 4.9) *
                                        RT_XFORM::create_roty_matrix(RT::PI / 1.75));

                earth_sphere->set_material(RT::material()
                                                   .set_pattern(earth_pattern) /// pattern
                                                   .set_ambient(2.125)         /// ambient
                                                   .set_diffuse(0.9)           /// diffuse
                                                   .set_shininess(400.0)       /// shininess
                                                   .set_reflective(0.9)
                                                   .set_specular(0.1)); /// specular
        }

        /// --------------------------------------------------------------------
        /// step-02 : the atmosphere which encloses the earth
        auto atmos_sphere = std::make_shared<RT::sphere>(false);
        {
                world.add(atmos_sphere);
                atmos_sphere->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.5, 0.0) *
                                        RT_XFORM::create_3d_scaling_matrix(5.0, 5.0, 5.0));

                auto atmos_sphere_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0, 0, 64),                 /// color-a
                        RT::color::RGB(155, 155, 255),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                atmos_sphere_pattern->transform(RT_XFORM::create_rotz_matrix(RT::PI_BY_2F));

                atmos_sphere->set_material(RT::material()
                                                   .set_pattern(atmos_sphere_pattern)
                                                   .set_diffuse(0.4)
                                                   .set_specular(0.1)
                                                   .set_transparency(0.225)
                                                   .set_reflective(0.1)
                                                   .set_refractive_index(RT::material::RI_WATER));
        }

        /// --------------------------------------------------------------------
        /// step-03 : and the blue-shell
        auto shell_sphere = std::make_shared<RT::sphere>(false);
        {
                world.add(shell_sphere);

                shell_sphere->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.5, 0.0) *
                                        RT_XFORM::create_3d_scaling_matrix(5.25, 5.25, 5.25));

                auto shell_sphere_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0, 0, 50));
                shell_sphere->set_material(RT::material()
                                                   .set_pattern(shell_sphere_pattern)
                                                   .set_ambient(0.0)
                                                   .set_diffuse(0.0)
                                                   .set_specular(0.1)
                                                   .set_shininess(300)
                                                   .set_transparency(0.99)
                                                   .set_refractive_index(1.2));
        }

        /// --------------------------------------------------------------------
        /// a transparent-cube enclosing the earth
        {
                auto transparent_cube = std::make_shared<RT::cube>(false);
                world.add(transparent_cube);

                transparent_cube->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.5, 0.0) *
                                            RT_XFORM::create_3d_scaling_matrix(5.3, 5.3, 5.3) *
                                            RT_XFORM::create_roty_matrix(-RT::PI / 1.5) *
                                            RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));

                auto transparent_cube_pattern =
                        std::make_shared<RT::solid_pattern>(RT::color(0.01, 0.01, 0.01));

                transparent_cube->set_material(RT::material()
                                                       .set_pattern(transparent_cube_pattern)
                                                       .set_ambient(0.0)
                                                       .set_diffuse(0.4)
                                                       .set_specular(0.9)
                                                       .set_shininess(300)
                                                       .set_reflective(0.1)
                                                       .set_transparency(0.8)
                                                       .set_refractive_index(1.055));
        }

        /// --------------------------------------------------------------------
        /// load the starry texture for wall and floor
        {
                auto maybe_star_map_canvas = RT::canvas::load_from_file("../textures/stars-8k.ppm");
                ASSERT(maybe_star_map_canvas.has_value());

                auto star_map_canvas  = maybe_star_map_canvas.value();
                auto star_map_texture = std::make_shared<RT::uv_image>(star_map_canvas);

                auto star_map_pattern =
                        std::make_shared<RT::texture_2d_pattern>(star_map_texture, RT::planar_map);

                /// --------------------------------------------------------------------
                /// floor first
                {
                        auto floor = std::make_shared<RT::plane>();
                        world.add(floor);

                        floor->transform(RT_XFORM::create_3d_translation_matrix(0.0, -10.0, 0.0));
                        floor->set_material(RT::material().set_pattern(star_map_pattern));
                }

                /// --------------------------------------------------------------------
                /// wall next
                {
                        auto wall = std::make_shared<RT::plane>();
                        world.add(wall);

                        wall->transform(RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 100.0) *
                                        RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));
                        wall->set_material(RT::material().set_pattern(star_map_pattern));
                }
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
RT::camera create_simple_world_camera()
{
        auto camera_01     = RT::camera(320 * 4, 256 * 4, 0.8);
        auto look_from     = RT::create_point(-2.6, 4.5, -30.0);
        auto look_to       = RT::create_point(-0.6, 0.6, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
