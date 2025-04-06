/*
 * implement raytracer stock materials
 **/

#include "patterns/stock_materials.hpp"

/// c++ includes
#include <limits>
#include <memory>
#include <random>
#include <time.h>
#include <utility>

/// our includes
#include "patterns/gradient_perlin_noise_pattern.hpp"
#include "patterns/perlin_noise_pattern.hpp"
#include "patterns/solid_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/tuple.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this function is called to return an instance of a "white glass"
        /// material.
        material create_material_transparent_glass()
        {
                return create_material_colored_glass(color(0.1, 0.1, 0.1));
        }

        /// --------------------------------------------------------------------
        /// this function is called to return an instance of a colored glass
        /// material.
        material create_material_colored_glass(color const& c)
        {
                auto solid_color_pattern = std::make_shared<solid_pattern>(c);
                return material()
                        .set_ambient(0.0)
                        .set_diffuse(0.4)
                        .set_specular(0.9)
                        .set_shininess(300)
                        .set_reflective(0.9)
                        .set_transparency(0.9)
                        .set_refractive_index(material::RI_GLASS)
                        .set_pattern(std::move(solid_color_pattern));
        }

        /// --------------------------------------------------------------------
        /// this function is called to return an instance of a matte material.
        material create_material_matte(color const& c)
        {
                auto solid_color_pattern = std::make_shared<solid_pattern>(c);
                return material().set_pattern(std::move(solid_color_pattern));
        }

        /// --------------------------------------------------------------------
        /// create a canvas (of specific size) of a noisy texture
        canvas generate_noisy_texture(size_t canvas_xpixels, size_t canvas_ypixels, color const& start_color,
                                      color const& end_color)
        {
                std::mt19937 rnd_gen(time(nullptr));
                std::uniform_int_distribution<unsigned> distrib(std::numeric_limits<unsigned>::min(),
                                                                std::numeric_limits<unsigned>::max());

                auto noisy_pattern = std::make_shared<gradient_perlin_noise_pattern>(start_color, end_color);

                perlin_noise_pattern n(noisy_pattern,    /// pattern
                                       distrib(rnd_gen), /// a new-random-number
                                       8);               /// octaves

                canvas canvas_retval       = canvas::create_binary(canvas_xpixels, canvas_ypixels);
                constexpr double frequency = 1.0;
                double const fx            = canvas_retval.width() / frequency;
                double const fy            = canvas_retval.height() / frequency;

                for (size_t y = 0; y < canvas_retval.height(); y++) {
                        for (size_t x = 0; x < canvas_retval.width(); x++) {
                                auto xy_color = n.color_at_point(create_point(x / fx, y / fy, 0.0));
                                canvas_retval.write_pixel(x, y, xy_color);
                        }
                }

                return canvas_retval;
        }

} // namespace raytracer
