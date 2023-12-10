/*
 * implement the raytracer canvas
 **/

#include "io/canvas.hpp"

/// c++ includes
#include <math.h>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>

/// our includes
#include "primitives/color.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        /// convenience
        using ppm_row_data_t = std::unique_ptr<unsigned char[]>;

        /// --------------------------------------------------------------------
        /// stringified representation of canvas-type instance
        static inline constexpr const char* stringify_canvas_enum(canvas_type ct)
        {
                switch (ct) {
                case PPM_CANVAS_ASCII:
                        return "PPM_CANVAS_ASCII";

                case PPM_CANVAS_BINARY:
                        return "PPM_CANVAS_BINARY";

                default:
                        break;
                }

                return "PPM_CANVAS_INVALID";
        }

        /// --------------------------------------------------------------------
        /// create an ascii canvas
        canvas canvas::create_ascii(std::size_t width, std::size_t height)
        {
                return canvas(width, height, PPM_CANVAS_ASCII);
        }

        /// --------------------------------------------------------------------
        /// create a binary canvas
        canvas canvas::create_binary(std::size_t width, std::size_t height)
        {
                return canvas(width, height, PPM_CANVAS_BINARY);
        }

        /// --------------------------------------------------------------------
        /// meta information about the canvas
        std::string canvas::stringify() const
        {
                std::stringstream ss("");

                // clang-format off
                ss << "{"
                   << "width: "  << this->width_  << ", "
                   << "height: " << this->height_ << ", "
                   << "type: "   << stringify_canvas_enum(type_)
                   << "}";
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// color-value of a point (x, y) on the canvas
        color canvas::read_pixel(size_t x, size_t y) const
        {
                return this->buf_[x + y * this->width_];
        }

        /// --------------------------------------------------------------------
        /// write the pixel data, and display it as well.
        void canvas::write_pixel(uint32_t x, uint32_t y, color const& c)
        {
                this->buf_[x + y * this->width_] = c;
        }

        /*
         * only private functions from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// create a canvas using named-ctor-idiom
        canvas::canvas(std::size_t width, std::size_t height, canvas_type type)
            : width_(width)
            , height_(height)
            , type_(type)
            , buf_(std::vector<color>(width * height))
        {
        }

        /// --------------------------------------------------------------------
        /// convert canvas 'color' to equivalent ppm values
        void canvas::ppm_color_at(size_t x,               /// x-location
                                  size_t y,               /// y-location
                                  unsigned char& r,       /// r (0..255)
                                  unsigned char& g,       /// g (0..255)
                                  unsigned char& b) const /// b (0..255)
        {
                auto pixel_color = this->read_pixel(x, y);

                r = roundf(clamp_in_range(pixel_color.R(), 0.0f, 1.0f) * 255.0);
                g = roundf(clamp_in_range(pixel_color.G(), 0.0f, 1.0f) * 255.0);
                b = roundf(clamp_in_range(pixel_color.B(), 0.0f, 1.0f) * 255.0);

                return;
        }

        /// --------------------------------------------------------------------
        /// get ppm encoded row of values
        ppm_row_data_t canvas::get_ppm_row(size_t y) const
        {
                auto ppm_colors     = std::make_unique<unsigned char[]>(ppm_num_values());
                auto ppm_colors_buf = ppm_colors.get();

                for (size_t i = 0, x = 0; i < ppm_num_values(); x++) {
                        ppm_color_at(x, y,
                                     ppm_colors_buf[i + 0],  /// r
                                     ppm_colors_buf[i + 1],  /// g
                                     ppm_colors_buf[i + 2]); /// b

                        i += 3;
                }

                return ppm_colors;
        }

} // namespace raytracer
