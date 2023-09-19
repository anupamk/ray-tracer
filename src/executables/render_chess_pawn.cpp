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
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/obj_file_loader.hpp"
#include "io/obj_parse_result.hpp"
#include "io/raytracer_renderer.hpp"
#include "io/world.hpp"
#include "patterns/blended_pattern.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/gradient_pattern.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/ring_pattern.hpp"
#include "patterns/solid_pattern.hpp"
#include "patterns/stock_materials.hpp"
#include "patterns/striped_pattern.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "patterns/uv_mapper.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cylinder.hpp"
#include "shapes/group.hpp"
#include "shapes/plane.hpp"
#include "shapes/shape_interface.hpp"
#include "shapes/sphere.hpp"
#include "utils/constants.hpp"
#include "utils/utils.hpp"

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
        auto dst_fname = "chess-pawn-checkered.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok render the scene
        Benchmark<> render_bm("MT render", 10);

        auto const rendered_canvas =
                render_bm.benchmark(RT::multi_threaded_renderer, RT::max_cores(), world, camera)[0];
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

        auto world_light_01 = RT::point_light(RT::create_point(-10.0, 13.0, -10.0), /// where
                                              RT::color::RGB(0xFF, 0xFF, 0xFF));    /// color

        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// a chequered board as well
        if (0) {
                auto chess_board = std::make_shared<RT::plane>();
                chess_board->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

                auto const chess_board_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color_black(), RT::color_white());

                auto const chess_board_material =
                        RT::material().set_pattern(chess_board_pattern).set_reflective(0.3);

                chess_board->set_material(chess_board_material);

                world.add(chess_board);
        }

        /// --------------------------------------------------------------------
        /// we are inside a cloudy sphere
        if (0) {
                auto s_01 = std::make_shared<RT::sphere>();
                world.add(s_01);

                auto cloud_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0x19, 0x19, 0x70),         /// color-a
                        RT::color::RGB(0xDC, 0xDC, 0xDC),         /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                auto sky_scale_factor = 1.2;
                cloud_pattern->transform(
                        RT_XFORM::create_3d_scaling_matrix(10.0, sky_scale_factor, sky_scale_factor));

                s_01->set_material(RT::material().set_pattern(cloud_pattern).set_refractive_index(1.4));

                s_01->transform(RT_XFORM::create_3d_scaling_matrix(50.0, 50.0, 50.0));
        }

        /// ------------------------------------------------------------
        /// and the floor is water
        if (0) {
                auto cloud_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0, 0, 110),                /// color-a
                        RT::color::RGB(100, 100, 100),            /// color-a
                        std::default_random_engine::default_seed, /// seed
                        16);

                auto floor = std::make_shared<RT::plane>(false);
                world.add(floor);
                auto floor_xform = RT_XFORM::create_3d_translation_matrix(0.0, -3.0, 0.0) *
                                   RT_XFORM::create_rotx_matrix(RT::PI / 13.0f);
                cloud_pattern->transform(RT_XFORM::create_3d_scaling_matrix(1.34, 0.6, -2.6));

                floor->set_material(RT::material()
                                            .set_pattern(cloud_pattern)
                                            .set_reflective(0.8)
                                            .set_refractive_index(RT::material::RI_WATER));
                floor->transform(floor_xform);
        }

        /// --------------------------------------------------------------------
        /// checkered floor
        {
                auto floor         = std::make_shared<RT::plane>();
                auto floor_mat     = RT::material().set_specular(0.0).set_reflective(0.4);
                auto floor_pattern = std::make_shared<RT::checkers_pattern>(RT::color::RGB(0x8b, 0x00, 0x00),
                                                                            RT::color::RGB(0xd2, 0xb4, 0x8c));
                floor_mat.set_pattern(floor_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = RT_XFORM::create_roty_matrix(0.31415) *
                                   RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0);
                floor->transform(floor_xform);

                world.add(floor);
        }

        /// ------------------------------------------------------------
        /// chess-pawn
        {
                RT::obj_file_parser chess_pawn_parser(RT::OBJ_ROOT + std::string("chess-pawn.obj"));
                auto chess_pawn_parse_result = chess_pawn_parser.parse();

                LOG_INFO("chess-pawn parsed, summary:'%s'", chess_pawn_parse_result.summarize().c_str());

                auto chess_pawn_material = RT::material()
                                                   .set_specular(1.0)
                                                   .set_transparency(1.0)
                                                   .set_reflective(0.9)
                                                   .set_shininess(300.0)
                                                   .set_ambient(0.1)
                                                   .set_diffuse(0.1)
                                                   .set_refractive_index(RT::material::RI_GLASS);

                /// ------------------------------------------------------------
                /// rotate the pawn by about 30° clockwise about the z-axis
                auto pawn_xform = RT_XFORM::create_rotz_matrix(-RT::PI_BY_6F) *
                                  RT_XFORM::create_3d_translation_matrix(-1.0, 1.5, 0.0);

                /// ------------------------------------------------------------
                /// add the default group and named groups into the world
                auto grp = chess_pawn_parse_result.default_group_ref();
                if (grp != nullptr) {
                        grp->set_cast_shadow(true);
                        grp->set_material(chess_pawn_material);
                        grp->transform(pawn_xform);
                        world.add(grp);
                }

                auto grp_list = chess_pawn_parse_result.group_list_cref();
                for (auto const& g_i : grp_list) {
                        g_i->set_cast_shadow(true);
                        g_i->set_material(chess_pawn_material);
                        g_i->transform(pawn_xform);
                        world.add(g_i);
                }

                LOG_INFO("chess-pawn loaded");
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01 = RT::camera(512 / 8, 512 / 8, RT::PI_BY_3F);
        auto xform     = RT_XFORM::create_view_transform(RT::create_point(0.0, 4.0, -10.0), /// look-from
                                                         RT::create_point(0.0, 2.0, 0.0),   /// look-to
                                                         RT::create_vector(0.0, 1.0, 0.0)); /// up-vector

        camera_01.transform(xform);

        return camera_01;
}
