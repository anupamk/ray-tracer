#pragma once

#include <memory>

/// our includes
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// forward declaration
        class shape_interface;

        /// --------------------------------------------------------------------
        /// this represents some information about the intersection.
        class intersection_info_t final
        {
            private:
                /// ------------------------------------------------------------
                /// is the intersection inside the object ?
                bool inside_;

                /// ------------------------------------------------------------
                /// which point, and where (on the ray) does the intersection
                /// happen ?
                double point_;
                tuple position_;

                /// ------------------------------------------------------------
                /// refractive index of material ray is passing
                /// from -> to (n1 -> n2)
                float ri_n1_;
                float ri_n2_;

                /// ------------------------------------------------------------
                /// a point _just_ 'above' the intersection position, indicating
                /// where the reflected ray will originate
                tuple over_position_;

                /// ------------------------------------------------------------
                /// a point _just_ 'under' the intersection position, indicating
                /// where the refracted ray will originate
                tuple under_position_;

                /// ------------------------------------------------------------
                /// where is the eye vector located
                tuple eye_vec_;

                /// ------------------------------------------------------------
                /// the normal-vector at the point of intersection
                tuple normal_vec_;

                /// ------------------------------------------------------------
                /// the reflection-vector at the point of intersection
                tuple reflect_vec_;

                /// ------------------------------------------------------------
                /// the shape that was intersected
                std::shared_ptr<shape_interface const> object_;

            public:
                intersection_info_t();

                intersection_info_t& inside(bool val);
                intersection_info_t& point(double val);
                intersection_info_t& position(tuple val);
                intersection_info_t& n1(float val);
                intersection_info_t& n2(float val);
                intersection_info_t& over_position(tuple val);
                intersection_info_t& under_position(tuple val);
                intersection_info_t& eye_vector(tuple val);
                intersection_info_t& normal_vector(tuple val);
                intersection_info_t& reflection_vector(tuple val);
                intersection_info_t& what_object(std::shared_ptr<shape_interface const> val);

            public:
                float schlick_approx() const;

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

                float n1() const
                {
                        return ri_n1_;
                }

                float n2() const
                {
                        return ri_n2_;
                }

                tuple over_position() const
                {
                        return over_position_;
                }

                tuple under_position() const
                {
                        return under_position_;
                }

                tuple eye_vector() const
                {
                        return eye_vec_;
                }

                tuple normal_vector() const
                {
                        return normal_vec_;
                }

                tuple reflection_vector() const
                {
                        return reflect_vec_;
                }

                std::shared_ptr<shape_interface const> what_object() const
                {
                        return object_;
                }
        };
} // namespace raytracer
