/*
 * this program generates various textures procedurally.
**/

/// c++ includes
#include <cstddef>

/// our includes
#include "color.hpp"
#include "canvas.hpp"
#include "constants.hpp"
#include "perlin_noise.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
**/
log_level_t LOG_LEVEL_NOW = LOG_LEVEL_INFO;

/// convenience mostly
namespace RT = raytracer;

static size_t const CANVAS_XSIZE_PIXELS = 1280;
static size_t const CANVAS_YSIZE_PIXELS = 1024;

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
	/// return linear interpolation of color from start .. end depending on
	/// the generated noise
	RT::color linear_noisy_color(double x, double y, double z) const
	{
		constexpr double noise_min   = 0.0;
		constexpr double noise_max   = 1.0;
		constexpr double noise_range = (noise_max - noise_min);

		double const noise = pn_.octave_noise_3d_clamped_01(x, y, z, octaves_);

		RT::color const color_start_val = start_ * (noise_max - noise);
		RT::color const color_end_val   = end_ * (noise - noise_min);
		RT::color const total_color     = color_start_val + color_end_val;

		return RT::color(total_color.R() / noise_range, total_color.G() / noise_range,
		                 total_color.B() / noise_range);
	}
};

int main(int argc, char** argv)
{
	noisy_color n(RT::color(0.0, 0.0, 0.0),                 /// start-color
	              RT::color(1.0, 69. / 255., 0.0),          /// end-color
	              std::default_random_engine::default_seed, /// seed
	              16);                                      /// octaves

	double frequency     = 2.0;
	RT::canvas canvas_01 = RT::canvas::create_binary(CANVAS_XSIZE_PIXELS, CANVAS_YSIZE_PIXELS);

	const double fx = canvas_01.width() / frequency;
	const double fy = canvas_01.height() / frequency;

	for (size_t y = 0; y < canvas_01.height(); y++) {
		for (size_t x = 0; x < canvas_01.width(); x++) {
			auto xy_color = n.linear_noisy_color(x / fx, y / fy, 0.0);
			canvas_01.write_pixel(x, y, xy_color);
		}
	}

	std::string fname = "simple-noise-" + std::to_string(frequency) + ".ppm";
	canvas_01.write(fname);

	LOG_INFO("saved file: '%s'", fname.c_str());

	return 0;
}
