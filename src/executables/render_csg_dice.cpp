/*
 * description
 *    this program renders a couple of dice(s?) using
 **/

/// c++ includes
#include <algorithm>
#include <cstdint>
#include <iterator>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

/// our includes
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/stock_materials.hpp"
#include "primitives/color.hpp"
#include "primitives/color_pallette.hpp"
#include "primitives/matrix.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/csg.hpp"
#include "shapes/cube.hpp"
#include "shapes/group.hpp"
#include "shapes/plane.hpp"
#include "shapes/shape_interface.hpp"
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
static std::shared_ptr<RT::shape_interface> create_a_dice(RT::material const& dice_material,
                                                          RT::material const& pip_material);

static std::vector<std::shared_ptr<RT::shape_interface>> create_dices(uint32_t num_dices);

int main(int argc, char** argv)
{
        auto world     = create_world();
        auto camera    = create_camera();
        auto dst_fname = "render-csg-dice.ppm";

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

/// ----------------------------------------------------------------------------
/// create dice-body. a dice is a cube whose corners are 'cut' by a slightly
/// larger sphere.
///
/// make it so.
static std::shared_ptr<RT::shape_interface> create_dice_body(RT::material const& mat)
{
        auto dice_cube = std::make_shared<RT::cube>(false);
        dice_cube->set_material(mat);

        auto cut_sphere = std::make_shared<RT::sphere>(false);
        cut_sphere->set_material(mat);
        cut_sphere->transform(RT_XFORM::create_3d_scaling_matrix(1.6, 1.6, 1.6));

        auto csg_op_intersection = std::make_shared<RT::csg_intersection>();
        return RT::csg_shape::create_csg(dice_cube, csg_op_intersection, cut_sphere);
}

/// ----------------------------------------------------------------------------
/// create a 'pip' for the dice face. a single 'pip' is nothing more than a
/// translated+scaled sphere.
///
/// multiple 'pips' are a group of single pips
static std::shared_ptr<RT::shape_interface> create_a_dice_pip(float i, float j, RT::material const& mat)
{
        auto pip_sphere = std::make_shared<RT::sphere>(false);
        pip_sphere->set_material(mat);

        auto pip_xformer = RT_XFORM::create_3d_translation_matrix(0.5 * i, 1.0, 0.5 * j) *
                           RT_XFORM::create_3d_scaling_matrix(0.2, 1.0, 0.2);

        pip_sphere->transform(pip_xformer);

        return pip_sphere;
}

static std::shared_ptr<RT::shape_interface> create_dice_face_1(RT::material const& pip_mat)
{
        return create_a_dice_pip(0.0, 0.0, pip_mat);
}

static std::shared_ptr<RT::shape_interface> create_dice_face_2(RT::material const& pip_mat)
{
        auto pips = std::make_shared<RT::group>(false);
        pips->add_child(create_a_dice_pip(-0.8, -0.8, pip_mat));
        pips->add_child(create_a_dice_pip(0.8, 0.8, pip_mat));

        return pips;
}

static std::shared_ptr<RT::shape_interface> create_dice_face_3(RT::material const& pip_mat)
{
        auto pips = std::make_shared<RT::group>(false);
        pips->add_child(create_a_dice_pip(0.0, 0.0, pip_mat));
        pips->add_child(create_a_dice_pip(-1.0, -1.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, 1.0, pip_mat));

        return pips;
}

static std::shared_ptr<RT::shape_interface> create_dice_face_4(RT::material const& pip_mat)
{
        auto pips = std::make_shared<RT::group>(false);
        pips->add_child(create_a_dice_pip(-0.8, -0.8, pip_mat));
        pips->add_child(create_a_dice_pip(-0.8, 0.8, pip_mat));
        pips->add_child(create_a_dice_pip(0.8, -0.8, pip_mat));
        pips->add_child(create_a_dice_pip(0.8, 0.8, pip_mat));

        return pips;
}

static std::shared_ptr<RT::shape_interface> create_dice_face_5(RT::material const& pip_mat)
{
        auto pips = std::make_shared<RT::group>(false);

        pips->add_child(create_a_dice_pip(0.0, 0.0, pip_mat));
        pips->add_child(create_a_dice_pip(-1.0, -1.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, -1.0, pip_mat));
        pips->add_child(create_a_dice_pip(-1.0, 1.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, 1.0, pip_mat));

        return pips;
}

static std::shared_ptr<RT::shape_interface> create_dice_face_6(RT::material const& pip_mat)
{
        auto pips = std::make_shared<RT::group>(false);

        pips->add_child(create_a_dice_pip(-1.0, -1.0, pip_mat));
        pips->add_child(create_a_dice_pip(-1.0, 0.0, pip_mat));
        pips->add_child(create_a_dice_pip(-1.0, 1.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, -1.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, 0.0, pip_mat));
        pips->add_child(create_a_dice_pip(1.0, 1.0, pip_mat));

        return pips;
}

/// ----------------------------------------------------------------------------
/// create a dice of a specific color
static std::shared_ptr<RT::shape_interface> create_a_dice(RT::material const& dice_material,
                                                          RT::material const& pip_material)
{
        /// --------------------------------------------------------------------
        /// dice body + pips
        auto db          = create_dice_body(dice_material);
        auto csg_diff_op = std::make_shared<RT::csg_difference>();

        /// ------------------------------------------------------------
        /// pip-1, and pip-6 are adjacent
        {
                auto pip_1 = create_dice_face_1(pip_material);
                db         = RT::csg_shape::create_csg(db, csg_diff_op, pip_1, false);

                auto pip_6 = create_dice_face_6(pip_material);
                pip_6->transform(RT_XFORM::create_3d_scaling_matrix(1.0, -1.0, 1.0));
                db = RT::csg_shape::create_csg(db, csg_diff_op, pip_6, false);
        }

        /// ------------------------------------------------------------
        /// pip-2, and pip-5 are adjacent
        {
                auto pip_2 = create_dice_face_2(pip_material);
                pip_2->transform(RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));
                db = RT::csg_shape::create_csg(db, csg_diff_op, pip_2, false);

                auto pip_5 = create_dice_face_5(pip_material);
                pip_5->transform(RT_XFORM::create_rotx_matrix(-RT::PI_BY_2F));
                db = RT::csg_shape::create_csg(db, csg_diff_op, pip_5, false);
        }

        /// --------------------------------------------------------------------
        /// pip-3, and pip-4 are adjacent
        {
                auto pip_3 = create_dice_face_3(pip_material);
                pip_3->transform(RT_XFORM::create_rotz_matrix(RT::PI_BY_2F));
                db = RT::csg_shape::create_csg(db, csg_diff_op, pip_3, false);

                auto pip_4 = create_dice_face_4(pip_material);
                pip_4->transform(RT_XFORM::create_rotz_matrix(-RT::PI_BY_2F));
                db = RT::csg_shape::create_csg(db, csg_diff_op, pip_4, false);
        }

        return db;
}

