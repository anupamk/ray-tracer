/*
 * implement the raytracer canvas
**/

/// c++ includes
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <sstream>
#include <string>
#include <typeindex>
#include <utility>

/// sdl2 includes
#include <SDL2/SDL.h>

/// our includes
#include "canvas.hpp"
#include "color.hpp"
#include "string_utils.hpp"
#include "utils.hpp"

namespace raytracer
{
	/// convenience
	using ppm_row_data_t = std::unique_ptr<unsigned char[]>;

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
		auto type_index = static_cast<int>(this->type_);

		// clang-format off
                ss << "{"
                   << "width: "  << this->width_  << ", "
                   << "height: " << this->height_ << ", "
                   << "type: "   << canvas_type_str_[type_index]
                   << "}";
		// clang-format on

		return ss.str();
	}

	/// --------------------------------------------------------------------
	/// display the contents of the canvas using sdl2
	void canvas::show() const
	{
		/// do it pixel-by-pixel there MUST be a faster way
		auto sdl2_paint_canvas = [&](SDL_Renderer* renderer) {
			for (size_t y = 0; y < this->height_; y++) {
				for (size_t x = 0; x < this->width_; x++) {
					unsigned char ir, ig, ib;
					this->ppm_color_at(x, y, ir, ig, ib);

					SDL_SetRenderDrawColor(renderer, ir, ig, ib, 255);
					SDL_RenderDrawPoint(renderer, x, y);
				}
			}
		};

		SDL_Renderer* renderer = nullptr; /// the renderer
		SDL_Window* window     = nullptr; /// and the window

		/// canonical sdl2 dance
		SDL_Init(SDL_INIT_VIDEO);
		SDL_CreateWindowAndRenderer(this->width_, this->height_, 0, &window, &renderer);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		SDL_RenderPresent(renderer);

		/// render and show
		sdl2_paint_canvas(renderer);
		SDL_RenderPresent(renderer);

		/// handle any events if required
		{
			SDL_Event event;
			bool quit = false;

			/// pressing escape quits...ooh, fancy !
			while (!quit) {
				while (SDL_PollEvent(&event)) {
					switch (event.type) {
					case SDL_KEYDOWN:
						switch (event.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit = true;
							break;

						default:
							break;
						}

					default:
						SDL_RenderPresent(renderer);
						break;
					}
				}

				SDL_Delay(1);
			}
		}

		/// cleanup
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();

		return;
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
