/// c includes
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/// c++ includes
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

/// 3rdparty testing library
#include "doctest/doctest.h"

/// our own thing
#include "common/include/logging.h"
#include "file_utils/mmapped_file_reader.hpp"
#include "io/canvas.hpp"
#include "patterns/uv_image_pattern.hpp"
#include "primitives/color.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

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
        auto c                 = RT::canvas::create_ascii(10, 2);
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
        std::vector<std::string> const have = {
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

/// ----------------------------------------------------------------------------
/// canvas read tests : the general approach is to create a file and fill it
/// with some data and then canvas::load_from_file(...) operate on it.

/// ----------------------------------------------------------------------------
/// ppm file with invalid magic
TEST_CASE("canvas::load_from_file(...) with invalid magic")
{
        std::string_view ppm_data = R"""(P32
1 1
255
0 0 0
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == false);
        unlink(ppm_fname.c_str());

        return;
}

/// ----------------------------------------------------------------------------
/// ppm file with valid magic and validating canvas width and height
TEST_CASE("canvas::load_from_file(...) check canvas width and height")
{
        std::string_view ppm_data = R"""(P3
10 2
255
0 0 0  0 0 0  0 0 0  0 0 0  0 0 0
0 0 0  0 0 0  0 0 0  0 0 0  0 0 0
0 0 0  0 0 0  0 0 0  0 0 0  0 0 0
0 0 0  0 0 0  0 0 0  0 0 0  0 0 0
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 10);
        CHECK(ppm_canvas.height() == 2);

        return;
}

/// ----------------------------------------------------------------------------
/// validate pixel data
TEST_CASE("canvas::load_from_file(...) validate pixel data")
{
        std::string_view ppm_data = R"""(P3
4 3
255
255 127 0  0 127 255  127 255 0  255 255 255
0 0 0  255 0 0  0 255 0  0 0 255
255 255 0  0 255 255  255 0 255  127 127 127
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 4);
        CHECK(ppm_canvas.height() == 3);

        /// --------------------------------------------------------------------
        /// (x, y) pixel, and expected color at that pixel
        struct {
                uint32_t x;
                uint32_t y;
                RT::color pixel_color;
        } const tc_list[] = {
                /// row-0
                {0, 0, RT::color{1.0, 0.498039, 0.0}},
                {1, 0, RT::color{0, 0.498039, 1.0}},
                {2, 0, RT::color{0.498039, 1.0, 0.0}},
                {3, 0, RT::color{1.0, 1.0, 1.0}},

                /// row-1
                {0, 1, RT::color{0.0, 0.0, 0.0}},
                {1, 1, RT::color{1.0, 0.0, 0.0}},
                {2, 1, RT::color{0.0, 1.0, 0.0}},
                {3, 1, RT::color{0.0, 0.0, 1.0}},

                /// row-2
                {0, 2, RT::color{1.0, 1.0, 0.0}},
                {1, 2, RT::color{0.0, 1.0, 1.0}},
                {2, 2, RT::color{1.0, 0.0, 1.0}},
                {3, 2, RT::color{0.498039, 0.498039, 0.498039}},
        };

        for (auto const& tc_expected : tc_list) {
                auto got_pixel_color = ppm_canvas.read_pixel(tc_expected.x, tc_expected.y);
                CHECK(got_pixel_color == tc_expected.pixel_color);
        }

        return;
}

/// ----------------------------------------------------------------------------
/// ignore comment lines
TEST_CASE("canvas::load_from_file(...) ignore comment lines")
{
        std::string_view ppm_data = R"""(P3
# this is a comment
2 1
# this, too
255
# another comment
255 255 255
# oh, no, comments in the pixel data!
255 0 255
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 2);
        CHECK(ppm_canvas.height() == 1);

        /// --------------------------------------------------------------------
        /// (x, y) pixel, and expected color at that pixel
        struct {
                uint32_t x;
                uint32_t y;
                RT::color pixel_color;
        } const tc_list[] = {
                /// row-0
                {0, 0, RT::color{1.0, 1.0, 1.0}},
                {1, 0, RT::color{1.0, 0.0, 1.0}},
        };

        for (auto const& tc_expected : tc_list) {
                auto got_pixel_color = ppm_canvas.read_pixel(tc_expected.x, tc_expected.y);
                CHECK(got_pixel_color == tc_expected.pixel_color);
        }

        return;
}

/// ----------------------------------------------------------------------------
/// allows an RGB triple to span lines
TEST_CASE("canvas::load_from_file(...) allows an RGB triple to span lines")
{
        std::string_view ppm_data = R"""(P3
1 1
255
51
153

204
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 1);
        CHECK(ppm_canvas.height() == 1);

        /// --------------------------------------------------------------------
        /// (x, y) pixel, and expected color at that pixel
        struct {
                uint32_t x;
                uint32_t y;
                RT::color pixel_color;
        } const tc_list[] = {
                /// row-0
                {0, 0, RT::color{0.2, 0.6, 0.8}},
        };

        for (auto const& tc_expected : tc_list) {
                auto got_pixel_color = ppm_canvas.read_pixel(tc_expected.x, tc_expected.y);
                CHECK(got_pixel_color == tc_expected.pixel_color);
        }

        return;
}

/// ----------------------------------------------------------------------------
/// parsing respects the scale setting
TEST_CASE("canvas::load_from_file(...) parsing respects the scale setting")
{
        std::string_view ppm_data = R"""(P3
2 2
100
100 100 100  50 50 50
75 50 25  0 0 0
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 2);
        CHECK(ppm_canvas.height() == 2);

        /// --------------------------------------------------------------------
        /// (x, y) pixel, and expected color at that pixel
        struct {
                uint32_t x;
                uint32_t y;
                RT::color pixel_color;
        } const tc_list[] = {
                /// row-0
                {0, 0, RT::color{1.0, 1.0, 1.0}},
                {1, 0, RT::color{0.5, 0.5, 0.5}},

                /// row-1
                {0, 1, RT::color{0.75, 0.5, 0.25}},
                {1, 1, RT::color{0.0, 0.0, 0.0}},
        };

        for (auto const& tc_expected : tc_list) {
                auto got_pixel_color = ppm_canvas.read_pixel(tc_expected.x, tc_expected.y);
                CHECK(got_pixel_color == tc_expected.pixel_color);
        }

        return;
}

/// ----------------------------------------------------------------------------
/// test checkers pattern as 2d canvas
TEST_CASE("uv_image checkers patterned canvas")
{
        std::string_view ppm_data = R"""(P3
10 10
10
0 0 0  1 1 1  2 2 2  3 3 3  4 4 4  5 5 5  6 6 6  7 7 7  8 8 8  9 9 9
1 1 1  2 2 2  3 3 3  4 4 4  5 5 5  6 6 6  7 7 7  8 8 8  9 9 9  0 0 0
2 2 2  3 3 3  4 4 4  5 5 5  6 6 6  7 7 7  8 8 8  9 9 9  0 0 0  1 1 1
3 3 3  4 4 4  5 5 5  6 6 6  7 7 7  8 8 8  9 9 9  0 0 0  1 1 1  2 2 2
4 4 4  5 5 5  6 6 6  7 7 7  8 8 8  9 9 9  0 0 0  1 1 1  2 2 2  3 3 3
5 5 5  6 6 6  7 7 7  8 8 8  9 9 9  0 0 0  1 1 1  2 2 2  3 3 3  4 4 4
6 6 6  7 7 7  8 8 8  9 9 9  0 0 0  1 1 1  2 2 2  3 3 3  4 4 4  5 5 5
7 7 7  8 8 8  9 9 9  0 0 0  1 1 1  2 2 2  3 3 3  4 4 4  5 5 5  6 6 6
8 8 8  9 9 9  0 0 0  1 1 1  2 2 2  3 3 3  4 4 4  5 5 5  6 6 6  7 7 7
9 9 9  0 0 0  1 1 1  2 2 2  3 3 3  4 4 4  5 5 5  6 6 6  7 7 7  8 8 8
)""";

        std::string ppm_fname = file_utils::fill_file_with_data(ppm_data);
        CHECK(ppm_fname.size() != 0);

        auto maybe_ppm_canvas = RT::canvas::load_from_file(ppm_fname);
        CHECK(maybe_ppm_canvas.has_value() == true);
        unlink(ppm_fname.c_str());

        auto ppm_canvas = maybe_ppm_canvas.value();
        CHECK(ppm_canvas.width() == 10);
        CHECK(ppm_canvas.height() == 10);

        /// --------------------------------------------------------------------
        /// (u, v) point on the image
        struct {
                float u;
                float v;
                RT::color pixel_color;
        } const tc_list[] = {
                {0.0, 0.0, RT::color{0.9, 0.9, 0.9}},
                {0.3, 0.0, RT::color{0.2, 0.2, 0.2}},
                {0.6, 0.3, RT::color{0.1, 0.1, 0.1}},
                {1.0, 1.0, RT::color{0.9, 0.9, 0.9}},
        };

        RT::uv_image const test_uv_img(ppm_canvas);

        for (auto const& tc_expected : tc_list) {
                auto const got_uv_pixel_color =
                        test_uv_img.uv_pattern_color_at(RT::uv_point{tc_expected.u,   /// u-value
                                                                     tc_expected.v}); /// v-value

                CHECK(got_uv_pixel_color == tc_expected.pixel_color);
        }

        return;
}
