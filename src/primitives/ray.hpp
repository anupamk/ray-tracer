#pragma once

/// c++ includes
#include <memory>
#include <optional>
#include <ostream>
#include <stddef.h>
#include <string>
#include <vector>

/// our includes
#include "primitives/intersection_info.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/matrix.hpp"
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declaration
        class shape_interface;

        /*
         * this class defines a ray and it's attributes. a ray is described
         * using parametric form of the equation of a straight line, which looks
         * like this:
         *
         *       P(t) = R.origin() + R.direction() * t
         * where:
         *       a point 't' on the ray 'R' which originates at 'origin' and has
         * a specific direction.
         *
         * ofcourse, the ray extends infinitely on both the sides of it's
         * origin, and therefore, 't' can be both positive and negative:
         *       +ve ==> point 'in front' of origin
         *       -ve ==> point 'behind' the origin
         **/
        class ray_t final
        {
            private:
                /// ------------------------------------------------------------
                /// point of origin of the ray
                tuple origin_;

                /// ------------------------------------------------------------
                /// direction where ray 'goes' to.
                tuple direction_;

            public:
                ray_t(tuple origin, tuple direction);

            public:
                tuple origin() const;
                tuple direction() const;

            public:
                /// ------------------------------------------------------------
                /// stringified representation of a ray
                std::string stringify() const;

                /// ------------------------------------------------------------
                /// compute the position of a point on the ray which is a
                /// distance 't' from the origin of the ray
                tuple position(double t) const;

                /// ------------------------------------------------------------
                /// apply a transformation matrix on a ray, and return the new
                /// transformed ray
                ray_t transform(fsize_dense2d_matrix_t const& M) const;

                /// ------------------------------------------------------------
                /// compute the result of a ray intersecting a shape.
                std::optional<intersection_records>
                intersect(std::shared_ptr<shape_interface const> const&) const;

                /// ------------------------------------------------------------
                /// return meta-information about an intersection. intersections
                /// are identified by an 'index', which defaults to '0' (or the
                /// first intersection)
                ///
                /// meta-information includes f.e. what object is intersected
                /// (and where), normal at point of intersection, reflection
                /// vector etc. etc.
                intersection_info_t prepare_computations(intersection_records const&, size_t index = 0) const;

                /// ------------------------------------------------------------
                /// returns 'true' if this ray intersects the list of
                /// world-objects before 'distance'. returns 'false' otherwise.
                bool has_intersection_before(
                        std::vector<std::shared_ptr<shape_interface const>> const& world_objects,
                        double distance) const;
        };

        /// --------------------------------------------------------------------
        /// compare 2 rays for equivalence. rays are equal when they both have
        /// same origin and direction
        bool operator==(ray_t const& lhs, ray_t const& rhs);

        /// --------------------------------------------------------------------
        /// stringifed representation of a ray
        std::ostream& operator<<(std::ostream& os, ray_t const& R);

} // namespace raytracer
