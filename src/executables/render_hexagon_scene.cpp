/*
 * this program implements a single/multi threaded world-rendering through a
 * camera.
 **/

/// c++ includes
#include <cstdint>
#include <memory>

#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/solid_pattern.hpp"
#include "patterns/stock_materials.hpp"
#include "patterns/striped_pattern.hpp"
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
#include "common/include/logging.h"

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

/// hexagon primitives
static std::shared_ptr<RT::sphere> create_hexagon_corner();
static std::shared_ptr<RT::cylinder> create_hexagon_edge();
static std::shared_ptr<RT::group> create_hexagon_side();
static std::shared_ptr<RT::group> create_hexagon();

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_camera();
        auto dst_fname = "render-hexagon.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok camera, render the scene
        auto const rendered_canvas = camera.render(world);
        rendered_canvas.write(dst_fname);

        return 0;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a hexagon corner
static std::shared_ptr<RT::sphere> create_hexagon_corner()
{
        auto corner       = std::make_shared<RT::sphere>(false);
        auto corner_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, -1.0) *
                            RT_XFORM::create_3d_scaling_matrix(0.25, 0.25, 0.25);

        corner->transform(corner_xform);

        return corner;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a hexagon edge
static std::shared_ptr<RT::cylinder> create_hexagon_edge()
{
        auto edge       = std::make_shared<RT::cylinder>(false, 0.0, 1.0, false);
        auto edge_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, -1.0) *
                          RT_XFORM::create_roty_matrix(-RT::PI_BY_6F) *
                          RT_XFORM::create_rotz_matrix(-RT::PI_BY_2F) *
                          RT_XFORM::create_3d_scaling_matrix(0.25, 1.0, 0.25);

        edge->transform(edge_xform);

        return edge;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a hexagon side
