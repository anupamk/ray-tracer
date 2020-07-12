#ifndef RAYTRACER_INTERSECTION_INFO_HPP__
#define RAYTRACER_INTERSECTION_INFO_HPP__

#include <memory>

/// our includes
#include "tuple.hpp"

namespace raytracer
{
        /// forward declaration
        class shape_interface;

        /// --------------------------------------------------------------------
        /// this represents some information about the intersection.
        class intersection_info_t
        {
            private:
                bool inside_;					/// intersction is inside ?
                double point_;					/// where intersection happens
                tuple position_;				/// position of intersection
                tuple over_position_;				/// EPSILON adjustment (over position_)
                tuple eye_vec_;					/// eye vector
                tuple normal_vec_;				/// normal at point of intersection
                std::shared_ptr<shape_interface const> object_; /// object that is intresected

            public:
                intersection_info_t();

                intersection_info_t& inside(bool val);
                intersection_info_t& point(double val);
                intersection_info_t& position(tuple val);
                intersection_info_t& over_position(tuple val);
                intersection_info_t& eye_vector(tuple val);
                intersection_info_t& normal_vector(tuple val);
                intersection_info_t& what_object(std::shared_ptr<shape_interface const> val);

            public:
                bool inside() const
                {
                        return inside_;
                }

                double point() const
                {
                        return point_;
                }

                tuple position() const
                {
                        return position_;
                }

                tuple over_position() const
                {
                        return over_position_;
                }

                tuple eye_vector() const
                {
                        return eye_vec_;
                }

                tuple normal_vector() const
                {
                        return normal_vec_;
                }

                std::shared_ptr<shape_interface const> what_object() const
                {
                        return object_;
                }
        };
} // namespace raytracer

#endif
