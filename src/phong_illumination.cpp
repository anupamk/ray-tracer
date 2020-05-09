/*
 * implement phong_illumination(...) model
**/

/// c++ includes
#include "logging.h"
#include <cmath>

/// our includes
#include "color.hpp"
#include "point_light.hpp"
#include "tuple.hpp"
#include "material.hpp"

namespace raytracer
{
	/*
         * this function implements the phong reflection model, a brief
         * description of which follows:
         *
         * phong reflection model, is an empirical model of local
         * illumination. it emulates surface reflection of light via interaction
         * between three different types of lighting:
         *
         *   - ambient lighting: this is the background lighting.
         *
         *     under this model ambient lighting is treated as a constant and it
         *     colors all points on the surface equally.
         *
         *   - diffuse reflection: this is the light reflected from matte
         *     surfaces.
         *
         *     it depends on the cosine-of-angle between the light source and
         *     the surface normal at the point of illumination.
         *
         *   - specular reflection: this is the reflection of the light source
         *     itself and causes a 'specular highlight' i.e. the bright spot on
         *     a curved surface.
         *
         *     it depends on the cosine-of-angle between the reflection vector
         *     and viewer (or eye) vector and is controlled by the 'shininess'
         *     of the surface.
        **/
	color phong_illumination(tuple const& surface_point,	    /// where ray intersects surface
				 material const& surface_material,  /// material
				 point_light const& incident_light, /// light illuminating the scene
				 tuple const& eye_vector,	    /// camera || viewer
				 tuple const& surface_normal)	    /// normal at intersection
	{
		/// combine surface-color with incident light's color
		auto const effective_color = surface_material.get_color() * incident_light.get_color();

		/// direction of the light-source
		auto const incident_light_dir = normalize(incident_light.position() - surface_point);

		/*
                 * now compute contributions of various components in the model
                **/

		/// --------------------------------------------------------------------------------
		/// ambient lighting
		auto const ambient_lighting = effective_color * surface_material.get_ambient();

		/// cosine of angle between the incident-light and surface normal-vector.
		float const light_dot_normal = dot(incident_light_dir, surface_normal);

		if (light_dot_normal < 0.0) {
			return ambient_lighting;
		}

		/// --------------------------------------------------------------------------------
		/// diffuse reflection component
		auto const diffuse = effective_color * surface_material.get_diffuse() * light_dot_normal;

		///
		/// reflect_dot_eye represents the cosine of angle between the
		/// reflection vector and eye vector
		///
		auto const reflection_vector = reflect(-incident_light_dir, surface_normal);
		float const reflect_dot_eye  = dot(reflection_vector, eye_vector);

		/// a -ve value here implies that light reflects away from the eye
		if (reflect_dot_eye <= 0.0) {
			return ambient_lighting + diffuse;
		}

		/// --------------------------------------------------------------------------------
		/// compute specular reflection
		auto const factor   = std::pow(reflect_dot_eye, surface_material.get_shininess());
		auto const specular = incident_light.get_color() * surface_material.get_specular() * factor;
 
		/// get the final (clamped) color from individual contributions               
                auto const final_color = ambient_lighting + diffuse + specular;
                return clamp(final_color);
	}
        
} // namespace raytracer
