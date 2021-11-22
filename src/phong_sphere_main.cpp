/*
 * single-threaded as well as multi-threaded rendering of a phong sphere.
 **/

/// c++ includes
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <utility>
#include <vector>

/// our includes
#include "camera.hpp"
#include "canvas.hpp"
#include "color.hpp"
#include "common/include/assert_utils.h"
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "intersection_record.hpp"
#include "material.hpp"
#include "matrix_transformations.hpp"
#include "phong_illumination.hpp"
#include "point_light.hpp"
#include "ray.hpp"
#include "raytracer_renderer.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
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

/// --------------------------------------------------------------------------------
/// file specific function declarations
static RT::world create_world();
static RT::camera create_camera();

int main(int argc, char** argv)
{
        const auto camera = create_camera();
        const auto world  = create_world();
        auto dst_fname    = "phong-sphere.ppm";

        LOG_INFO("canvas details : {width (pixels): %d, height (pixels): %d, "
                 "destination: '%s'}",
                 camera.hsize(), camera.vsize(), dst_fname);

        /// --------------------------------------------------------------------
        /// benchmark the render with '10' renders performed, and throwing away
        /// the results from '1' of them
        Benchmark<> render_bm(10, 2);
        LOG_INFO("render benchmark info: '%s'", render_bm.stringify().c_str());

        /// --------------------------------------------------------------------
        /// just use the first [0] result only please
        /// auto rendered_canvas = render_bm.benchmark(RT::multi_threaded_renderer, world, camera)[0];
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
 * file specific functions
 **/

/// ---------------------------------------------------------------------------
/// this function is called to create a world which is then rendered using
/// primitives from the camera.
static RT::world create_world()
{
        auto world = RT::world();

        /// --------------------------------------------------------------------
        /// just a sphere
        {
                auto sphere_01         = std::make_shared<RT::sphere>();
                auto sphere_01_pattern = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.2, 1.0));
                sphere_01->set_material(RT::material().set_pattern(sphere_01_pattern).set_specular(0.2));
                sphere_01->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

                world.add(sphere_01);
        }

        /// --------------------------------------------------------------------
        /// and a light
        {
                auto world_light_01 =
                        RT::point_light(RT::create_point(-20.0, 20.0, -20.0), RT::color_white());

                world.add(world_light_01);
        }

        return world;
}

/// ----------------------------------------------------------------------------
/// this function is called to create a camera through which the world is then
/// observed.
static RT::camera create_camera()
{
        auto camera_01     = RT::camera(1280, 1024, RT::PI_BY_2F);
        auto look_from     = RT::create_point(0.0, 0.0, -5.0);
        auto look_to       = RT::create_point(0.0, -1.0, 5.0);
        auto up_dir_vector = RT::create_vector(0.0, 1.0, 0.0);
        auto xform         = RT_XFORM::create_view_transform(look_from, look_to, up_dir_vector);

        camera_01.transform(xform);

        return camera_01;
}
