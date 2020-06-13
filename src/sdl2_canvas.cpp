/*
 * implement the raytracer sdl2-canvas
**/

/// sdl2 includes
#include <SDL2/SDL.h>

/// c++ includes
#include <cstddef>

/// our includes
#include "sdl2_canvas.hpp"
#include "color.hpp"
#include "constants.hpp"

namespace raytracer
{
        sdl2_canvas::sdl2_canvas(size_t width, size_t height)
            : width_(width)
            , height_(height)
        {
                /// here we go again...do the sdl dance
                SDL_Init(SDL_INIT_VIDEO);
                SDL_CreateWindowAndRenderer(width, height, 0, &window_, &renderer_);
                SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 0);
                SDL_RenderClear(renderer_);
                SDL_RenderPresent(renderer_);
        }

        sdl2_canvas::~sdl2_canvas()
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

                                case SDL_WINDOWEVENT_EXPOSED:
                                        SDL_RenderPresent(this->renderer_);
                                        break;

                                default:
                                        break;
                                }
                        }

                        SDL_Delay(1);
                }

                /// cleanup
                SDL_DestroyRenderer(this->renderer_);
                SDL_DestroyWindow(this->window_);
                SDL_Quit();
        }

        ///
        /// this function is called to draw a pixel at (x, y) on the canvas with
        /// a color 'C'
        ///
        void sdl2_canvas::write_pixel(size_t x, size_t y, color C)
        {
                /// scale a floating color-value.
                auto scale_color_val = [](float value) -> uint8_t {
                        size_t color_val = value * PPM_MAX_COLOR_VALUE;

                        /// clamp high values to max
                        if (color_val >= PPM_MAX_COLOR_VALUE) {
                                color_val = PPM_MAX_COLOR_VALUE;
                        }

                        return color_val;
                };

                // clang-format off
                SDL_SetRenderDrawColor(this->renderer_,
                                       scale_color_val(C.R()),
                                       scale_color_val(C.G()),
                                       scale_color_val(C.B()), 255);
                // clang-format on
                SDL_RenderDrawPoint(this->renderer_, x, y);

                return;
        }

        ///
        /// show the current state of canvas on demand
        ///
        void sdl2_canvas::show_canvas() const
        {
                SDL_RenderPresent(this->renderer_);

                return;
        }
} // namespace raytracer
