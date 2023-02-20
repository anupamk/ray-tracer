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
#include "common/include/logging.h"
#include "io/camera.hpp"
#include "io/raytracer_renderer.hpp"
#include "io/world.hpp"
#include "patterns/blended_pattern.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/gradient_pattern.hpp"
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/gradient_ring_pattern.hpp"
#include "patterns/material.hpp"
#include "patterns/perlin_noise_pattern.hpp"
#include "patterns/ring_pattern.hpp"
#include "patterns/solid_pattern.hpp"
#include "patterns/stock_materials.hpp"
#include "patterns/striped_pattern.hpp"
#include "patterns/texture_2d_pattern.hpp"
#include "patterns/uv_checkers.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "patterns/uv_mapper.hpp"
#include "patterns/uv_noise_texture.hpp"
#include "primitives/color.hpp"
#include "primitives/matrix_transformations.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/cone.hpp"
#include "shapes/cylinder.hpp"
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
RT::world create_cone_world();
RT::camera create_cone_world_camera();

int main(int argc, char** argv)
{
        auto world     = create_cone_world();
        auto camera    = create_cone_world_camera();
        auto dst_fname = "cone-scene.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// ok render the scene
        Benchmark<> render_bm("MT render");
        auto const rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
        rendered_canvas.write(dst_fname);
        render_bm.show_stats();

        return 0;
}

/*
 * only file specific functions from this point onwards
 **/

/// ----------------------------------------------------------------------------
/// this function is called to create a world which contains just cylinders as
/// primitive shapes
RT::world create_cone_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-4.0, 5.0, -20.0), RT::color_white());
        world.add(world_light_01);

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_02 =
                RT::point_light(RT::create_point(5.0, 5.0, 20.0), RT::color::RGB(0x9e, 0x9e, 0x9e));
        world.add(world_light_02);

        /// --------------------------------------------------------------------
        /// axes + origin, so that we get the bearings right
        {
                auto axes_xform    = RT_XFORM::create_3d_scaling_matrix(0.01, 0.01, 0.01);
                auto axes_pattern  = std::make_shared<RT::solid_pattern>(RT::color::RGB(0x4f, 0x4f, 0x4f));
                auto axes_material = RT::material()
                                             .set_ambient(0.0)
                                             .set_diffuse(1.0)
                                             .set_reflective(0.0)
                                             .set_transparency(0.0)
                                             .set_refractive_index(0.0)
                                             .set_pattern(axes_pattern);

                /// ------------------------------------------------------------
                /// x-axis
                auto x_axis = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(x_axis);

                x_axis->transform(axes_xform * RT_XFORM::create_rotz_matrix(RT::PI_BY_2F));
                x_axis->set_material(axes_material);

                /// ------------------------------------------------------------
                /// y-axis
                auto y_axis = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(y_axis);

                y_axis->transform(axes_xform);
                y_axis->set_material(axes_material);

                /// ------------------------------------------------------------
                /// z-axis
                auto z_axes = std::make_shared<RT::cylinder>(false, -RT::INF, RT::INF, false);
                world.add(z_axes);

                z_axes->transform(axes_xform * RT_XFORM::create_rotx_matrix(RT::PI_BY_2F));
                z_axes->set_material(axes_material);
        }

        /// --------------------------------------------------------------------
        /// create a cone on the y-axis and place a sphere on top of it
        {
                auto c1 = std::make_shared<RT::cone>(false, -1.0, 0.0, true);
                world.add(c1);

                c1->set_material(RT::create_material_matte(RT::color(0.6, 0.0, 0.0)));
                c1->transform(RT_XFORM::create_3d_translation_matrix(0.0, 1.0, 0.0));

                auto s1 = std::make_shared<RT::sphere>(false);
                world.add(s1);

                s1->set_material(RT::create_material_matte(RT::color(0.6, 0.6, 0.6)));
                s1->transform(RT_XFORM::create_3d_translation_matrix(0.0, 1.4, 0.0) *
                              RT_XFORM::create_3d_scaling_matrix(0.4, 0.4, 0.4));
        }

        /// --------------------------------------------------------------------
        /// apply a (noisy) texture on a cone
        {
                auto c2 = std::make_shared<RT::cone>(false, -1.0, 0.0, false);
                world.add(c2);

                /// ------------------------------------------------------------
                /// now generate a 640 x 512 pixel noisy texture and map that
                /// onto the cone i.e. c2
                auto noisy_texture = std::make_shared<RT::uv_image>(
                        RT::generate_noisy_texture(640,                                /// x-dim
                                                   512,                                /// y-dim
                                                   RT::color::RGB(0xff, 0x00, 0xff),   /// start-color
                                                   RT::color::RGB(0xff, 0xff, 0xff))); /// end-color

                auto c2_pattern = std::make_shared<RT::texture_2d_pattern>(noisy_texture, RT::conical_map);
                auto c2_pattern_transform = RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0) *
                                            RT_XFORM::create_3d_scaling_matrix(2.0, 1.0, 1.0);
                c2_pattern->transform(c2_pattern_transform);

                c2->set_material(RT::material().set_pattern(c2_pattern));
                c2->transform(RT_XFORM::create_3d_translation_matrix(0.0, -1.0, 0.0) *
                              RT_XFORM::create_3d_scaling_matrix(0.8, 2.0, 0.8));
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_cylinder_world(...)
RT::camera create_cone_world_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_2F);
        auto look_from     = RT::create_point(-2.0, 1.0, -5.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
