#include "world.hpp"

/// c++ includes
#include <algorithm>
#include <cmath>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

/// our includes
#include "color.hpp"
#include "intersection_record.hpp"
#include "matrix_transformations.hpp"
#include "phong_illumination.hpp"
#include "point_light.hpp"
#include "shape_interface.hpp"
#include "solid_pattern.hpp"
#include "sphere.hpp"
#include "tuple.hpp"

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
	void world::add(const std::shared_ptr<const shape_interface> s)
	{
		shape_list_.push_back(s);
		return;
	}

	/// --------------------------------------------------------------------
	/// lights in the world
	std::vector<point_light> const& world::lights() const
	{
		return light_list_;
	}

	/// --------------------------------------------------------------------
	/// shapes in the world
	std::vector<std::shared_ptr<const shape_interface>> const& world::shapes() const
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
	/// compute the color when a ray hits the world
	color world::shade_hit(intersection_info_t const& xs_info, uint8_t remaining) const
	{
		color shade_color = color_black();

		// clang-format off
                for (auto const& a_light : light_list_) {
                        auto point_in_shadow = is_shadowed(xs_info.over_position(), a_light);
                        
                        shade_color += phong_illumination(xs_info.what_object(),   /// object-material
                                                          xs_info.over_position(), /// point of intersection
                                                          a_light,		   /// the light
                                                          xs_info.eye_vector(),    /// eye
                                                          xs_info.normal_vector(), /// normal
                                                          point_in_shadow);	   /// shadowed ?
                }
		// clang-format on

		auto const reflect_color = reflected_color(xs_info, remaining);
		auto const refract_color = refracted_color(xs_info, remaining);

		/// ------------------------------------------------------------
		/// employ reflectance with reflection and refraction
		auto const mat = xs_info.what_object()->get_material();
		if ((mat.get_reflective() > 0.0) && (mat.get_transparency() > 0.0)) {
			auto const reflectance = xs_info.schlick_approx();

			return shade_color                              /// phong-color
			       + (reflect_color * reflectance)          /// reflection+reflectance
			       + (refract_color * (1.0 - reflectance)); /// refract+reflectance
		}

		return (shade_color + reflect_color + refract_color);
	}

	/// --------------------------------------------------------------------
	/// compute the color due a ray intersecting a shape in the world
	color world::color_at(ray_t const& r, uint8_t remaining) const
	{
		/// compute the visible intersection
		auto xs_list       = intersect(r);
		auto vis_xs_record = visible_intersection(xs_list);

		if (vis_xs_record) {
			/// ok, so there seems to be a visible intersection. compute the
			/// color
			auto xs_record     = vis_xs_record.value();
			auto const xs_info = r.prepare_computations(xs_list, xs_record.index());
			return shade_hit(xs_info, remaining);
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
		for (auto const& s : shape_list_) {
			ss << s->stringify() << std::endl;
		}

		return ss.str();
	}

	/// --------------------------------------------------------------------
	/// returns true if the point(pt), is in shadow w.r.t the light sources
	/// making up the entire scene. returns false otherwise.
	///
	/// cast a ray, called shadow-ray, from the point towards the light
	/// source.
	///
	/// if shadow-ray intersects something between point, and light-source,
	/// then the point is considered to be in shadow.
	bool world::is_shadowed(tuple const& pt, point_light const& light) const
	{
		auto const pt_to_light   = light.position() - pt;
		auto const dist_to_light = magnitude(pt_to_light);
		auto const shadow_ray    = ray_t(pt, normalize(pt_to_light));

		return shadow_ray.has_intersection_before(shapes(), dist_to_light);
	}

	/// --------------------------------------------------------------------
	/// compute the color for reflections
	color world::reflected_color(intersection_info_t const& xs_info, uint8_t remaining) const
	{
		auto const xs_obj_material = xs_info.what_object()->get_material();
		auto const mat_reflective  = xs_obj_material.get_reflective();

		/// surprise: non reflective objects reflect no color at all.
		if ((mat_reflective == 0.0) || (remaining < 1)) {
			return color_black();
		}

		ray_t const reflected_ray(xs_info.over_position(), xs_info.reflection_vector());
		return color_at(reflected_ray, remaining - 1) * mat_reflective;
	}

	/// --------------------------------------------------------------------
	/// compute the refracted color
	color world::refracted_color(intersection_info_t const& xs_info, uint8_t remaining) const
	{
		auto const xs_obj_material  = xs_info.what_object()->get_material();
		auto const mat_transparency = xs_obj_material.get_transparency();

		/// surprise: non transparent objects have no refracted-color
		/// component at all
		if ((mat_transparency == 0.0) || (remaining < 1)) {
			return color_black();
		}

		/// ------------------------------------------------------------
		/// account for total-internal-reflection using Snell's Law
		double const ri_ratio  = xs_info.n1() / xs_info.n2();
		double const cos_i     = dot(xs_info.eye_vector(), xs_info.normal_vector());
		double const sin_sqr_t = (ri_ratio * ri_ratio) * (1.0 - (cos_i * cos_i));

		if (sin_sqr_t > 1.0) {
			return color_black();
		}

		/// ------------------------------------------------------------
		/// figure out the refracted-ray (rr) and compute its
		/// color.
		double const cos_t      = std::sqrt(1.0 - sin_sqr_t);
		auto const rr_direction = xs_info.normal_vector() * (ri_ratio * cos_i - cos_t) -
					  (xs_info.eye_vector() * ri_ratio);

		/// refracted-ray originates at a point just-under the point of
		/// intersection...
		auto const refracted_ray = ray_t(xs_info.under_position(), rr_direction);

		return color_at(refracted_ray, remaining - 1) * mat_transparency;
	}

	/*
	 * only private functions from this point onwards
	 **/

	/// --------------------------------------------------------------------
	/// create a default light for the world
	point_light world::create_default_light()
	{
		auto const light_position = create_point(-10.0, 10.0, -10.0);
		auto const light_color    = color(1.0, 1.0, 1.0);

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
                auto sphere_01_pattern        = std::make_shared<solid_pattern>(color(0.8, 1.0, 0.6));
                auto const sphere_01_material = material().set_pattern(sphere_01_pattern)
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
