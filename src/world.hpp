#ifndef RAYTRACER_WORLD_HPP__
#define RAYTRACER_WORLD_HPP__

/// c++ includes
#include "color.hpp"
#include <memory>
#include <vector>

/// our includes
#include "shape_interface.hpp"
#include "point_light.hpp"
#include "intersection_info.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this implements a world object which acts as a container for a set
        /// of objects that make up a scene including various instantiation of
        /// the shape, light-sources etc. etc.
        class world final
        {
            private:
                std::vector<point_light> light_list_;
                std::vector<std::shared_ptr<shape_interface>> shape_list_;

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
                void add(std::shared_ptr<shape_interface>);
                std::vector<std::shared_ptr<shape_interface>>& modify_shapes();

            public:
                std::vector<point_light> const& lights() const;
                std::vector<std::shared_ptr<shape_interface>> const& shapes() const;

                /// sorted list of intersections that a ray makes in this world
                intersection_records intersect(ray_t const&) const;

                /// compute the color when a ray hits the world
                color shade_hit(intersection_info_t const&) const;

                /// compute the color due to a ray.
                color color_at(ray_t const&) const;

                /// stringified representation of the world
                std::string stringify() const;

            private:
                static point_light create_default_light();
                static std::vector<std::shared_ptr<shape_interface>> create_default_shapes();
        };
} // namespace raytracer

#endif /// RAYTRACER_WORLD_HPP__