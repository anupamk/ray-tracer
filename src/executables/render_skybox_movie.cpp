/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <string>

/// system includes
#include <string.h>

#include "common/include/assert_utils.h"
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/cube_map_texture.hpp"
#include "patterns/material.hpp"
#include "patterns/striped_pattern.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cube.hpp"
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
static RT::camera create_camera();

/// ----------------------------------------------------------------------------
/// a global cube so that we load textures only once.
auto cube_global = std::make_shared<RT::cube>(false);

int main(int argc, char** argv)
{
        auto camera = create_camera();
        auto world  = RT::world();

        /// --------------------------------------------------------------------
        /// light up the world
        auto const world_light_01 =
                RT::point_light(RT::create_point(0.0, 100.0, -100.0), RT::color(1.0, 1.0, 1.0));
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// a reflective floor
        auto floor = std::make_shared<RT::plane>();
        {
                auto floor_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color(0.35, 0.35, 0.35),  /// color-a
                                                               RT::color(0.65, 0.65, 0.65)); /// color-b

                floor->transform(RT_XFORM::create_roty_matrix(1.5708));

                floor->set_material(RT::material()
                                            .set_pattern(floor_pattern) /// pattern
                                            .set_ambient(0.02)          /// ambient
                                            .set_specular(0.0)          /// specular
                                            .set_reflective(0.5)        /// reflectiveness
                                            .set_shininess(0.05));      /// shininess

                world.add(floor);
        }

        /// 03. west-wall
        auto west_wall = std::make_shared<RT::plane>();
        {
                auto wall_pattern =
                        std::make_shared<RT::striped_pattern>(RT::color(0.45, 0.45, 0.45),  /// color-a
                                                              RT::color(0.55, 0.55, 0.55)); /// color-b

                auto wall_material = RT::material()             /// canonical-material
                                             .set_ambient(0.0)  /// with
                                             .set_diffuse(0.4)  /// special
                                             .set_specular(0.0) /// properties
                                             .set_reflective(0.3)
                                             .set_pattern(wall_pattern);

                west_wall->set_material(wall_material);

                auto west_wall_xform = RT_XFORM::create_3d_translation_matrix(-5.0, 0.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(1.5708) * /// rotate-to-vertical
                                       RT_XFORM::create_roty_matrix(1.5708);  /// orient-texture

                west_wall->transform(west_wall_xform);

                world.add(west_wall);
        }

        /// --------------------------------------------------------------------
        /// setup the cube and its properties
        auto cube_surface_properties = RT::material().set_diffuse(0.0).set_specular(0.0).set_ambient(1.0);
        {
                auto const textureize = [](std::string const& fname) -> std::shared_ptr<RT::uv_image> {
                        LOG_INFO("texturizing '%s'", fname.c_str());

                        auto canvas_maybe = RT::canvas::load_from_file(fname);
                        ASSERT(canvas_maybe.has_value());

                        auto canvas_value   = canvas_maybe.value();
                        auto canvas_texture = std::make_shared<RT::uv_image>(canvas_value);

                        return canvas_texture;
                };

                auto const left_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/negx.ppm"));
                auto const front_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/posz.ppm"));
                auto const right_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/posx.ppm"));
                auto const back_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/negz.ppm"));
                auto const up_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/posy.ppm"));
                auto const down_face_texture =
                        textureize(RT::TEXTURE_ROOT + std::string("skybox/chapel/negy.ppm"));

                // clang-format off
                auto const cube_pattern = std::make_shared<RT::cube_texture>(left_face_texture,
                                                                             front_face_texture,
                                                                             right_face_texture,
                                                                             back_face_texture,
                                                                             up_face_texture,
                                                                             down_face_texture);

                cube_surface_properties.set_pattern(cube_pattern);
                // clang-format on
        }

        /// --------------------------------------------------------------------
        /// initial cube orientation and position
        {
                cube_global->transform(RT_XFORM::create_3d_translation_matrix(2.0, 4.0, 0.0) *
                                       RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0) *
                                       RT_XFORM::create_rotx_matrix(0.0) * RT_XFORM::create_roty_matrix(0.0));

                cube_global->set_material(cube_surface_properties);

                world.add(cube_global);
        }

        /// --------------------------------------------------------------------
        /// benchmark the render with 'num_iterations' renders performed, and
        /// throwing away the results from 'num_discards' of them
        benchmark_t<> render_bm("MT render");

        constexpr uint32_t max_images = 500;
        constexpr float rot_xy_step   = (2 * RT::PI) / (1.0f * max_images);
        auto movie_image_fname        = "/tmp/skybox-image-";

        /// --------------------------------------------------------------------
        /// step-1: render a bunch of images
        for (uint32_t i = 0; i < max_images; i++) {
                std::stringstream ss("");
                ss << movie_image_fname << std::setfill('0') << std::setw(5) << i << ".ppm";

                auto const dst_fname = ss.str();

                /// --------------------------------------------------------------------
                /// ok camera, render the scene
                auto render_params         = RT::config_render_params().antialias(true);
                auto const rendered_canvas = camera.render(world, render_params);
                rendered_canvas.write(dst_fname);

                /// --------------------------------------------------------------------
                /// show what we got
                LOG_INFO("rendered:'%s' %05d / %05d", dst_fname.c_str(), i, max_images);

                /// ------------------------------------------------------------
                /// next round orientation
                cube_global->transform(RT_XFORM::create_3d_translation_matrix(0.0, 4.0, 0.0) *
                                       RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0) *
                                       RT_XFORM::create_rotx_matrix(0.0 + i * rot_xy_step) *
                                       RT_XFORM::create_roty_matrix(0.0 - i * rot_xy_step));
        }

        /// --------------------------------------------------------------------
        /// step-2: create a movie from the set of images rendered in step-1
        {
                auto const movie_name      = "skybox-movie.mp4";
                auto const movie_framerate = 25;

                /// ------------------------------------------------------------
                /// command-line:
                ///    "ffmpeg -y -framerate 25 -i /tmp/skybox-image-%5d.ppm skybox-movie.mp4
                std::stringstream create_movie_cmdline_ss;
                create_movie_cmdline_ss << "ffmpeg"
                                        << " -y " /// overwrite without asking
                                        << " -framerate " << movie_framerate << " -i " << movie_image_fname
                                        << "%5d.ppm"
                                        << " " << movie_name;

                auto const create_movie_cmdline = create_movie_cmdline_ss.str();

                LOG_INFO("creating the movie '%s' from source images '%s-*.ppm', cmdline: '%s'", movie_name,
                         movie_image_fname, create_movie_cmdline.c_str());

                errno             = 0;
                auto const retval = system(create_movie_cmdline.c_str());
                if (retval != 0) {
                        LOG_ERROR("failed to create movie, cmdline: '%s', errno/reason: '%d'/'%s'",
                                  create_movie_cmdline.c_str(), errno, strerror(errno));
                }
        }

        return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01 = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, 1.2);
        auto look_from = RT::create_point(-1.0, 2.0, -20.0);
        /// auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        /// auto look_from = RT::create_point(-10.0, 2.0, -25.0);
        auto look_to = RT::create_point(0.0, 1.1, 0.0);

        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
