#pragma once

/// c++ includes
#include <cmath>
#include <ostream>
#include <string>

/// our includes
#include "utils/utils.hpp"

namespace raytracer
{
        /// tuple can represent either a point or a vector.
        enum class tuple_type_t : unsigned char {
                VECTOR = 0,
                POINT  = 1,
        };

        /*
         * we are following the left-handed-coordinate system as follows
         *    - x : +ve x points to the right
         *    - y : +ve y points up
         *    - z : +ve z points inside the screen
         **/
        class tuple final
        {
            private:
                double x_;
                double y_;
                double z_;
                tuple_type_t w_;

            public:
                constexpr explicit tuple(double x, double y, double z, tuple_type_t w)
                    : x_(x)
                    , y_(y)
                    , z_(z)
                    , w_(w)
                {
                }

            public:
                /*
                 * get the values out
                 **/
                constexpr double x() const
                {
                        return this->x_;
                }

                constexpr double y() const
                {
                        return this->y_;
                }

                constexpr double z() const
                {
                        return this->z_;
                }

                constexpr tuple_type_t tuple_type() const
                {
                        return this->w_;
                }

            public:
                /// ------------------------------------------------------------
                /// query tuple for point || vector
                constexpr bool is_point() const
                {
                        return (this->w_ == tuple_type_t::POINT);
                }

                constexpr bool is_vector() const
                {
                        return (this->w_ == tuple_type_t::VECTOR);
                }

                /// ------------------------------------------------------------
                /// forcefully make this tuple a vector !
                constexpr void vectorify()
                {
                        this->w_ = tuple_type_t::VECTOR;
                }

                /// ------------------------------------------------------------
                /// forcefully make this tuple a point !
                constexpr void pointify()
                {
                        this->w_ = tuple_type_t::POINT;
                }

                /// ------------------------------------------------------------
                /// stringified representation of a tuple
                std::string stringify() const;

            public:
                /*
                 * some operators
                 **/
                tuple& operator+=(tuple);
                tuple& operator-=(tuple);
        };

        tuple operator+(tuple a, tuple b);
        tuple operator-(tuple a, tuple b);
        std::ostream& operator<<(std::ostream& os, tuple const& a);

        /// --------------------------------------------------------------------
        /// epsilon equality comparison of two tuples
        constexpr bool operator==(tuple lhs, tuple rhs)
        {
                // clang-format off
                return (lhs.tuple_type() == rhs.tuple_type() &&
                        epsilon_equal(lhs.x(), rhs.x())      &&
                        epsilon_equal(lhs.y(), rhs.y())      &&
                        epsilon_equal(lhs.z(), rhs.z()));
                // clang-format on
        }

        /// --------------------------------------------------------------------
        /// scalar operations multiply, divide, negation
        constexpr tuple operator*(tuple a, double f)
        {
                return tuple(a.x() * f, a.y() * f, a.z() * f, a.tuple_type());
        }

        constexpr tuple operator/(tuple a, double f)
        {
                return tuple(a.x() / f, a.y() / f, a.z() / f, a.tuple_type());
        }

        constexpr tuple operator-(tuple a)
        {
                return (a * -1.0);
        }

        constexpr tuple operator+(tuple a, double f)
        {
                return tuple(a.x() + f, a.y() + f, a.z() + f, a.tuple_type());
        }

        /// --------------------------------------------------------------------
        /// create a point using tuple as the underlying interface.
        constexpr tuple create_point(double x, double y, double z)
        {
                return tuple(x, y, z, tuple_type_t::POINT);
        }

        /// --------------------------------------------------------------------
        /// create a vector using tuple as the underlying interface.
        constexpr tuple create_vector(double x, double y, double z)
        {
                return tuple(x, y, z, tuple_type_t::VECTOR);
        }

        /// --------------------------------------------------------------------
        /// return the dot product of two vectors
        constexpr double dot(tuple a, tuple b)
        {
                return ((a.x() * b.x()) + // x
                        (a.y() * b.y()) + // y
                        (a.z() * b.z())); // z
        }

        /// --------------------------------------------------------------------
        /// return the cross product of two vectors
        constexpr tuple cross(tuple a, tuple b)
        {
                return create_vector((a.y() * b.z()) - (a.z() * b.y()),  // x
                                     (a.z() * b.x()) - (a.x() * b.z()),  // y
                                     (a.x() * b.y()) - (a.y() * b.x())); // z
        }

        /// --------------------------------------------------------------------
        /// return a point an +INFINITY
        constexpr tuple point_at_positive_infinity()
        {
                return create_point(INF, INF, INF);
        }

        /// --------------------------------------------------------------------
        /// return a point an -INFINITY
        constexpr tuple point_at_negative_infinity()
        {
                return create_point(-INF, -INF, -INF);
        }

        inline double magnitude(tuple a)
        {
                return std::sqrt(dot(a, a));
        }

        /// --------------------------------------------------------------------
        /// return a tuple such that it's magnitude is '1'
        inline tuple normalize(tuple a)
        {
                return a / magnitude(a);
        }

        /// --------------------------------------------------------------------
        /// return the reflection vector of a vector 'in' at the normal 'N'
        tuple reflect(tuple in, tuple N);

} // namespace raytracer
