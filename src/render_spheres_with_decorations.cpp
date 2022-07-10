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
#include "blended_pattern.hpp"
#include "camera.hpp"
#include "checkers_pattern.hpp"
#include "color.hpp"
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "constants.hpp"
#include "cube.hpp"
#include "cylinder.hpp"
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
#include "stock_materials.hpp"
#include "striped_pattern.hpp"
#include "texture_2d_pattern.hpp"
#include "tuple.hpp"
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
RT::world create_shapes_world();
RT::camera create_shapes_world_camera();

/// ----------------------------------------------------------------------------
/// setup global random number generation primitives
static std::mt19937 rnd_gen(time(nullptr));
static std::uniform_int_distribution<unsigned> distrib(std::numeric_limits<unsigned>::min(),
                                                       std::numeric_limits<unsigned>::max());

int main(int argc, char** argv)
{
        auto world     = create_shapes_world();
        auto camera    = create_shapes_world_camera();
        auto dst_fname = "decorated-spheres.ppm";

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
RT::world create_shapes_world()
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

                /// ------------------------------------------------------------
                /// 'tiny' sphere marking origin
                auto origin_sphere = std::make_shared<RT::sphere>(false);
                world.add(origin_sphere);

                auto origin_sphere_scaler = RT_XFORM::create_3d_scaling_matrix(0.1, 0.1, 0.1);
                origin_sphere->transform(origin_sphere_scaler);

                origin_sphere->set_material(RT::create_material_matte(RT::color::RGB(0xff, 0x45, 0x00)));
        }

        /// --------------------------------------------------------------------
        {
                auto s1 = std::make_shared<RT::sphere>();
                world.add(s1);

                s1->transform(RT_XFORM::create_3d_translation_matrix(2.0, 0.0, 2.0) *
                              RT_XFORM::create_3d_scaling_matrix(0.3, 0.3, 0.3));
        }

        /// --------------------------------------------------------------------
        /// another sphere i.e. s2, notice that transformation order is
        /// reversed from that of s1
        {
                auto s2 = std::make_shared<RT::sphere>();
                world.add(s2);

                auto s2_pattern = std::make_shared<RT::solid_pattern>(RT::color::RGB(0xd8, 0xbf, 0xd8));
                s2->set_material(RT::material().set_pattern(s2_pattern));

                /// ------------------------------------------------------------
                /// scale -> translate
                s2->transform(RT_XFORM::create_3d_scaling_matrix(0.3, 0.3, 0.3) *
                              RT_XFORM::create_3d_translation_matrix(2.0, 0.0, 2.0));
        }

        /// --------------------------------------------------------------------
        /// sphere with striped pattern
        {
                auto s3 = std::make_shared<RT::sphere>();
                world.add(s3);

                auto s3_transforms = RT_XFORM::create_3d_translation_matrix(8.0, 0.0, 8.0) *
                                     RT_XFORM::create_roty_matrix(-RT::PI_BY_6F) *
                                     RT_XFORM::create_3d_scaling_matrix(1.5, 1.5, 1.5);

                s3->transform(s3_transforms);

                /// ------------------------------------------------------------
                /// note that all the patterns that we create here are defined
                /// within [0 .. 1]. but we want the pattern to repeat multiple
                /// times across the surface of s3.
                ///
                /// scale + rotate (optionally) accordingly
                auto s3_pattern = std::make_shared<RT::striped_pattern>(RT::color::RGB(0x8b, 0x00, 0x00),
                                                                        RT::color::RGB(0xd2, 0xb4, 0x8c));

                auto s3_pattern_transform = RT_XFORM::create_3d_scaling_matrix(0.1, 1.0, 1.0);
                s3_pattern->transform(s3_pattern_transform);

                auto s3_material = RT::material().set_pattern(s3_pattern);
                s3->set_material(s3_material);
        }

        /// --------------------------------------------------------------------
        /// sphere with gradient decorations
        {
                auto s4 = std::make_shared<RT::sphere>();
                world.add(s4);

                auto s4_transforms = RT_XFORM::create_3d_translation_matrix(4.0, 0.0, 15.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.5, 2.5, 2.5) *
                                     RT_XFORM::create_roty_matrix(RT::PI_BY_6F);

                s4->transform(s4_transforms);

                /// ------------------------------------------------------------
                /// couple of points:
                ///
                /// 1. the gradient pattern is designed to alternate every whole
                /// number i.e. at x = 0, 1, 2, 3 ...
                ///
                /// 2. moreover, we want to apply the pattern across the entire
                /// sphere once.
                ///
                /// so, from (1) and (2), it follows that we need to transform
                /// the pattern.
                ///
                /// now, a sphere (in object space) is centered at (0.0, 0.0,
                /// 0.0) and has a radius of 1.0. this implies that it spans
                /// (along the x-axes) from [-1.0 .. 1.0].
                ///
                /// therefore, we need to :
                ///     - translate the pattern to: (-1.0, 0.0, 0.0) and
                ///     - then scale it to: (2.0, 1.0, 1.0)
                /// thereby ensuring that it spans across the entire sphere as
                /// expected.
                auto s4_pattern = std::make_shared<RT::gradient_pattern>(RT::color::RGB(0x8b, 0x00, 0x00),
                                                                         RT::color::RGB(0xd2, 0xb4, 0x8c));

                auto s4_pattern_transform = RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0) *
                                            RT_XFORM::create_3d_scaling_matrix(2.0, 1.0, 1.0);
                s4_pattern->transform(s4_pattern_transform);

                auto s4_material = RT::material().set_pattern(s4_pattern);
                s4->set_material(s4_material);
        }

        /// --------------------------------------------------------------------
        /// let's now blend patterns together
        {
                auto s5 = std::make_shared<RT::sphere>();
                world.add(s5);

                auto s5_transforms = RT_XFORM::create_3d_translation_matrix(-3.0, 0.0, 10.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.5, 2.5, 2.5) *
                                     RT_XFORM::create_roty_matrix(RT::PI_BY_4F);

                s5->transform(s5_transforms);

                /// ------------------------------------------------------------
                /// we will be blending two striped patterns. so stripes are
                /// made from alternating colors (color-1, color-2) which varies
                /// as x varies. when x is in [0..1] we get color-1, and when x
                /// is in [1..2] we get color-2.
                ///
                /// so, we create 2 stripes, and then lay them on each other at
                /// an angle, to "blend" them...
                auto s5_p1 = std::make_shared<RT::striped_pattern>(RT::color::RGB(0xff, 0xfa, 0xcd),
                                                                   RT::color::RGB(0x6b, 0x8e, 0x23));

                s5_p1->transform(RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
                                 RT_XFORM::create_3d_scaling_matrix(0.05, 1.0, 1.0));

                auto s5_p2 = std::make_shared<RT::striped_pattern>(RT::color::RGB(0xff, 0xfa, 0xcd),
                                                                   RT::color::RGB(0x6b, 0x8e, 0x23));

                s5_p2->transform(RT_XFORM::create_3d_scaling_matrix(0.01, 1.0, 1.0));

                auto s5_pattern  = std::make_shared<RT::blended_pattern>(s5_p1, s5_p2);
                auto s5_material = RT::material().set_pattern(s5_pattern);
                s5->set_material(s5_material);
        }

        /// --------------------------------------------------------------------
        /// let's now try out blending gradient ring pattern. the general idea
        /// is that at the poles of the sphere we have same or approx. same
        /// color...
        {
                auto s6 = std::make_shared<RT::sphere>();
                world.add(s6);

                auto s6_transforms = RT_XFORM::create_3d_translation_matrix(-1.0, -5.0, 10.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.5, 2.5, 2.5) *
                                     RT_XFORM::create_rotz_matrix(RT::PI_BY_4F);
                s6->transform(s6_transforms);

                auto s6_p1 = std::make_shared<RT::gradient_ring_pattern>(RT::color::RGB(0x19, 0x19, 0x70),
                                                                         RT::color::RGB(0xb0, 0xc4, 0xde));

                auto s6_p2 = std::make_shared<RT::gradient_ring_pattern>(RT::color::RGB(0xb0, 0xc4, 0xde),
                                                                         RT::color::RGB(0x6f, 0x00, 0xaf));

                auto s6_pattern = std::make_shared<RT::blended_pattern>(s6_p1, s6_p2);

                auto s6_material = RT::material().set_pattern(s6_pattern);
                s6->set_material(s6_material);
        }

        /// --------------------------------------------------------------------
        /// let's now try out checkers pattern
        {
                auto s7 = std::make_shared<RT::sphere>();
                world.add(s7);

                auto s7_transforms = RT_XFORM::create_3d_translation_matrix(5.0, -5.0, 10.0) *
                                     RT_XFORM::create_3d_scaling_matrix(1.5, 1.5, 1.5) *
                                     RT_XFORM::create_roty_matrix(RT::PI_BY_4F);
                s7->transform(s7_transforms);

                auto s7_pattern = std::make_shared<RT::checkers_pattern>(RT::color::RGB(0xff, 0xff, 0xff),
                                                                         RT::color::RGB(0x00, 0x64, 0x00));

                s7_pattern->transform(RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 1.0) *
                                      RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0));

                auto s7_material = RT::material().set_pattern(s7_pattern);
                s7->set_material(s7_material);
        }

        /// --------------------------------------------------------------------
        /// let's now try and texture-map a noise pattern on a sphere
        {
                auto s8 = std::make_shared<RT::sphere>(false);
                world.add(s8);

                auto s8_transforms = RT_XFORM::create_3d_translation_matrix(4.5, -2.3, 2.0) *
                                     RT_XFORM::create_3d_scaling_matrix(1.6, 1.6, 1.6) *
                                     RT_XFORM::create_roty_matrix(RT::PI_BY_4F);
                s8->transform(s8_transforms);

                /// ------------------------------------------------------------
                /// now generate a 640 x 512 pixel noisy texture and map that
                /// onto the sphere i.e. s8
                auto noisy_texture = std::make_shared<RT::uv_image>(
                        RT::generate_noisy_texture(640,                                /// canvas-xdim
                                                   512,                                /// canvas-ydim
                                                   RT::color::RGB(0x19, 0x19, 0x70),   /// noise-start-color
                                                   RT::color::RGB(0xff, 0xff, 0xff))); /// noise-end-color

                auto s8_pattern = std::make_shared<RT::texture_2d_pattern>(noisy_texture, RT::spherical_map);
                s8_pattern->transform(RT_XFORM::create_3d_scaling_matrix(1.1, 1.1, 1.1) *
                                      RT_XFORM::create_roty_matrix(RT::PI_BY_2F));

                auto s8_material = RT::material().set_pattern(s8_pattern);
                s8->set_material(s8_material);
        }

        /// ------------------------------------------------------------
        /// let's now try a uv_noise pattern on a sphere
        {
                auto s9 = std::make_shared<RT::sphere>(false);
                world.add(s9);

                auto s9_transforms = RT_XFORM::create_3d_translation_matrix(8.0, 3.3, 6.0) *
                                     RT_XFORM::create_3d_scaling_matrix(1.1, 1.1, 1.1) *
                                     RT_XFORM::create_roty_matrix(RT::PI_BY_4F);
                s9->transform(s9_transforms);

                auto s9_uv_texture =
                        std::make_shared<RT::uv_noise>(RT::color::RGB(0x8b, 0x00, 0x00), /// color-u
                                                       RT::color::RGB(0xff, 0xff, 0xff), /// color-v
                                                       distrib(rnd_gen),                 /// seed
                                                       16);                              /// octaves

                auto s9_pattern = std::make_shared<RT::texture_2d_pattern>(s9_uv_texture, RT::spherical_map);

                s9_pattern->transform(RT_XFORM::create_3d_scaling_matrix(1.1, 1.1, 1.1) *
                                      RT_XFORM::create_roty_matrix(RT::PI_BY_2F));

                s9->set_material(RT::material().set_pattern(s9_pattern));
        }

        /// ------------------------------------------------------------
        /// let's now try a colored glass sphere
        {
                auto s10 = std::make_shared<RT::sphere>(false);
                world.add(s10);

                auto s10_transforms = RT_XFORM::create_3d_translation_matrix(4.5, -1.0, 3.0) *
                                      RT_XFORM::create_3d_scaling_matrix(9.5, 6.5, 1.2) *
                                      RT_XFORM::create_roty_matrix(-RT::PI_BY_6F);

                s10->transform(s10_transforms);
                s10->set_material(RT::create_material_transparent_glass());
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
                                   RT_XFORM::create_3d_translation_matrix(0.0, -10.0, 0.0);
                floor->transform(floor_xform);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// create a camera for looking at the world created by
/// create_world(...)
RT::camera create_shapes_world_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_2F);
        auto look_from     = RT::create_point(-2.0, 1.0, -5.0);
        auto look_to       = RT::create_point(0.0, 0.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
