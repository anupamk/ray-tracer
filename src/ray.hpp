#ifndef RAYTRACER_RAY_HPP__
#define RAYTRACER_RAY_HPP__

/// c++ includes
#include <algorithm>
#include <memory>
#include <ostream>
#include <string>

// our includes
#include "tuple.hpp"
#include "matrix.hpp"
#include "shape_interface.hpp"
#include "intersection_info.hpp"

namespace raytracer
{
        /*
         * this defines a ray and it's attributes using parametric form of the
         * equation of a straight line.
         *
         * a point 't' on the ray 'R' is represented by the following equation:
         *       P(t) = R.origin() + R.direction() * t
         *
         * where 't' is the distance on the ray from its origin, and 't' can be
         * both positive or negative, with following interpretation:
         *
         *       t : +ve ==> point 'in front' of origin
         *       t : -ve ==> point 'behind' the origin
        **/
        class ray_t
        {
            private:
                tuple origin_;
                tuple direction_;

            public:
                ray_t(tuple origin, tuple direction);

            public:
                tuple origin() const;
                tuple direction() const;

            public:
                tuple position(double t) const;
                ray_t transform(fsize_dense2d_matrix_t const& M) const;
                std::string stringify() const;
                std::optional<intersection_records> intersect(std::shared_ptr<const shape_interface> const&) const;

                /// return some information about the intersection
                intersection_info_t prepare_computations(intersection_record) const;
        };

        bool operator==(ray_t const& lhs, ray_t const& rhs);
        std::ostream& operator<<(std::ostream& os, ray_t const& R);

} // namespace raytracer

#endif // RAYTRACER_RAY_HPP_
