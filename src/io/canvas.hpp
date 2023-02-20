#pragma once

/*
 * this file implements the canvas i.e. the virtual drawing board on which we
 * visualize our creations !
 *
 * canvas generates images in the ppm (portable pixmap) format. specifically
 * both ppm3 (ascii) and ppm6 (binary) encoded images are supported.
 *
 * sdl2 based renderer can be brought to bear (beer ?) for 'online'
 * visualization as well.
 **/

/// c++ includes
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

/// our includes
#include "common/include/logging.h"
#include "primitives/color.hpp"

namespace raytracer
{
        /// ------------------------------------------------------------
        /// what types of canvas we support
        enum canvas_type {
                PPM_CANVAS_INVALID = 0,
                PPM_CANVAS_ASCII   = 1,
                PPM_CANVAS_BINARY  = 2,
        };

        /// ----------------------------------------------------------------------------
        /// netpbm based color canvas
        class canvas final
        {
            private:
                std::size_t const width_{0};
                std::size_t const height_{0};
                canvas_type const type_{PPM_CANVAS_INVALID};
                std::vector<color> buf_;

            public:
                /// ------------------------------------------------------------
                /// named constructors
                static canvas create_ascii(std::size_t width, std::size_t height);
                static canvas create_binary(std::size_t width, std::size_t height);

                /// ------------------------------------------------------------
                /// read a ppm file and generate a canvas
                static std::optional<canvas> load_from_file(std::string file_name);

            public:
                /// ------------------------------------------------------------
                /// ACCESSORS

                constexpr std::size_t width() const
                {
                        return this->width_;
                }

                constexpr std::size_t height() const
                {
                        return this->height_;
                }

                color read_pixel(size_t x, size_t y) const
                {
                        return this->buf_[x + y * this->width_];
                }

                /// meta information about the canvas
                std::string stringify() const;

                /// save canvas to a persistent store
                void write(std::string const& fname) const;

                /// display contents of canvas using sdl2
                void show() const;

            public:
                /// ------------------------------------------------------------
                /// MUTATORS

                void write_pixel(size_t x, size_t y, color c)
                {
                        this->buf_[x + y * this->width_] = c;
                }

            private:
                /// ------------------------------------------------------------
                /// ACCESSORS

                constexpr std::size_t ppm_num_values() const
                {
                        /// 'this->width_ * 3' ∵ 3 bytes per pixel
                        return width() * 3;
                }

            private:
                /// ∵ named-constructor idiom
                canvas(std::size_t width, std::size_t height, canvas_type type);

            private:
                /// ------------------------------------------------------------
                /// save the to a persistent store
                void write_binary(std::string const&) const;
                void write_ascii(std::string const&) const;

                void ppm_color_at(size_t x, size_t y, unsigned char& r, unsigned char& g,
                                  unsigned char& b) const;

                std::unique_ptr<unsigned char[]> get_ppm_row(size_t y) const;
        };

} // namespace raytracer
