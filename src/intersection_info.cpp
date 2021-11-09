#include "intersection_info.hpp"
#include "tuple.hpp"

namespace raytracer
{
        /// ----------------------------------------------------------------------------
        /// a default constructed intersection info
        intersection_info_t::intersection_info_t()
            : inside_(false)
            , point_(0.0)
            , position_(create_point(0.0, 0.0, 0.0))
            , ri_n1_(0.0)
            , ri_n2_(0.0)
            , over_position_(create_point(0.0, 0.0, 0.0))
            , under_position_(create_point(0.0, 0.0, 0.0))
            , eye_vec_(create_vector(0.0, 0.0, 0.0))
            , normal_vec_(create_vector(0.0, 0.0, 0.0))
            , reflect_vec_(create_vector(0.0, 0.0, 0.0))
            , object_(nullptr)
        {
        }

        intersection_info_t& intersection_info_t::inside(bool val)
        {
                inside_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::point(double val)
        {
                point_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::position(tuple val)
        {
                position_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::n1(float val)
        {
                ri_n1_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::n2(float val)
        {
                ri_n2_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::over_position(tuple val)
        {
                over_position_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::under_position(tuple val)
        {
                under_position_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::eye_vector(tuple val)
        {
                eye_vec_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::normal_vector(tuple val)
        {
                normal_vec_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::reflection_vector(tuple val)
        {
                reflect_vec_ = val;
                return *this;
        }

        intersection_info_t& intersection_info_t::what_object(std::shared_ptr<shape_interface const> val)
        {
                object_ = val;
                return *this;
        }

        /// --------------------------------------------------------------------
        /// compute the schlick approximation
        float intersection_info_t::schlick_approx() const
        {
                /// find cos of angle between eye and normal vector
                auto cos          = dot(eye_vector(), normal_vector());
                auto const n1_val = n1();
                auto const n2_val = n2();

                /// total-internal-reflection can occur only when n1 > n2
                if (n1_val > n2_val) {
                        float const n         = n1_val / n2_val;
                        float const sin_sqr_t = n * n * (1.0 - (cos * cos));

                        if (sin_sqr_t > 1.0) {
                                return 1.0;
                        }

                        /// compute cos_t
                        auto const cos_t = std::sqrt(1.0 - sin_sqr_t);

                        /// when n1 > n2, we use the cos_t instead of cos
                        cos = cos_t;
                }

                float r0 = std::pow((n1_val - n2_val) / (n1_val + n2_val), 2.0);
                return r0 + (1.0 - r0) * std::pow((1.0 - cos), 5.0);
        }

} // namespace raytracer
