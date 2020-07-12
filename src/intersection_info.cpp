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
            , over_position_(create_point(0.0, 0.0, 0.0))
            , eye_vec_(create_vector(0.0, 0.0, 0.0))
            , normal_vec_(create_vector(0.0, 0.0, 0.0))
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

        intersection_info_t& intersection_info_t::over_position(tuple val)
        {
                over_position_ = val;
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

        intersection_info_t& intersection_info_t::what_object(std::shared_ptr<shape_interface const> val)
        {
                object_ = val;
                return *this;
        }

} // namespace raytracer
