#ifndef RAYTRACER_SCENE_PARAMS_HPP__
#define RAYTRACER_SCENE_PARAMS_HPP__

/// c++ includes
#include <cstdint>
#include <memory>
#include <string>

/// our includes
#include "tuple.hpp"
#include "point_light.hpp"
#include "shape_interface.hpp"
#include "material.hpp"
#include "color.hpp"
#include "sphere.hpp"
#include "matrix_transformations.hpp"
#include "solid_pattern.hpp"

namespace raytracer
{
        /// ----------------------------------------------------------------------------
        /// this describes the scene parmeters that are used for coloring a particular
        /// point on the scene.
        class scene_params
        {
            public:
                /// stuff 'outside' the scene
                uint32_t const canvas_dim_x;
                uint32_t const canvas_dim_y;
                float const wall_zpos;
                float const wall_xsize;
                float const wall_ysize;
                tuple const camera_position;

                /// objects making up the scene
                std::shared_ptr<shape_interface> const shape;
                point_light const light;

                // clang-format off
                float wall_half_xsize()   const { return wall_xsize * 0.5;          }
                float wall_half_ysize()   const { return wall_ysize * 0.5;          }
                float wall_xpixel_size()  const { return wall_xsize / canvas_dim_x; }
                float wall_ypixel_size()  const { return wall_ysize / canvas_dim_y; }
                // clang-format on
        };

        /// ----------------------------------------------------------------------------
        /// this function is called to create scene parameters. these describe things
        /// like position of the wall, the sphere (it's material, color etc) and the
        /// size of canvas where things are drawn etc.
        static inline scene_params create_scene_params()
        {
                namespace RT = raytracer;

                /// --------------------------------------------------------------------------------
                /// define some scene specific constants first.
                auto const CAMERA_POSITION = create_point(0.0, 0.0, -5.0);
                float const WALL_ZPOS	   = 10.0;
                float const WALL_XSIZE	   = 7.0;
                float const WALL_YSIZE	   = 7.0;

                /// maintain a 3:4 aspect ratio for the canvas
                uint32_t const canvas_dim_x = 1280;
                uint32_t const canvas_dim_y = (canvas_dim_x * 3) / 4;

                /// --------------------------------------------------------------------------------
                /// setup sphere
                auto sphere_material	= material();
                auto const sphere_pattern = std::make_shared<RT::solid_pattern>(RT::color(1.0, 0.2, 1.0));
                sphere_material.set_pattern(sphere_pattern).set_specular(0.7);

                auto sphere_01 = std::make_shared<raytracer::sphere>();
                sphere_01->set_material(sphere_material);

                /// --------------------------------------------------------------------------------
                /// setup scene-lighting
                auto const light_position = create_point(-20.0, 20.0, -20.0);
                auto const light_color	  = raytracer::color_white();
                auto const scene_light	  = point_light(light_position, light_color);

                // clang-format off
                auto const scene_descr = (std::string("sphere: {") + sphere_01->stringify() + "}, " +
                                          std::string("light: {") + scene_light.stringify() + "}");

                LOG_DEBUG("phong-sphere scene description '%s'", scene_descr.c_str());
                // clang-format on

                /// and finally return the scene
                return scene_params{canvas_dim_x,    /// x-dimension
                                    canvas_dim_y,    /// y-dimension
                                    WALL_ZPOS,	     /// wall-position
                                    WALL_XSIZE,	     /// wall-x-size
                                    WALL_YSIZE,	     /// wall-y-size
                                    CAMERA_POSITION, /// camera-position
                                    sphere_01,	     /// sphere
                                    scene_light};    /// and the light
        }

} // namespace raytracer

#endif /// RAYTRACER_SCENE_PARAMS_HPP__
