/*
 *  implement raytracer shape material
 **/

#include "patterns/material.hpp"

/// c++ includes
#include <ostream>
#include <string>

/// our includes
#include "patterns/pattern_interface.hpp"
#include "patterns/solid_pattern.hpp"
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class shape_interface;

        /// --------------------------------------------------------------------
        /// a default material has a solid pattern
        material::material()
            : ambient_(0.1)
            , diffuse_(0.9)
            , specular_(0.9)
            , shininess_(200.0)
            , reflective_(0.0)
            , refractive_index_(RI_VACCUM)
            , transparency_(0.0)
            , pattern_(std::make_shared<solid_pattern>())
        {
        }

        /// --------------------------------------------------------------------
        /// all accessors

        float material::get_ambient() const
        {
                return this->ambient_;
        }

        float material::get_diffuse() const
        {
                return this->diffuse_;
        }

        float material::get_specular() const
        {
                return this->specular_;
        }

        float material::get_shininess() const
        {
                return this->shininess_;
        }

        float material::get_reflective() const
        {
                return this->reflective_;
        }

        float material::get_refractive_index() const
        {
                return this->refractive_index_;
        }

        float material::get_transparency() const
        {
                return this->transparency_;
        }

        color material::get_color(std::shared_ptr<shape_interface const> a_shape, tuple const& pt) const
        {
                return this->pattern_->color_at_shape(a_shape, pt);
        }

        std::shared_ptr<pattern_interface> material::get_pattern() const
        {
                return this->pattern_;
        }

        /// --------------------------------------------------------------------
        /// stringified representation of the material
        std::string material::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << std::fixed << std::left
                   << "ambient: "    << this->get_ambient()                << ", "
                   << "diffuse: "    << this->get_diffuse()                << ", "
                   << "specular: "   << this->get_specular()               << ", "
                   << "shininess: "  << this->get_shininess()              << ", "
                   << "reflective: " << this->get_reflective()             << ", "
                   << "refractive-index: " << this->get_refractive_index() << ", "
                   << "transparency: " << this->get_transparency()
                   ;
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// all the mutators

        material& material::set_ambient(float val)
        {
                this->ambient_ = val;
                return *this;
        }

        material& material::set_diffuse(float val)
        {
                this->diffuse_ = val;
                return *this;
        }

        material& material::set_specular(float val)
        {
                this->specular_ = val;
                return *this;
        }

        material& material::set_shininess(float val)
        {
                this->shininess_ = val;
                return *this;
        }

        material& material::set_reflective(float val)
        {
                this->reflective_ = val;
                return *this;
        }

        material& material::set_refractive_index(float val)
        {
                this->refractive_index_ = val;
                return *this;
        }

        material& material::set_transparency(float val)
        {
                this->transparency_ = val;
                return *this;
        }

        material& material::set_pattern(std::shared_ptr<pattern_interface> val)
        {
                this->pattern_ = val;
                return *this;
        }

        /// --------------------------------------------------------------------
        /// overloaded operators

        std::ostream& operator<<(std::ostream& os, material const& M)
        {
                return os << M.stringify();
        }

        /// --------------------------------------------------------------------
        /// two materials are equal when they have same values for all phong
        /// attributes as well as the associated pattern.
        bool operator==(material const& lhs, material const& rhs)
        {
                auto const origin    = create_point(0.0, 0.0, 0.0);
                auto const lhs_color = lhs.get_pattern()->color_at_point(origin);
                auto const rhs_color = rhs.get_pattern()->color_at_point(origin);

                // clang-format off
                return ((lhs.get_ambient()          == rhs.get_ambient())          &&
                        (lhs.get_diffuse()          == rhs.get_diffuse())          &&
                        (lhs.get_specular()         == rhs.get_specular())         &&
                        (lhs.get_shininess()        == rhs.get_shininess())        &&
                        (lhs.get_reflective()       == rhs.get_reflective())       &&
                        (lhs.get_refractive_index() == rhs.get_refractive_index()) &&
                        (lhs.get_transparency()     == rhs.get_transparency())     &&
                        (lhs_color                  == rhs_color));
                // clang-format on
        }

} // namespace raytracer
