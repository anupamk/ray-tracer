#ifndef RAYTRACER_SRC_SDL2_CANVAS_HPP__
#define RAYTRACER_SRC_SDL2_CANVAS_HPP__

/*
 * this file implements the canvas using sdl2
**/

/// sdl2 includes
#include <SDL2/SDL.h>

/// c++ includes
#include <cstddef>

/// our includes
#include "color.hpp"

namespace raytracer
{
	class sdl2_canvas
	{
	    private:
		size_t width_{0};
		size_t height_{0};
                
		SDL_Renderer* renderer_ = nullptr; /// the renderer
		SDL_Window* window_	= nullptr; /// and the window

	    public:
		sdl2_canvas(size_t width, size_t height);
		~sdl2_canvas();

	    public:
		void write_pixel(size_t x, size_t y, color C);
                void show_canvas() const;
	};
} // namespace raytracer

#endif /// RAYTRACER_SRC_SDL2_CANVAS_HPP__
