/*
 * implement the raytracer canvas ppm writer
 **/

/// c++ includes
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>
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
	/// --------------------------------------------------------------------
	/// save a canvas to persistent store
	void canvas::write(std::string const& fname) const
	{
		switch (this->type_) {
		case PPM_CANVAS_BINARY:
			this->write_binary(fname);
			break;

		case PPM_CANVAS_ASCII:
			this->write_ascii(fname);
			break;

		default:
			break;
		}

		return;
	}

	/*
	 * only private functions from this point onwards
	 **/

	/// --------------------------------------------------------------------
	/// save canvas data in binary format i.e. P6
	void canvas::write_binary(std::string const& fname) const
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
		        "%ld %ld\n"                   /// x-y dimensions
		        "255\n"                       /// colors/pixel
		        ,                             /// --values--
		        this->width_, this->height_); /// width, height

		/// step-2 : add image data (row at a time)
		for (size_t i = 0; i < this->height_; i++) {
			auto raw_data     = get_ppm_row(i);
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
	void canvas::write_ascii(std::string const& fname) const
	{
		/// ------------------------------------------------------------
		/// generate a ppm-fmt representation of a row of pixels
		///
		/// though not 'strictly' required by P3 format specification,
		/// but we dump an integral number of pixels-per-row. this aids
		/// in easy scanning of generated image data
		/// ------------------------------------------------------------
		auto ppm_fmt_pixel_row = [](std::unique_ptr<unsigned char[]> row_data,
		                            size_t row_data_size) -> std::string {
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
					auto nl_location              = current_line.size() - rgb_str_len;
					current_line[nl_location - 1] = '\n';
					current_length                = rgb_str_len;
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
		        "%ld %ld\n"                   /// x-y dimensions
		        "255\n"                       /// colors/pixel
		        ,                             /// --values--
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
