#pragma once

/// c++ includes
#include <memory>
#include <string>

/// our includes
#include "patterns/pattern_interface.hpp"
#include "primitives/color.hpp"
#include "primitives/uv_point.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class tuple;
        class uv_pattern_interface;

        /*
         * define various faces of a cube in cross-format notation:
         *
         *        +---+                     U: up     (+y)
         *        | U |                     L: left   (-x)
         *    +---+---+---+---+             F: front  (+z)
         *    | L | F | R | B |             R: right  (+x)
         *    +---+---+---+---+             B: back   (-z)
         *        | D |                     D: down   (-y)
         *        +---+
         *
         * so, the 'up' face is along the '+y' axes, 'left' face is along the
         * '-x' axes etc. (we are using right-handed-coordinate system)
         *
         * makes it easier to manage operations specific to a face f.e. the uv
         * texture mapping etc.
         **/
        enum cube_face_enum_values {
                CUBE_FACE_INVALID = 0,
                CUBE_FACE_LEFT    = 1,
                CUBE_FACE_RIGHT   = 2,
                CUBE_FACE_UP      = 3,
                CUBE_FACE_DOWN    = 4,
                CUBE_FACE_FRONT   = 5,
                CUBE_FACE_BACK    = 6,
        };

        /*
         * wrap it in a class for ease-of-use
         **/
        class cube_face final
        {
            private:
                cube_face_enum_values value_;
                cube_face()
                {
                }

            public:
                cube_face(cube_face_enum_values v)
                    : value_(v)
                {
                }

                operator cube_face_enum_values() const
                {
                        return value_;
                }

                cube_face& operator=(cube_face_enum_values v)
                {
                        value_ = v;
                        return *this;
                }

                bool operator==(cube_face_enum_values const v) const
                {
                        return value_ == v;
                }

                bool operator!=(cube_face_enum_values const v) const
                {
                        return value_ != v;
                }

                /// ------------------------------------------------------------
                /// convert to string
                std::string stringify() const
                {
                        switch (value_) {
                        case CUBE_FACE_LEFT:
                                return "CUBE_FACE_LEFT";

                        case CUBE_FACE_RIGHT:
                                return "CUBE_FACE_RIGHT";

                        case CUBE_FACE_UP:
                                return "CUBE_FACE_UP";

                        case CUBE_FACE_DOWN:
                                return "CUBE_FACE_DOWN";

                        case CUBE_FACE_FRONT:
                                return "CUBE_FACE_FRONT";

                        case CUBE_FACE_BACK:
                                return "CUBE_FACE_BACK";

                        default:
                                break;
                        }

                        return "CUBE_FACE_INVALID";
                }
        };

        /*
         * this is the "cube-map" and is used for texturing a cube.
         **/
        class cube_texture final : public pattern_interface
        {
            private:
                /// ------------------------------------------------------------
                /// each face of the cube can potentially have a different
                /// texture.
                std::shared_ptr<uv_pattern_interface> const pattern_left_face_;
                std::shared_ptr<uv_pattern_interface> const pattern_front_face_;
                std::shared_ptr<uv_pattern_interface> const pattern_right_face_;
                std::shared_ptr<uv_pattern_interface> const pattern_back_face_;
                std::shared_ptr<uv_pattern_interface> const pattern_up_face_;
                std::shared_ptr<uv_pattern_interface> const pattern_down_face_;

            public:
                // clang-format off
		cube_texture(std::shared_ptr<uv_pattern_interface> left,
		         std::shared_ptr<uv_pattern_interface> front,
		         std::shared_ptr<uv_pattern_interface> right,
		         std::shared_ptr<uv_pattern_interface> back,
                         std::shared_ptr<uv_pattern_interface> up,
		         std::shared_ptr<uv_pattern_interface> down);
                // clang-format on

            public:
                /// ------------------------------------------------------------
                /// return the color at a specific point on the pattern
                color color_at_point(tuple const& pt) const override;

            private:
                /// --------------------------------------------------------------------
                /// this function is called to find the face upon which the point lies
                cube_face face_from_point(tuple const&) const;

                /// --------------------------------------------------------------------
                /// map a point on the cube to a corresponding uv point. one
                /// function for each face
                uv_point uv_map_front(tuple const&) const;
                uv_point uv_map_back(tuple const&) const;
                uv_point uv_map_up(tuple const&) const;
                uv_point uv_map_down(tuple const&) const;
                uv_point uv_map_left(tuple const&) const;
                uv_point uv_map_right(tuple const&) const;
        };

} // namespace raytracer
