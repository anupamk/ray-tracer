/*
 * this program generates various textures procedurally.
 **/

/// c++ includes
#include <chrono>
#include <cstddef>
#include <random>

/// our includes
#include "io/canvas.hpp"
#include "patterns/checkers_pattern.hpp"
#include "patterns/gradient_pattern.hpp"
#include "patterns/perlin_noise.hpp"
#include "patterns/perlin_noise_pattern.hpp"
#include "primitives/color.hpp"
#include "utils/constants.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT = raytracer;

class noisy_color
{
    private:
        RT::color start_;
        RT::color end_;
        RT::perlin_noise pn_;
        uint32_t octaves_;

    public:
        noisy_color(RT::color start, RT::color end, uint32_t seed, uint32_t octaves)
            : start_(start)
            , end_(end)
            , pn_(RT::perlin_noise(seed))
            , octaves_(octaves)
        {
        }

    public:
        /// --------------------------------------------------------------------
        /// return linear interpolation of color from [start, end] depending on
        /// the generated noise
        RT::color linear_noisy_color(double x, double y, double z) const
        {
                double const noise = pn_.octave_noise_3d_clamped_01(x, y, z, octaves_);

                constexpr double noise_min   = 0.0;
                constexpr double noise_max   = 1.0;
                constexpr double noise_range = (noise_max - noise_min);

                RT::color const color_start_val = start_ * (noise_max - noise);
                RT::color const color_end_val   = end_ * (noise - noise_min);
                RT::color const total_color     = (color_start_val + color_end_val) / noise_range;

                return RT::color(total_color.R(), total_color.G(), total_color.B());
        }
};

int main(int argc, char** argv)
{
        std::mt19937 rnd_gen(time(nullptr));
        std::uniform_int_distribution<unsigned> distrib(std::numeric_limits<unsigned>::min(),
                                                        std::numeric_limits<unsigned>::max());

        /// --------------------------------------------------------------------
        /// generate MAX_NUMBER_OF_CANVASES each with CANVAS_XSIZE_PIXELS X
        /// CANVAS_YSIZE_PIXELS. stitch them together for a movie !
        constexpr size_t CANVAS_XSIZE_PIXELS    = 1280;
        constexpr size_t CANVAS_YSIZE_PIXELS    = 1024;
        constexpr size_t MAX_NUMBER_OF_CANVASES = 1;

        auto gp = std::make_shared<RT::checkers_pattern>(RT::color::RGB(0x00, 0x00, 0xff),  /// start-color
                                                         RT::color::RGB(0xff, 0xff, 0x00)); /// end-color

        for (size_t c = 0; c < MAX_NUMBER_OF_CANVASES; c++) {
                RT::canvas ith_canvas = RT::canvas::create_binary(CANVAS_XSIZE_PIXELS, CANVAS_YSIZE_PIXELS);

                constexpr double frequency = 1.0;
                const double fx            = ith_canvas.width() / frequency;
                const double fy            = ith_canvas.height() / frequency;

                RT::perlin_noise_pattern n(gp,               /// gradient-pattern
                                           distrib(rnd_gen), /// a new-random-number
                                           1);               /// octaves

                /// ------------------------------------------------------------
                /// generate the canvas
                for (size_t y = 0; y < ith_canvas.height(); y++) {
                        for (size_t x = 0; x < ith_canvas.width(); x++) {
                                auto xy_color = n.color_at_point(RT::create_point(x / fx, y / fy, 0.0));
                                ith_canvas.write_pixel(x, y, xy_color);
                        }
                }

                /// ------------------------------------------------------------
                /// save it
                char fname[1024] = {'\0'};
                snprintf(fname, 1024, "./noisy-textures/simple-noise-%06zu.ppm", c);
                ith_canvas.write(fname);

                LOG_INFO("canvas: '%s' %06zu/%06zu generated", fname, c, MAX_NUMBER_OF_CANVASES);
        }

        return 0;
}
