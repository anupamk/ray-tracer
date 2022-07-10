/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
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
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_camera();
        auto dst_fname = "nested-glass-spheres.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok render the scene
        Benchmark<> render_bm("ST render");

        auto rendered_canvas = render_bm.benchmark(RT::single_threaded_renderer, world, camera)[0];
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
static RT::world create_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(2.0, 10.0, -5.0), RT::color(0.9, 0.9, 0.9));
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// wall
        {
                auto wall = std::make_shared<RT::plane>();
                world.add(wall);

                auto wall_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 10.0) *
                                  RT_XFORM::create_rotx_matrix(RT::PI_BY_2F);

                wall->transform(wall_xform);

                auto wall_pattern = std::make_shared<RT::checkers_pattern>(RT::color(0.15, 0.15, 0.15),
                                                                           RT::color(0.85, 0.85, 0.85));
                wall->set_material(RT::material()
                                           .set_pattern(wall_pattern)
                                           .set_ambient(0.8)
                                           .set_diffuse(0.2)
                                           .set_specular(0.0));
        }

        /// --------------------------------------------------------------------
        /// glass ball with a hollow sphere
        {
                auto s_01 = std::make_shared<RT::sphere>(false);
                world.add(s_01);

                s_01->set_material(
                        RT::material()
                                .set_pattern(std::make_shared<RT::solid_pattern>(RT::color(1.0, 1.0, 1.0)))
                                .set_ambient(0.0)
                                .set_diffuse(0.0)
                                .set_specular(0.9)
                                .set_shininess(300)
                                .set_reflective(0.9)
                                .set_transparency(0.9)
                                .set_refractive_index(RT::material::RI_GLASS));

                /// ------------------------------------------------------------
                /// hollow center
                auto s_02 = std::make_shared<RT::sphere>(false);
                world.add(s_02);

                s_02->transform(RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5));

                s_02->set_material(
                        RT::material()
                                .set_pattern(std::make_shared<RT::solid_pattern>(RT::color(1.0, 1.0, 1.0)))
                                .set_ambient(0.0)
                                .set_diffuse(0.0)
                                .set_specular(0.9)
                                .set_shininess(300)
                                .set_reflective(0.9)
                                .set_transparency(0.9)
                                .set_refractive_index(RT::material::RI_AIR));
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, 0.45);
        auto look_from     = RT::create_point(0.0, 0.0, -5.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
