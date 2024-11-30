/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <memory>
#include <optional>
#include <random>
#include <string>

/// our includes
#include "common/include/assert_utils.h"
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/perlin_noise_pattern.hpp"
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
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_camera();
        auto dst_fname = "render-blue-earth.ppm";

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

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// --------------------------------------------------------------------
        /// create the floor with a blended pattern
        auto floor = std::make_shared<RT::plane>();
        {
                auto floor_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color(0.6, 0.6, 0.6),                 /// color-a
                        RT::color(0.7, 0.7, 0.7),                 /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);                                      /// octaves

                floor->set_material(RT::material()
                                            .set_pattern(floor_pattern) /// pattern
                                            .set_ambient(0.02)          /// ambient
                                            .set_specular(0.0)          /// specular
                                            .set_shininess(0.05));      /// shininess
        }

        /// --------------------------------------------------------------------
        /// earth-sphere
        auto earth_sphere = std::make_shared<RT::sphere>();
        {
                auto const texture_fname = RT::TEXTURE_ROOT + std::string("earth-8k-daymap.ppm");
                LOG_INFO("begin texturizing '%s'", texture_fname.c_str());

                auto maybe_earth_map_canvas = RT::canvas::load_from_file(texture_fname);
                ASSERT(maybe_earth_map_canvas.has_value());
                auto earth_map_canvas = maybe_earth_map_canvas.value();

                LOG_INFO("end texturizing '%s'", texture_fname.c_str());

                auto sp_01_earth_texture = std::make_shared<RT::uv_image>(earth_map_canvas);
                auto sp_01_earth_pattern =
                        std::make_shared<RT::texture_2d_pattern>(sp_01_earth_texture, RT::spherical_map);

                earth_sphere->transform(RT_XFORM::create_3d_scaling_matrix(3.75, 3.75, 3.75) *
                                        RT_XFORM::create_3d_translation_matrix(2.0, 1.3, 0.5) *
                                        RT_XFORM::create_rotz_matrix(RT::PI_BY_6F) *
                                        RT_XFORM::create_rotx_matrix(-RT::PI / 8.0) *
                                        RT_XFORM::create_roty_matrix(-RT::PI_BY_5F));

                earth_sphere->set_material(RT::material()
                                                   .set_pattern(sp_01_earth_pattern) /// pattern
                                                   .set_ambient(0.3)                 /// ambient
                                                   .set_diffuse(1.2)                 /// diffuse
                                                   .set_shininess(1.0)               /// shininess
                                                   .set_specular(0.1));              /// specular
        }

        /// --------------------------------------------------------------------
        /// moon-sphere
        auto fuzzy_sphere_01 = std::make_shared<RT::sphere>();
        {
                auto noisy_solid_pattern = std::make_shared<RT::perlin_noise_pattern>(
                        RT::color_white(),
                        std::default_random_engine::default_seed, /// seed
                        16);                                      /// octaves

                fuzzy_sphere_01->transform(RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5) *
                                           RT_XFORM::create_3d_translation_matrix(-15.0, 3.3, -35.0) *
                                           RT_XFORM::create_roty_matrix(RT::PI_BY_2F));

                fuzzy_sphere_01->set_material(RT::material()
                                                      .set_pattern(noisy_solid_pattern) /// pattern
                                                      .set_ambient(0.1)                 /// ambient
                                                      .set_diffuse(0.9)                 /// diffuse
                                                      .set_shininess(10.0)              /// shininess
                                                      .set_specular(0.1));              /// specular
        }

        /// --------------------------------------------------------------------
        /// red-sphere
        auto fuzzy_sphere_02 = std::make_shared<RT::sphere>(false);
        {
                auto fuzzy_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color(0.5, 0.0, 0.0),                 /// u
                        RT::color(1.0, 0.6, 0.6),                 /// v
                        std::default_random_engine::default_seed, /// seed
                        16);                                      /// octaves

                fuzzy_sphere_02->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0) *
                                           RT_XFORM::create_3d_translation_matrix(-3.5, 2.0, 3.0) *
                                           RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
                                           RT_XFORM::create_rotz_matrix(-RT::PI));

                fuzzy_sphere_02->set_material(RT::material()
                                                      .set_pattern(fuzzy_pattern) /// pattern
                                                      .set_ambient(0.1)           /// ambient
                                                      .set_diffuse(0.9)           /// diffuse
                                                      .set_shininess(10.0)        /// shininess
                                                      .set_specular(0.1));        /// specular
        }

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-1.0, 100.0, -100.0), RT::color_white());

        /// --------------------------------------------------------------------
        /// now create the world...
        auto world = RT::world();
        world.add(world_light_01);
        world.add(floor);
        world.add(fuzzy_sphere_01);
        world.add(fuzzy_sphere_02);
        world.add(earth_sphere);

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS, RT::canvas::Y_PIXELS, 0.8);
        auto look_from     = RT::create_point(-10.0, 2.0, -25.0);
        auto look_to       = RT::create_point(0.0, 1.1, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
