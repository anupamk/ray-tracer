#ifndef RAYTRACER_PLANE_HPP__
#define RAYTRACER_PLANE_HPP__

/// c++ includes
#include "badge.hpp"
#include <optional>
#include <ostream>
#include <string>

/// our includes
#include "shape_interface.hpp"
#include "intersection_record.hpp"
#include "tuple.hpp"
#include "ray.hpp"
#include "assert_utils.h"
#include "uv_point.hpp"

namespace raytracer
{
        /*
         * this defines a xz-plane
        **/
        class plane final : public shape_interface
        {
            public:
                plane();

            public:
                /// ------------------------------------------------------------
                /// stringified representation of a plane
                std::string stringify() const override;

                /// ------------------------------------------------------------
                /// compute intersection of a ray with the plane
                std::optional<intersection_records> intersect(the_badge<ray_t>, ray_t const& R) const override;

                /// ------------------------------------------------------------
                /// normal vector at a give point on the plane (in local
                /// coordinates)
                tuple normal_at_local(tuple const&) const override;

                /// ------------------------------------------------------------
                /// map a point on the surface of the plane to a corresponding
                /// uv-value (on a texture)
                uv_point map_to_uv(tuple const&) const override;
        };

        std::ostream& operator<<(std::ostream& os, plane const& P);
} // namespace raytracer

#endif /// RAYTRACER_PLANE_HPP__
