#pragma once

/*
 * this file implements the canvas i.e. the virtual drawing board on which we
 * visualize our creations !
 *
 * canvas generates images in the ppm (portable pixmap) format. specifically
 * both ppm3 (ascii) and ppm6 (binary) encoded images are supported.
 *
 * x11 based renderer can be brought to bear (beer ?) for 'online'
 * visualization as well.
 **/

/// c++ includes
#include <cstddef>
#include <memory>
#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

/// our includes
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
                /*
                 * @brief
                 *    common canvas size for all renders. ofcourse, these sizes
                 *    are not cast in stone, and other ones can be chosen as
                 *    well.
                 *
                 *    for now, by default, we render all images at 2k
                 *    resolution.
                 **/
                static constexpr uint32_t X_PIXELS_2K = 2560;
                static constexpr uint32_t Y_PIXELS_2K = 1440;

                /*
                 * @brief
                 *    1080p or 1k resolution for those demanding scenes that
                 *    just take too long.
                 **/
                static constexpr uint32_t X_PIXELS_1K = 1920;
                static constexpr uint32_t Y_PIXELS_1K = 1080;

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

                /// ------------------------------------------------------------
                /// read || write the color of a pixel at (x, y) location on the
                /// canvas.
                color read_pixel(size_t x, size_t y) const;
                void write_pixel(uint32_t x, uint32_t y, color const& c);

                /// meta information about the canvas
                std::string stringify() const;

                /// save canvas to a persistent store
                void write(std::string const& fname) const;

            private:
                /// ------------------------------------------------------------
                /// ACCESSORS

                constexpr std::size_t ppm_num_values() const
                {
                        /// 'this->width_ * 3' ∵ 3 bytes per pixel
                        return width() * 3;
                }

            private:
                /// ------------------------------------------------------------
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
