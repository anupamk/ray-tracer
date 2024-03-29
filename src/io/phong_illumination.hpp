#pragma once

/// our includes
#include "patterns/material.hpp"
#include "primitives/color.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "shapes/shape_interface.hpp"

namespace raytracer
{
        color phong_illumination(std::shared_ptr<shape_interface const>, /// which shape ?
                                 tuple const& surface_point,             /// where ray intersects surface
                                 point_light const& incident_light,      /// light illuminating the scene
                                 tuple const& eye_vector,                /// camera || viewer
                                 tuple const& surface_normal,            /// normal at intersection
                                 bool is_shadowed = false);              /// is the point shadowed ?
}
