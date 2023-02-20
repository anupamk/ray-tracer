/// c++ includes
#include <cmath>

/// our includes
#include "patterns/uv_mapper.hpp"
#include "primitives/tuple.hpp"
#include "primitives/uv_point.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// spherical_map(...) maps a point 'P' on the surface of a sphere to a
        /// corresponding uv-point on the texture.
        uv_point spherical_map(tuple const& P)
        {
                /// ------------------------------------------------------------
                /// compute the azimuthal (along the x-z axis) angle theta,
                /// which varies over the range (-π..π]
                ///
                /// theta increases clockwise when viewed from above. but this
                /// will be fixed later.
                auto const theta = std::atan2(P.x(), P.z());

                /// ------------------------------------------------------------
                /// compute the polar (along the y-x axis) angle which varies
                /// over the range [0..π].
                auto const radius = magnitude(P);
                auto const phi    = std::acos(P.y() / radius);

                /// ------------------------------------------------------------
                /// 0.5 < raw_u ≤ 0.5
                auto const raw_u = theta / (2 * PI);

                /// ------------------------------------------------------------
                /// fix the direction as well i.e subtract from 1 so that it
                /// increases counter-clockwise (when viewed from above)
                auto const u = 1 - (raw_u + 0.5);

                /// ------------------------------------------------------------
                /// polar angle is '0' at the south pole of the sphere and is
                /// '1' at the north pole. so we flip it over
                auto const v = 1.0 - phi / PI;

                return uv_point(u, v);
        }

        /// --------------------------------------------------------------------
        /// planar_map(...) maps a point 'P' on the surface of a plane to
        /// a corresponding uv-point on the texture.
        uv_point planar_map(tuple const& P)
        {
                auto const u_val = modulus(P.x(), 1.0);
                auto const v_val = modulus(P.z(), 1.0);

                return uv_point(u_val, v_val);
        }

        /// --------------------------------------------------------------------
        /// cylindrical_map(...) maps a point 'P' on the surface of a cylinder
        /// to a corresponding uv-point on the texture.
        uv_point cylindrical_map(tuple const& P)
        {
                /// ------------------------------------------------------------
                /// compute the azimuthal (along the x-z axis) angle theta,
                /// which varies over the range (-π..π]
                ///
                /// theta increases clockwise when viewed from above. but this
                /// will be fixed later.
                auto const theta = std::atan2(P.x(), P.z());

                /// ------------------------------------------------------------
                /// 0.5 < raw_u ≤ 0.5
                auto const raw_u = theta / (2 * PI);

                /// ------------------------------------------------------------
                /// fix the direction as well i.e subtract from 1.0 so that it
                /// increases counter-clockwise (when viewed from above)
                auto const u = 1.0 - (raw_u + 0.5);

                /// ------------------------------------------------------------
                /// v goes from (0.0 .. 1.0) between whole units of y
                auto const v = 1.0 - modulus(P.y(), 1.0);

                return uv_point(u, v);
        }

        /// --------------------------------------------------------------------
        /// conical_map(...) maps a point 'P' on the surface of a cone to a
        /// corresponding uv-point on the texture.
        uv_point conical_map(tuple const& P)
        {
                /// ------------------------------------------------------------
                /// compute the azimuthal (along the x-z axis) angle theta,
                /// which varies over the range (-π..π]
                ///
                /// theta increases clockwise when viewed from above. but this
                /// will be fixed later.
                auto const theta = std::atan2(P.x(), P.z());

                /// ------------------------------------------------------------
                /// 0.5 < raw_u ≤ 0.5
                auto const raw_u = theta / (2 * PI);

                /// ------------------------------------------------------------
                /// fix the direction as well i.e subtract from 1.0 so that it
                /// increases counter-clockwise (when viewed from above)
                auto const u = 1.0 - (raw_u + 0.5);

                /// ------------------------------------------------------------
                /// v goes from (0.0 .. 1.0) between whole units of y
                auto const v = 1.0 - modulus(P.y(), 1.0);

                return uv_point(u, v);
        }

        /// add more mappings ↑

} // namespace raytracer
