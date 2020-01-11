#ifndef RAYTRACER_SRC_CANVAS_HPP__
#define RAYTRACER_SRC_CANVAS_HPP__

/*
 * this file implements the canvas i.e. a virtual drawing board on which to
 * visualize our creations !
 *
 * canvas generates images in the ppm (portable pixmap) format. specifically
 * both ppm3 and ppm6 i.e. ascii and binary encoded images (respectively) are
 * supported.
 *
 * canvas can also render its current state using sdl2 as well.
**/

/// c++ includes
#include <cstddef>
#include <memory>
#include <string>

/// our includes
#include "color.hpp"
#include "logging.h"


namespace raytracer
{
	/// ----------------------------------------------------------------------------
	/// netpbm based color canvas
	class canvas
	{
	    private:
		/// what types of canvas we support
		enum canvas_type {
			PPM_CANVAS_INVALID = 0,
			PPM_CANVAS_ASCII   = 1,
			PPM_CANVAS_BINARY  = 2,
		};

		const char* const canvas_type_str_[PPM_CANVAS_BINARY + 1] = {
			[PPM_CANVAS_INVALID] = "PPM_CANVAS_INVALID",
			[PPM_CANVAS_ASCII]   = "PPM_CANVAS_ASCII",
			[PPM_CANVAS_BINARY]  = "PPM_CANVAS_BINARY",
		};

	    private:
		std::size_t const width_{0};
		std::size_t const height_{0};
		canvas_type const type_{PPM_CANVAS_INVALID};
		std::unique_ptr<color[]> buf_{nullptr};

	    public:
		/// named constructors
		static canvas create_ascii(std::size_t width, std::size_t height);
		static canvas create_binary(std::size_t width, std::size_t height);

	    public:
		/// ------------------------------------------------------------
		/// ACCESSORS
                
		constexpr std::size_t width() const
		{
			return this->width_;
		}

		constexpr std::size_t height() const
		{
			return this->height_;
		}

		color read_pixel(size_t x, size_t y) const
		{
			return this->buf_[x + y * this->width_];
		}

		/// meta information about the canvas
		std::string stringify() const;

		/// save canvas to a persistent store
		void save(std::string const& fname) const;

		/// display contents of canvas using sdl2
		void show() const;

	    public:
		/// ------------------------------------------------------------
		/// MUTATORS

		void write_pixel(size_t x, size_t y, color c)
		{
			this->buf_[x + y * this->width_] = c;
		}

	    private:
		/// ------------------------------------------------------------
		/// ACCESSORS

		constexpr std::size_t ppm_num_values() const
		{
			/// 'this->width_ * 3' ∵ 3 bytes per pixel
			return width() * 3;
		}

	    private:
		/// ∵ named-constructor idiom
		canvas(std::size_t width, std::size_t height, canvas_type type);

		/// save the to a persistent store
		void save_binary(std::string const&) const;
		void save_ascii(std::string const&) const;

		void ppm_color_at(size_t x, size_t y, unsigned char& r, unsigned char& g,
				  unsigned char& b) const;

		std::unique_ptr<unsigned char[]> get_ppm_row(size_t y) const;
	};

} // namespace raytracer

#endif // RAYTRACER_SRC_CANVAS_HPP__
