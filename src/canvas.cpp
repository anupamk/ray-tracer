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
                   << "type: "   << this->canvas_type_str_[type_index]
                   << "}";
                // clang-format on

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// save a canvas to persistent store
        void canvas::save(std::string const& fname) const
        {
                switch (this->type_) {
                case PPM_CANVAS_BINARY:
                        this->save_binary(fname);
                        break;

                case PPM_CANVAS_ASCII:
                        this->save_ascii(fname);
                        break;

                default:
                        break;
                }

                return;
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
            , buf_(std::make_unique<color[]>(width * height))
        {
        }

        /// --------------------------------------------------------------------
        /// convert canvas 'color' to equivalent ppm values
        void canvas::ppm_color_at(size_t x,		  /// x-location
                                  size_t y,		  /// y-location
                                  unsigned char& r,	  /// r (0..255)
                                  unsigned char& g,	  /// g (0..255)
                                  unsigned char& b) const /// b (0..255)
        {
                auto pixel_color = this->read_pixel(x, y);

                r = roundf(pixel_color.R() * 255.0);
                g = roundf(pixel_color.G() * 255.0);
                b = roundf(pixel_color.B() * 255.0);

                return;
        }

        /// --------------------------------------------------------------------
        /// get ppm encoded row of values
        ppm_row_data_t canvas::get_ppm_row(size_t y) const
        {
                auto ppm_colors	    = std::make_unique<unsigned char[]>(ppm_num_values());
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

        /// --------------------------------------------------------------------
        /// save canvas data in binary format i.e. P6
        void canvas::save_binary(std::string const& fname) const
        {
                FILE* dst_file = fopen(fname.c_str(), "wb"); /// 'b' ==> binary

                if (dst_file == nullptr) {
                        fprintf(stderr, "failed writing: '%s'. reason: '%s'\n", fname.c_str(),
                                strerror(errno));
                        return;
                }

                /// step-1: add image header
                fprintf(dst_file,
                        "P6\n"
                        "%ld %ld\n"		      /// x-y dimensions
                        "255\n"			      /// colors/pixel
                        ,			      /// --values--
                        this->width_, this->height_); /// width, height

                /// step-2 : add image data (row at a time)
                for (size_t i = 0; i < this->height_; i++) {
                        auto raw_data	  = std::move(get_ppm_row(i));
                        auto raw_data_buf = raw_data.get();

                        fwrite(raw_data_buf, sizeof(unsigned char), ppm_num_values(), dst_file);
                }

                /// ok, we are done here
                fflush(dst_file);
                fclose(dst_file);

                return;
        }

        /// --------------------------------------------------------------------
        /// save canvas data in ascii format i.e. P3
        void canvas::save_ascii(std::string const& fname) const
        {
                /// ------------------------------------------------------------
                /// generate a ppm-fmt representation of a row of pixels
                ///
                /// though not 'strictly' required by P3 format specification,
                /// but we dump an integral number of pixels-per-row. this aids
                /// in easy scanning of generated image data
                /// ------------------------------------------------------------
                auto ppm_fmt_pixel_row = [](ppm_row_data_t row_data, size_t row_data_size) -> std::string {
                        std::string current_line;

                        /// 1 color comprising 3 pixel data values are consumed per iteration
                        for (size_t pixel = 0, current_length = 0; pixel < row_data_size; pixel += 3) {
                                /// --------------------------------------------
                                /// both of 'r' and 'g' components will _always_
                                /// be followed by a ' '.
                                ///
                                /// the 'b' component on the other hand, can
                                /// followed by either a ' ' or '\n'.
                                ///
                                /// however the '\n' will only be required when
                                /// we have accumulated PPM_MAX_LINE_LENGTH bytes
                                /// worth of data in the current line.
                                ///
                                /// so, we just add ' ' by default, and 'fix'
                                /// things later when we hit limit...
                                /// --------------------------------------------
                                auto r = std::to_string(row_data[pixel + 0]) + " ";
                                auto g = std::to_string(row_data[pixel + 1]) + " ";
                                auto b = std::to_string(row_data[pixel + 2]) + " ";

                                auto const rgb_str     = r + g + b;
                                auto const rgb_str_len = rgb_str.size();

                                /// --------------------------------------------
                                current_line += rgb_str;
                                current_length += rgb_str_len;

                                /// --------------------------------------------
                                /// if the *current* length exceeds
                                /// PPM_MAX_LINE_LENGTH, it implies that the last
                                /// rgb_str that was added pushed us over the
                                /// edge.
                                ///
                                /// just go back, and repalce the extra " " with
                                /// a '\n' to indicate starting of a new line of
                                /// pixel data.
                                /// --------------------------------------------
                                if (current_length > PPM_MAX_LINE_LENGTH) {
                                        auto nl_location	      = current_line.size() - rgb_str_len;
                                        current_line[nl_location - 1] = '\n';
                                        current_length		      = rgb_str_len;
                                }
                        }

                        /// trim possibly trailing ' ' from the line
                        rtrim(current_line);

                        return current_line;
                };

                FILE* dst_file = fopen(fname.c_str(), "w");

                if (dst_file == nullptr) {
                        fprintf(stderr, "failed writing: '%s'. reason: '%s'\n", fname.c_str(),
                                strerror(errno));
                        return;
                }

                /// step-1: add image-header
                fprintf(dst_file,
                        "P3\n"
                        "%ld %ld\n"		      /// x-y dimensions
                        "255\n"			      /// colors/pixel
                        ,			      /// --values--
                        this->width_, this->height_); /// width, height

                /// ------------------------------------------------------------
                /// step-2: add image data, 1 row at a time
                for (size_t i = 0; i < this->height_; i++) {
                        auto raw_data = get_ppm_row(i);
                        auto fmt_data = ppm_fmt_pixel_row(std::move(raw_data), ppm_num_values());

                        fprintf(dst_file, "%s\n", fmt_data.c_str());
                }

                /// ok we are done here
                fflush(dst_file);
                fclose(dst_file);

                return;
        }

} // namespace raytracer
