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
#include "gradient_ring_pattern.hpp"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "plane.hpp"
#include "point_light.hpp"
#include "raytracer_renderer.hpp"
#include "ring_pattern.hpp"
#include "shape_interface.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "striped_pattern.hpp"
#include "tuple.hpp"
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
        auto const world  = create_world();
        auto const camera = create_camera();
        auto dst_fname    = "render-with-patterns.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// benchmark the render with '10' renders performed, and throwing away
        /// the results from '1' of them
        Benchmark<> render_bm(10, 1);
        LOG_INFO("render benchmark info: '%s'", render_bm.stringify().c_str());

        /// --------------------------------------------------------------------
        /// just use the first [0] result only please
        auto rendered_canvas = render_bm.benchmark(RT::single_threaded_renderer, world, camera)[0];
        rendered_canvas.write(dst_fname);

        /// --------------------------------------------------------------------
        /// show what we got
        LOG_INFO("render benchmark results : {mean (ms): '%05zu', standard-deviation (ms): '%05zu'}",
                 render_bm.mean(),                /// mean-usec
                 render_bm.standard_deviation()); /// stddev-usec

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
        /// create the floor
        auto fp_01 =
                std::make_shared<RT::striped_pattern>(RT::color(0.8, 0.8, 0.8), RT::color(0.6, 0.6, 0.6));

        fp_01->transform(RT_XFORM::create_roty_matrix(RT::PI_BY_2F) *
                         RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 5.5));

        auto fp_02 =
                std::make_shared<RT::striped_pattern>(RT::color(0.8, 0.8, 0.8), RT::color(0.6, 0.6, 0.6));

        auto floor_pattern_01 = std::make_shared<RT::blended_pattern>(fp_01, fp_02);

        floor_pattern_01->transform(RT_XFORM::create_roty_matrix(-RT::PI_BY_2F) *
                                    RT_XFORM::create_3d_scaling_matrix(0.6, 1.0, 1.0));

        auto floor = std::make_shared<RT::plane>();
        floor->set_material(RT::material().set_pattern(floor_pattern_01).set_specular(0.1));

        /// --------------------------------------------------------------------
        /// sphere-01 : with striped pattern
        auto sphere_01_pattern = std::make_shared<RT::striped_pattern>(RT::color(0.0, 0.8, 0.0),  /// color-a
                                                                       RT::color(1.0, 1.0, 1.0)); /// color-b

        sphere_01_pattern->transform(RT_XFORM::create_rotz_matrix(RT::PI_BY_4F) *
                                     RT_XFORM::create_3d_scaling_matrix(0.2, 1.0, 1.0));

        auto sphere_01 = std::make_shared<RT::sphere>();
        sphere_01->transform(RT_XFORM::create_3d_translation_matrix(-0.5, 1.33, 4.5) *
                             RT_XFORM::create_3d_scaling_matrix(1.33, 1.33, 1.33));

        sphere_01->set_material(RT::material()
                                        .set_pattern(sphere_01_pattern) /// pattern
                                        .set_diffuse(0.7)               /// diffuse
                                        .set_specular(0.3));            /// specular

        /// --------------------------------------------------------------------
        /// sphere-02 : with gradient pattern
        auto sphere_02_pattern = std::make_shared<RT::gradient_pattern>(RT::color(0.8, 0.8, 0.0),  /// color-a
                                                                        RT::color(0.9, 0.0, 0.0)); /// color-b

        sphere_02_pattern->transform(RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.0, 1.0, 1.0));

        auto sphere_02 = std::make_shared<RT::sphere>();
        sphere_02->transform(RT_XFORM::create_3d_translation_matrix(1.2, 0.5, 0.0) *
                             RT_XFORM::create_rotz_matrix(RT::PI_BY_4F) *
                             RT_XFORM::create_3d_scaling_matrix(0.5, 0.5, 0.5));

        sphere_02->set_material(RT::material()
                                        .set_pattern(sphere_02_pattern) /// pattern
                                        .set_diffuse(0.7)               /// diffuse
                                        .set_specular(0.3));            /// specular

        /// --------------------------------------------------------------------
        /// sphere-03 : with gradient pattern again
        auto sphere_03_pattern = std::make_shared<RT::gradient_pattern>(RT::color(1.0, 1.0, 1.0),  /// color-a
                                                                        RT::color(1.0, 0.0, 0.0)); /// color-b

        sphere_03_pattern->transform(RT_XFORM::create_3d_translation_matrix(-1.0, 0.0, 0.0) *
                                     RT_XFORM::create_3d_scaling_matrix(2.0, 1.0, 1.0));

        auto sphere_03 = std::make_shared<RT::sphere>();
        sphere_03->transform(RT_XFORM::create_3d_translation_matrix(-1.5, 0.33, -0.75) *
                             RT_XFORM::create_3d_scaling_matrix(0.2, 0.33, 0.2));

        sphere_03->set_material(RT::material()
                                        .set_pattern(sphere_03_pattern) /// pattern
                                        .set_diffuse(0.7)               /// diffuse
                                        .set_specular(0.3));            /// specular

        /// --------------------------------------------------------------------
        /// the world light
        auto world_light_01 = RT::point_light(RT::create_point(-3.0, 15.0, -2.0), RT::color_white());

        /// --------------------------------------------------------------------
        /// now create the world...
        auto world = RT::world();
        world.add(world_light_01);

        world.add(floor);
        world.add(sphere_01);
        world.add(sphere_02);
        world.add(sphere_03);

        LOG_DEBUG("world details:\n%s\n", world.stringify().c_str());

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_3F);
        auto look_from     = RT::create_point(0.0, 1.5, -5.0);
        auto look_to       = RT::create_point(0.0, 1.0, 0.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
