#ifndef RAYTRACER_PHONG_ILLUMINATION_HPP__
#define RAYTRACER_PHONG_ILLUMINATION_HPP__

/// our includes
#include "color.hpp"
#include "material.hpp"
#include "point_light.hpp"
#include "shape_interface.hpp"
#include "tuple.hpp"

namespace raytracer
{
        color phong_illumination(std::shared_ptr<shape_interface const>, /// which shape ?
                                 tuple const& surface_point,             /// where ray intersects surface
                                 point_light const& incident_light,      /// light illuminating the scene
                                 tuple const& eye_vector,                /// camera || viewer
                                 tuple const& surface_normal,            /// normal at intersection
                                 bool is_shadowed = false);              /// is the point shadowed ?
}

#endif /// RAYTRACER_PHONG_ILLUMINATION_HPP__