static std::shared_ptr<RT::group> create_hexagon_side()
{
        auto side = std::make_shared<RT::group>();
        side->add_child(create_hexagon_corner());
        side->add_child(create_hexagon_edge());

        return side;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a hexagon
static std::shared_ptr<RT::group> create_hexagon()
{
        auto hexagon = std::make_shared<RT::group>();

        for (uint8_t i = 0; i < 6; i++) {
                auto side_i = create_hexagon_side();
                side_i->transform(RT_XFORM::create_roty_matrix(i * RT::PI_BY_3F));
                hexagon->add_child(side_i);
        }

        return hexagon;
}

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        /// now create the world...
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        {
                auto l_01 = RT::point_light(RT::create_point(-4.9, 4.9, -1.0), RT::color_white());
                world.add(l_01);
        }

        /// --------------------------------------------------------------------
        /// axes + origin, so that we get the bearings right
        {
                /// ------------------------------------------------------------
                /// 'tiny' sphere marking origin
                auto origin_sphere = std::make_shared<RT::sphere>(false);
                world.add(origin_sphere);

                auto origin_sphere_scaler = RT_XFORM::create_3d_scaling_matrix(0.1, 0.1, 0.1);
                origin_sphere->transform(origin_sphere_scaler);

                origin_sphere->set_material(RT::create_material_matte(RT::color::RGB(0xff, 0x45, 0x00)));
        }

        /// --------------------------------------------------------------------
        /// a hexagon with some material
        {
                auto hex_01 = create_hexagon();
                world.add(hex_01);

                hex_01->transform(RT_XFORM::create_3d_translation_matrix(4.0, -2.1, 4.0) *
                                  RT_XFORM::create_rotx_matrix(-RT::PI_BY_6F) *
                                  RT_XFORM::create_rotz_matrix(RT::PI_BY_6F) *
                                  RT_XFORM::create_roty_matrix(-RT::PI / 2.5));

                auto sp_02_pattern = std::make_shared<RT::gradient_perlin_noise_pattern>(
                        RT::color::RGB(0x19, 0x19, 0x70), RT::color::RGB(0xFF, 0xFF, 0xFF));

                auto hexa_material = RT::material().set_pattern(sp_02_pattern);
                hex_01->set_material(hexa_material);
        }

        /// --------------------------------------------------------------------
        /// a checkered floor
        {
                auto floor = std::make_shared<RT::plane>();
                world.add(floor);

                auto floor_mat = RT::material().set_specular(0.0).set_reflective(0.4);
                auto floor_pattern =
                        std::make_shared<RT::checkers_pattern>(RT::color(0.35, 0.35, 0.35),  /// color-a
                                                               RT::color(0.65, 0.65, 0.65)); /// color-b
                floor_mat.set_pattern(floor_pattern);
                floor->set_material(floor_mat);

                auto floor_xform = RT_XFORM::create_roty_matrix(0.31415) *
                                   RT_XFORM::create_3d_translation_matrix(0.0, -3.0, 0.0);
                floor->transform(floor_xform);
        }

        /// 02. the ceiling
        auto ceiling = std::make_shared<RT::plane>();
        {
                world.add(ceiling);

                auto ceiling_mat     = RT::material().set_ambient(0.3).set_specular(0.0);
                auto ceiling_pattern = std::make_shared<RT::solid_pattern>(RT::color(0.8, 0.8, 0.8));
                ceiling_mat.set_pattern(ceiling_pattern);
                ceiling->set_material(ceiling_mat);

                auto ceiling_xform = RT_XFORM::create_3d_translation_matrix(0.0, 5.0, 0.0);
                ceiling->transform(ceiling_xform);
        }

        /// --------------------------------------------------------------------
        /// wall material
        auto wall_pattern = std::make_shared<RT::striped_pattern>(RT::color(0.45, 0.45, 0.45),  /// color-a
                                                                  RT::color(0.55, 0.55, 0.55)); /// color-b
        auto wall_xform =
                RT_XFORM::create_3d_scaling_matrix(0.25, 0.25, 0.25) * RT_XFORM::create_roty_matrix(1.5708);

        wall_pattern->transform(wall_xform);

        auto wall_material = RT::material()             /// canonical-material
                                     .set_ambient(0.0)  /// with
                                     .set_diffuse(0.4)  /// special
                                     .set_specular(0.0) /// properties
                                     .set_reflective(0.3)
                                     .set_pattern(wall_pattern);

        /// 03. west-wall
        auto west_wall = std::make_shared<RT::plane>();
        {
                world.add(west_wall);

                west_wall->set_material(wall_material);

                auto west_wall_xform = RT_XFORM::create_3d_translation_matrix(-5.0, 0.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(1.5708) * /// rotate-to-vertical
                                       RT_XFORM::create_roty_matrix(1.5708);  /// orient-texture
                west_wall->transform(west_wall_xform);
        }

        /// 04. east-wall
        auto east_wall = std::make_shared<RT::plane>();
        {
                world.add(east_wall);

                east_wall->set_material(wall_material);

                auto east_wall_xform = RT_XFORM::create_3d_translation_matrix(5.0, 0.0, 0.0) *
                                       RT_XFORM::create_rotz_matrix(1.5708) * /// rotate-to-vertical
                                       RT_XFORM::create_roty_matrix(1.5708);  /// orient-texture

                east_wall->transform(east_wall_xform);
        }

        /// 05. north-wall
        auto north_wall = std::make_shared<RT::plane>();
        {
                world.add(north_wall);

                north_wall->set_material(wall_material);

                auto north_wall_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, 5.0) *
                                        RT_XFORM::create_rotx_matrix(1.5708); /// rotate-to-vertical

                north_wall->transform(north_wall_xform);
        }

        /// 05. south-wall
        auto south_wall = std::make_shared<RT::plane>();
        {
                world.add(south_wall);

                south_wall->set_material(wall_material);

                auto south_wall_xform = RT_XFORM::create_3d_translation_matrix(0.0, 0.0, -5.0) *
                                        RT_XFORM::create_rotx_matrix(1.5708); /// rotate-to-vertical

                south_wall->transform(south_wall_xform);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1600, 1200, 1.152);
        auto look_from     = RT::create_point(-4.0, 0.5, -4.9);
        auto look_to       = RT::create_point(-0.6, -1.0, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
