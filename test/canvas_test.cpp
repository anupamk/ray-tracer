/// c++ includes
#include <cstddef>
#include <fstream>
#include <string>
#include <vector>

/// 3rdparty testing library
#include "doctest.h"

/// our own thing
#include "constants.hpp"
#include "color.hpp"
#include "canvas.hpp"
#include "logging.h"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// a convenience alias
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// canvas creation
TEST_CASE("canvas::canvas(...)")
{
        size_t const width  = 20;
        size_t const height = 10;

        auto c = RT::canvas::create_ascii(width, height);

        /// basic checks
        CHECK(c.width() == width);
        CHECK(c.height() == height);

        /// check that all pixels are default initialized to black
        for (size_t y = 0; y < height; y++) {
                for (size_t x = 0; x < width; x++) {
                        CHECK(c.read_pixel(x, y) == RT::color_black());
                }
        }
}

/// ----------------------------------------------------------------------------
/// canvas writing
TEST_CASE("canvas::read_pixel(...)/write_pixel(...)")
{
        RT::canvas c = RT::canvas::create_ascii(10, 20);

        /// write + read
        const auto red = RT::color_red();
        c.write_pixel(2, 3, red);
        const auto pixel_color = c.read_pixel(2, 3);

        CHECK(pixel_color == red);
}

/// ----------------------------------------------------------------------------
/// canvas generate ppm string
TEST_CASE("canvas::save(...)")
{
        auto c		       = RT::canvas::create_ascii(10, 2);
        auto const pixel_color = RT::color(1.0, 0.8, 0.6);

        /// color all pixels with a specific color
        for (size_t y = 0; y < c.height(); y++) {
                for (size_t x = 0; x < c.width(); x++) {
                        c.write_pixel(x, y, pixel_color);
                }
        }

        /// save the canvas data
        auto const canvas_fname = "/tmp/rt-test-canvas";
        c.write(canvas_fname);

        /// expected data format
        const std::vector<std::string> have = {
                "P3",
                "10 2",
                "255",
                "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153",
                "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153",
                "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153",
                "255 204 153 255 204 153 255 204 153 255 204 153 255 204 153",
                "",
        };

        /// validate saved line-by-line against what we expect
        std::ifstream canvas_data(canvas_fname);

        // clang-format off
        for (struct {size_t line; std::string got; } v = {0, ""}; // for index+data
             std::getline(canvas_data, v.got);
             ++v.line) {
                CHECK(v.got == have[v.line]);
        }
        // clang-format on
}
