#include "world.hpp"

/// c++ includes
#include <algorithm>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

/// our includes
#include "shape_interface.hpp"
#include "point_light.hpp"
#include "color.hpp"
#include "matrix_transformations.hpp"
#include "sphere.hpp"
#include "phong_illumination.hpp"

namespace raytracer
{
        world::world()
            : light_list_() /// darkness...no light
            , shape_list_() /// and no shapes
        {
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a default world i.e a world with a
        /// single light, and 2 spheres...
        world world::create_default_world()
        {
                world w;

                /// add light
                w.add(create_default_light());

                /// and shapes
                auto shape_list = create_default_shapes();
                for (auto s : shape_list) {
                        w.add(s);
                }

                return w;
        }

        /// --------------------------------------------------------------------
        /// this function is called to add a light to the world
        void world::add(point_light p)
        {
                light_list_.push_back(p);
                return;
        }

        /// --------------------------------------------------------------------
        /// this function is called to delete the top-most light-source in the
        /// world.
        void world::pop_light()
        {
                if (light_list_.empty()) {
                        return;
                }

                light_list_.erase(light_list_.begin());
                return;
        }

        /// --------------------------------------------------------------------
        /// this function returns a reference to the list of light sources for
        /// further modification
        std::vector<point_light>& world::modify_lights()
        {
                return light_list_;
        }

        /// --------------------------------------------------------------------
        /// this function is called to add a light to the world
        void world::add(std::shared_ptr<shape_interface> s)
        {
                shape_list_.push_back(s);
                return;
        }

        /// --------------------------------------------------------------------
        /// this function returns a reference to the list of shapes for further
        /// modification
        std::vector<std::shared_ptr<shape_interface>>& world::modify_shapes()
        {
                return shape_list_;
        }

        /// --------------------------------------------------------------------
        /// lights in the world
        std::vector<point_light> const& world::lights() const
        {
                return light_list_;
        }

        /// --------------------------------------------------------------------
        /// shapes in the world
        std::vector<std::shared_ptr<shape_interface>> const& world::shapes() const
        {
                return shape_list_;
        }

        /// --------------------------------------------------------------------
        /// this function is called to return a sorted list of intersections
        /// that a ray makes when it hits objects / shapes in this world
        intersection_records world::intersect(ray_t const& R) const
        {
                intersection_records xs_result;

                for (auto const& shape : shape_list_) {
                        auto shape_xs_record = R.intersect(shape);

                        /// no intersections
                        if (!shape_xs_record) {
                                continue;
                        }

                        /// one or more intersections
                        auto xs_list = shape_xs_record.value();
                        for (auto& xs : xs_list) {
                                xs_result.push_back(xs);
                        }
                }

                /// sort whatever we got
                std::sort(xs_result.begin(), xs_result.end());

                return xs_result;
        }

        /// --------------------------------------------------------------------
        ///
        color world::shade_hit(intersection_info_t const& xs_info) const
        {
                color shade_color = color_black();

                // clang-format off
                for (auto const world_light : light_list_) {
                        shade_color +=  phong_illumination(xs_info.position(),                    /// where intersection happens
                                                           xs_info.what_object()->get_material(), /// object-material
                                                           world_light,				  /// the light
                                                           xs_info.eye_vector(),		  /// eye
                                                           xs_info.normal_vector());		  /// normal
                }
                // clang-format on

                return shade_color;
        }

        /// --------------------------------------------------------------------
        /// compute the color due a ray intersecting a shape in the world
        color world::color_at(ray_t const& r) const
        {
                /// compute the visible intersection
                auto const xs_list	 = intersect(r);
                auto const vis_xs_record = visible_intersection(xs_list);

                if (vis_xs_record.has_value()) {
                        /// ok, so there seems to be a visible intersection. compute the
                        /// color
                        auto const xs_info = r.prepare_computations(vis_xs_record.value());

                        return shade_hit(xs_info);
                }

                /// no visible intersection
                return color_black();
        }

        /// --------------------------------------------------------------------
        /// stringified representation of the world
        std::string world::stringify() const
        {
                std::stringstream ss("");

                /// first all the lights
                ss << "* lights [" << light_list_.size() << "]" << std::endl;

                for (auto const l : light_list_) {
                        ss << l.stringify() << std::endl;
                }

                ss << std::endl;

                /// next all the shapes
                ss << "* shapes [" << shape_list_.size() << "]" << std::endl;
                for (auto const s : shape_list_) {
                        ss << s->stringify() << std::endl;
                }

                return ss.str();
        }

        /*
         * only private functions from this point onwards
        **/

        /// --------------------------------------------------------------------
        /// create a default light for the world
        point_light world::create_default_light()
        {
                auto const light_position = create_point(-10.0, 10.0, -10.0);
                auto const light_color	  = color(1.0, 1.0, 1.0);

                return point_light(light_position, light_color);
        }

        /// --------------------------------------------------------------------
        /// create default shapes that are always present in the world
        std::vector<std::shared_ptr<shape_interface>> world::create_default_shapes()
        {
                // clang-format off

                /// ------------------------------------------------------------
                /// default unit sphere, with elaborate properties
                auto sphere_01                = std::make_shared<raytracer::sphere>();
                auto const sphere_01_material = material().set_color(color(0.8, 1.0, 0.6))
                                                          .set_diffuse(0.7)
                                                          .set_specular(0.2);

                sphere_01->set_material(sphere_01_material);

                // clang-format on

                /// ------------------------------------------------------------
                /// default unit sphere with a radius of 0.5 units
                auto sphere_02 = std::make_shared<raytracer::sphere>();
                sphere_02->transform(matrix_transformations_t::create_3d_scaling_matrix(0.5, 0.5, 0.5));

                /// ok we are done here
                std::vector<std::shared_ptr<shape_interface>> retval = {sphere_01, sphere_02};
                return retval;
        }

} // namespace raytracer