/// ----------------------------------------------------------------------------
/// create a bunch of dices with random colors for body and pip, randomly placed
/// and oriented etc. etc.
static std::vector<std::shared_ptr<RT::shape_interface>> create_dices(uint32_t num_dices)
{
        std::vector<std::shared_ptr<RT::shape_interface>> dices;

        std::random_device rd;
        std::mt19937 gen(rd());

        std::uniform_int_distribution<> rnd_color_range(0, RT::color_pallette.size());
        std::uniform_real_distribution<> rnd_scale_range(0.01, 0.8);
        std::uniform_real_distribution<> rnd_rotx_range(-RT::PI, RT::PI);

        std::uniform_real_distribution<> rnd_posx_range(-15.0, 15.0);
        std::uniform_real_distribution<> rnd_posy_range(-1.5, 7.5);
        std::uniform_real_distribution<> rnd_posz_range(-13.0, 12.0);

        for (uint32_t i = 0; i < num_dices; i++) {
                auto dice_color_kv         = std::next(std::begin(RT::color_pallette), rnd_color_range(gen));
                RT::material dice_material = RT::create_material_matte(dice_color_kv->second);
                dice_material.set_diffuse(1.0);

                auto pip_color_kv         = std::next(std::begin(RT::color_pallette), rnd_color_range(gen));
                RT::material pip_material = RT::create_material_matte(pip_color_kv->second);
                pip_material.set_diffuse(1.0);

                auto the_dice = create_a_dice(dice_material, pip_material);

                auto dice_scale = rnd_scale_range(gen);
                auto dice_pos_x = rnd_posx_range(gen);
                auto dice_pos_y = rnd_posy_range(gen);
                auto dice_pos_z = rnd_posz_range(gen);
                auto dice_rot_x = rnd_rotx_range(gen);

                the_dice->transform(
                        RT_XFORM::create_3d_translation_matrix(dice_pos_x, dice_pos_y, dice_pos_z) *
                        RT_XFORM::create_rotx_matrix(dice_rot_x) *
                        RT_XFORM::create_3d_scaling_matrix(dice_scale, dice_scale, dice_scale));

                dices.push_back(the_dice);
        }

        LOG_INFO("total dices:'%zu'", dices.size());

        return dices;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a world which will be rendered
static RT::world create_world()
{
        auto world = RT::world();

        auto world_light_01 = RT::point_light(RT::create_point(-9.0, 10.0, -20.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// floor
        {
                auto floor_pattern = std::make_shared<RT::checkers_pattern>(RT::color(0.9, 0.9, 0.9),
                                                                            RT::color(0.75, 0.75, 0.75));

                auto floor_material = RT::material().set_diffuse(1.0).set_specular(0.0);
                floor_material.set_pattern(floor_pattern);

                auto floor = std::make_shared<RT::plane>();
                floor->set_material(floor_material);
                floor->transform(RT_XFORM::create_3d_translation_matrix(0.0, -3.0, 2.0) *
                                 RT_XFORM::create_roty_matrix(-RT::PI_BY_2F));

                world.add(floor);
        }

        /// --------------------------------------------------------------------
        /// dices
        {
                auto dices = create_dices(192);
                std::for_each(dices.begin(), dices.end(),
                              [&world](std::shared_ptr<RT::shape_interface> const& dice) {
                                      world.add(dice);
                              });
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(RT::canvas::X_PIXELS_2K, RT::canvas::Y_PIXELS_2K, RT::PI / 1.2);
        auto look_from     = RT::create_point(-2.6, 0.5, -15.9);
        auto look_to       = RT::create_point(-0.6, 1.0, -0.8);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
