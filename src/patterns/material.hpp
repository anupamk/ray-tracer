#pragma once

/// c++ includes
#include <memory>
#include <ostream>
#include <string>

/// our includes
#include "patterns/pattern_interface.hpp"
#include "primitives/color.hpp"

namespace raytracer
{
        /*
         * this defines the material which makes up a shape. it contains
         * attributes from the phong reflection model as well the pattern that
         * makes up the material.
         **/
        class material final
        {
            private:
                /// ------------------------------------------------------------
                /// phong reflection-model properties
                float ambient_;
                float diffuse_;
                float specular_;
                float shininess_;

                /// ------------------------------------------------------------
                /// other properties
                float reflective_;
                float refractive_index_;
                float transparency_;

                /// ------------------------------------------------------------
                /// material pattern
                std::shared_ptr<pattern_interface> pattern_;

            public:
                /// ------------------------------------------------------------
                /// refractive index of different materials
                static constexpr double RI_VACCUM  = 1.0;
                static constexpr double RI_AIR     = 1.00029;
                static constexpr double RI_WATER   = 1.333;
                static constexpr double RI_GLASS   = 1.52;
                static constexpr double RI_DIAMOND = 2.417;

            public:
                explicit material();

            public:
                color get_color(std::shared_ptr<shape_interface const>, tuple const&) const;
                std::shared_ptr<pattern_interface> get_pattern() const;

                /// getters
                float get_ambient() const;
                float get_diffuse() const;
                float get_specular() const;
                float get_shininess() const;
                float get_reflective() const;
                float get_refractive_index() const;
                float get_transparency() const;

                /// setters
                material& set_ambient(float val);
                material& set_diffuse(float val);
                material& set_specular(float val);
                material& set_shininess(float val);
                material& set_pattern(std::shared_ptr<pattern_interface> val);
                material& set_reflective(float val);
                material& set_refractive_index(float val);
                material& set_transparency(float val);

            public:
                /// stringified representation of the material
                std::string stringify() const;
        };

        /// --------------------------------------------------------------------
        /// few useful overloaded operators
        bool operator==(material const& lhs, material const& rhs);
        std::ostream& operator<<(std::ostream& os, material const& M);

} // namespace raytracer
