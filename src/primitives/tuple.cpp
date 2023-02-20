/*
 * implement the raytracer tuple
 **/

/// c++ includes
#include <ios>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>

/// our includes
#include "common/include/assert_utils.h"
#include "primitives/tuple.hpp"
#include "utils/execution_profiler.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// implement 'a += b' where both 'a' and 'b' are tuples
        tuple& tuple::operator+=(tuple other)
        {
                /// this.point + other.point == undefined
                if (other.is_point() && this->is_point()) {
                        ASSERT_FAIL("tuple::opertor+=(...) adding points !");
                }

                /// whether we get a point or a vector, tuple components can be
                /// individually added
                this->x_ += other.x_;
                this->y_ += other.y_;
                this->z_ += other.z_;

                /// this.vector + other.vector == this.vector
                if (other.is_vector() && this->is_vector()) {
                        return *this;
                }

                /// this.point + other.vector == this.point
                this->w_ = tuple_type_t::POINT;
                return *this;
        }

        /// --------------------------------------------------------------------
        /// implement 'a -= b' where both 'a' and 'b' are tuples
        tuple& tuple::operator-=(tuple other)
        {
                /// this.vector - this.point == undefined
                if (this->is_vector() && other.is_point()) {
                        ASSERT_FAIL("tuple::opertor-=(...) subtracting point from vector !");
                }

                /// whether we get a point or a vector, tuple components can be
                /// individually added
                this->x_ -= other.x_;
                this->y_ -= other.y_;
                this->z_ -= other.z_;

                /// this.point - other.vector == this.point
                if (this->is_point() && other.is_vector()) {
                        this->w_ = tuple_type_t::POINT;
                        return *this;
                }

                /// 2 remaining cases yield a vector
                ///    - this.point - other.point   == this.vector
                ///    - this.vector - other.vector == this.vector
                this->w_ = tuple_type_t::VECTOR;
                return *this;
        }

        /// --------------------------------------------------------------------
        /// stringified representation of a tuple
        std::string tuple::stringify() const
        {
                int const tuple_type_val = (this->tuple_type() == tuple_type_t::VECTOR) ? 0 : 1;
                std::stringstream ss("");

                // clang-format off
                ss << std::fixed << std::left
                   << "x: " << this->x() << ", "
                   << "y: " << this->y() << ", "
                   << "z: " << this->z() << ", "
                   << "w: " << tuple_type_val;
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// we are better off reducing the number of functions that need access
        /// to class internals, so, just define binary '+', '-' in terms of '+='
        /// and '-='.
        tuple operator+(tuple a, tuple b)
        {
                return a += b;
        }

        tuple operator-(tuple a, tuple b)
        {
                return a -= b;
        }

        /// --------------------------------------------------------------------
        /// 'reasonably' formatted output for the tuple
        std::ostream& operator<<(std::ostream& os, tuple const& a)
        {
                return os << a.stringify();
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the reflection of a vector 'in'
        /// around the normal vector 'N'
        tuple reflect(tuple in, tuple N)
        {
                PROFILE_SCOPE;

                ASSERT(in.is_vector() && N.is_vector());
                return in - N * 2 * dot(in, N);
        }

} // namespace raytracer
