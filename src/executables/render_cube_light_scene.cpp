/*
 * render a scene using cubes as predominant shape
 **/

/// c++ includes
#include <memory>
#include <optional>
#include <random>
#include <string>

#include "common/include/assert_utils.h"
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/gradient_pattern.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "patterns/uv_mapper.hpp"
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
RT::world create_simple_world();
RT::camera create_simple_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_simple_world();
        auto camera    = create_simple_world_camera();
        auto dst_fname = "cube-light-scene.ppm";

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

        auto world_light_02 =
                RT::point_light(RT::create_point(5.0, 5.0, -5.0), RT::color::RGB(255, 255, 255));
        world.add(world_light_02);

        /// --------------------------------------------------------------------
        /// ceiling cube
        {
                auto ceiling_cube = std::make_shared<RT::sphere>(false);
                world.add(ceiling_cube);

                ceiling_cube->transform(RT_XFORM::create_3d_translation_matrix(-2.0, 8.0, 20.0) *
                                        RT_XFORM::create_3d_scaling_matrix(100.3, 0.2, 100.3));

                auto ceiling_cube_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0, 0, 100),                /// color-a
                        RT::color::RGB(200, 200, 200),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                ceiling_cube->set_material(RT::material()
                                                   .set_pattern(ceiling_cube_pattern)
                                                   .set_ambient(0.0)
                                                   .set_diffuse(0.4)
                                                   .set_specular(0.9)
                                                   .set_shininess(300)
                                                   .set_reflective(0.6)
                                                   .set_transparency(0.8)
                                                   .set_refractive_index(1.5));
        }

        /// --------------------------------------------------------------------
        /// floor cube
        {
                auto floor_cube = std::make_shared<RT::sphere>(false);
                world.add(floor_cube);

                floor_cube->transform(RT_XFORM::create_3d_translation_matrix(-2.0, -3.0, 3.0) *
                                      RT_XFORM::create_3d_scaling_matrix(-35.3, 0.3, -35.3));

                auto floor_cube_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0, 0, 100),                /// color-a
                        RT::color::RGB(200, 200, 200),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                floor_cube->set_material(RT::material()
                                                 .set_pattern(floor_cube_pattern)
                                                 .set_ambient(0.0)
                                                 .set_diffuse(0.4)
                                                 .set_specular(0.9)
                                                 .set_shininess(300)
                                                 .set_reflective(0.6)
                                                 .set_transparency(0.8)
                                                 .set_refractive_index(1.5));
        }

        /// --------------------------------------------------------------------
        /// a sun sphere
        {
                auto sun_sphere = std::make_shared<RT::sphere>(false);
                world.add(sun_sphere);

                sun_sphere->transform(RT_XFORM::create_3d_translation_matrix(-2.2, 3.5, 6.0) *
                                      RT_XFORM::create_rotz_matrix(RT::PI_BY_4F) *
                                      RT_XFORM::create_3d_scaling_matrix(4.25, 4.25, 4.25));

                auto sun_sphere_pattern =
                        std::make_shared<RT::gradient_pattern>(RT::color(0.8, 0.8, 0.0),  /// color-a
                                                               RT::color(0.9, 0.0, 0.0)); /// color-b

                sun_sphere_pattern->transform(RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0) *
                                              RT_XFORM::create_3d_scaling_matrix(2.0, 1.0, 1.0));

                sun_sphere->set_material(RT::material()
                                                 .set_pattern(sun_sphere_pattern) /// pattern
                                                 .set_diffuse(0.7)                /// diffuse
                                                 .set_specular(0.3));             /// specular
        }

        /// --------------------------------------------------------------------
        /// load the starry texture for wall and floor
        {
                auto maybe_star_map_canvas =
                        RT::canvas::load_from_file(RT::TEXTURE_ROOT + std::string("stars-8k.ppm"));
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
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, 0.8);
        auto look_from     = RT::create_point(-2.6, 4.5, -30.0);
        auto look_to       = RT::create_point(-0.6, 1.6, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
