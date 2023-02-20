#pragma once


/// c++ includes
#include <memory>

/// our includes
#include "primitives/color.hpp"
#include "patterns/pattern_interface.hpp"
#include "patterns/solid_pattern.hpp"

namespace raytracer
{
        template <class T>
        class binary_pattern : public pattern_interface
        {
            private:
                std::shared_ptr<pattern_interface> const pattern_a_;
                std::shared_ptr<pattern_interface> const pattern_b_;

            protected:
                /// ------------------------------------------------------------
                /// don't allow deletion through base
                virtual ~binary_pattern()
                {
                }

            public:
                binary_pattern(color a, color b)
                    : binary_pattern(std::make_shared<solid_pattern>(a), /// convert-to-solid-pattern
                                     std::make_shared<solid_pattern>(b)) /// convert-to-solid-pattern
                {
                }

                binary_pattern(std::shared_ptr<pattern_interface> pattern_a,
                               std::shared_ptr<pattern_interface> pattern_b)
                    : pattern_a_(pattern_a)
                    , pattern_b_(pattern_b)
                {
                }

            protected:
                /// ------------------------------------------------------------
                /// return the first color at a point
                color color_a(tuple const& P) const
                {
                        return pattern_a_->color_at_point(pattern_a_->inv_transform() * P);
                }

                /// ------------------------------------------------------------
                /// return the second color at a point
                color color_b(tuple const& P) const
                {
                        return pattern_b_->color_at_point(pattern_b_->inv_transform() * P);
                }
        };

} // namespace raytracer
