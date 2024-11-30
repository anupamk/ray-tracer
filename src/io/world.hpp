#pragma once

/// c++ includes
#include <initializer_list>
#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

/// our includes
#include "primitives/color.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/point_light.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class intersection_info_t;
        class ray_t;
        class shape_interface;
        class tuple;

        /// --------------------------------------------------------------------
        /// this implements a world object which acts as a container for a set
        /// of objects that make up a scene including various instantiation of
        /// the shape, light-sources etc. etc.
        class world final
        {
            private:
                std::vector<point_light> light_list_;
                std::vector<std::shared_ptr<shape_interface const>> shape_list_;

                /// ------------------------------------------------------------
                /// avoid bouncing rays between reflective surfaces till
                /// infinity. this limits max number of reflections/refractions
                /// to some sane number
                static inline constexpr uint8_t MAX_RECURSION_DEPTH = 5;

            public:
                world();
                static world create_default_world();

                /// ------------------------------------------------------------
                /// light operations
                void add(point_light);
                void pop_light();
                std::vector<point_light>& modify_lights();

                /// ------------------------------------------------------------
                /// shape operations
                void add(std::shared_ptr<shape_interface const> const);

            public:
                std::vector<point_light> const& lights() const;
                std::vector<std::shared_ptr<shape_interface const>> const& shapes() const;

                /// sorted list of intersections that a ray makes in this world
                intersection_records intersect(ray_t const&) const;

                /// compute the color when a ray hits the world
                color shade_hit(intersection_info_t const&, uint8_t remaining = MAX_RECURSION_DEPTH) const;

                /// ------------------------------------------------------------
                /// compute the color due to one or more rays, at a specific
                /// pixel.
                ///
                /// this is quite useful f.e. when we want to perform
                /// antialiased rendering of images with multiple samples per
                /// pixel.
                ///
                /// 'aa_color_scale' is a value in range (0.0 .. 1.0]
                color color_at(ray_t const&, uint8_t remaining = MAX_RECURSION_DEPTH) const;

                /// stringified representation of the world
                std::string stringify() const;

                /// return true if point 'pt' is in shadow w.r.t a light source.
                bool is_shadowed(tuple const& pt, point_light const& light) const;

                /// compute the reflected color
                color reflected_color(intersection_info_t const&,
                                      uint8_t remaining = MAX_RECURSION_DEPTH) const;

                /// compute the refracted color
                color refracted_color(intersection_info_t const&,
                                      uint8_t remaining = MAX_RECURSION_DEPTH) const;

            private:
                static point_light create_default_light();
                static std::vector<std::shared_ptr<shape_interface>> create_default_shapes();
        };
} // namespace raytracer
