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
#include "cylinder.hpp"
#include "gradient_pattern.hpp"
#include "gradient_perlin_noise_pattern.hpp"
#include "group.hpp"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "obj_file_loader.hpp"
#include "obj_parse_result.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "raytracer_renderer.hpp"
#include "ring_pattern.hpp"
#include "shape_interface.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "stock_materials.hpp"
#include "striped_pattern.hpp"
#include "texture_2d_pattern.hpp"
#include "tuple.hpp"
#include "uv_image_pattern.hpp"
#include "uv_mapper.hpp"
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
        auto dst_fname = "cessna.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok render the scene
        Benchmark<> render_bm("MT render");

        auto const rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
        LOG_INFO("rendering completed");

        rendered_canvas.write(dst_fname);
        LOG_INFO("image rendered to:'%s'", dst_fname);

        render_bm.show_stats();

        return 0;
}

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// now create the world...
        auto world = RT::world();

        auto world_light_01 =
                RT::point_light(RT::create_point(0.0, -20.5, 25.0), RT::color::RGB(0xFF, 0xFF, 0xFF));

        world.add(world_light_01);

        /// ------------------------------------------------------------
        /// we are inside a cloudy sphere
        {
                auto s_01 = std::make_shared<RT::sphere>();
                world.add(s_01);

                auto cloud_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0x19, 0x19, 0x70),         /// color-a
                        RT::color::RGB(0xDC, 0xDC, 0xDC),         /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);
                cloud_pattern->transform(RT_XFORM::create_3d_scaling_matrix(1.0, 1.0, 1.0));

                s_01->set_material(RT::material()
                                           .set_pattern(cloud_pattern)
                                           .set_reflective(0.08)
                                           .set_refractive_index(1.4));

                s_01->transform(RT_XFORM::create_3d_scaling_matrix(25.0, 30.0, 50.0));
        }

        /// ------------------------------------------------------------
        /// and the floor is water
        {
                auto cloud_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0, 0, 110),                /// color-a
                        RT::color::RGB(100, 100, 100),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                auto floor = std::make_shared<RT::plane>(false);
                world.add(floor);
                auto floor_xform = RT_XFORM::create_3d_translation_matrix(0.0, -3.0, 0.0) *
                                   RT_XFORM::create_rotx_matrix(RT::PI / 13.0f);
                cloud_pattern->transform(RT_XFORM::create_3d_scaling_matrix(-12.0, 0.6, -2.6));
                floor->set_material(RT::material()
                                            .set_pattern(cloud_pattern)
                                            .set_reflective(1.0)
                                            .set_refractive_index(RT::material::RI_WATER));
                floor->transform(floor_xform);
        }

        /// ------------------------------------------------------------
        /// cessna - 1
        {
                auto obj_fname = "/home/anupam/source-code/3d-models.git/3D-models/cessna.obj";

                RT::obj_file_parser teapot_parser(obj_fname);
                auto parse_result = teapot_parser.parse();

                LOG_INFO("cessna-01 parsed, summary:'%s'", parse_result.summarize().c_str());

                auto model_xform = RT_XFORM::create_3d_translation_matrix(-1.0, 3.2, 25.0) *
                                   RT_XFORM::create_roty_matrix(-RT::PI / 1.65f) *
                                   RT_XFORM::create_rotx_matrix(RT::PI / 22.0f) *
                                   RT_XFORM::create_rotz_matrix(-RT::PI_BY_6F) *
                                   RT_XFORM::create_3d_scaling_matrix(0.4, 0.4, 0.4);

                /// auto model_material = RT::create_material_matte(RT::color::RGB(0x8b, 0x8b, 0x83));
                auto model_material = RT::create_material_matte(RT::color::RGB(0x8b, 0x00, 0x00));
                model_material.set_reflective(0.2);

                /// ------------------------------------------------------------
                /// add default group and named groups into the world
                auto grp = parse_result.default_group_cref();
                if (grp != nullptr) {
                        world.add(grp);
                        grp->transform(model_xform);
                        grp->set_material(model_material);
                }

                auto grp_list = parse_result.group_list_cref();
                for (auto const& g_i : grp_list) {
                        world.add(g_i);
                        g_i->transform(model_xform);
                        g_i->set_material(model_material);
                }

                LOG_INFO("cessna-01 loaded");
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, 2.0);
        auto look_from     = RT::create_point(0.0, 0.2, -4.5);
        auto look_to       = RT::create_point(0.0, -0.5, 100.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}