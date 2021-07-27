#ifndef RAYTRACER_SRC_COLOR_HPP__
#define RAYTRACER_SRC_COLOR_HPP__

/*
 * this file implements the representation of color with which various image
 * pixels will be drawn.
 **/

/// c++ includes
#include <ostream>
#include <string>

// our includes
#include "tuple.hpp"
#include "utils.hpp"

namespace raytracer
{
	/*
	 * a color is composite of (r)ed, (g)reen, and (b)lue colors,
	 * represented via a tuple (point)
	 **/
	class color
	{
	    private:
		tuple rgb_;

	    public:
		constexpr explicit color()
		    : color(0.0, 0.0, 0.0)
		{
		}

		constexpr explicit color(float r, float g, float b)
		    : rgb_(create_point(r, g, b))
		{
		}

		/// ------------------------------------------------------------
		/// create a color by specifying rgb values. we cannot use
		/// an equivalent constructor due to ambiguous overload between
		/// `float` and `uint8_t`, which leads to explicit qualification
		/// at usage points. so ↓ is arguably better.
		static constexpr color RGB(uint8_t r, uint8_t g, uint8_t b)
		{
			return color(r / 255.0f, g / 255.0f, b / 255.0f);
		}

	    public:
		/// get the values out
		constexpr float R() const
		{
			return this->rgb_.x();
		}

		constexpr float G() const
		{
			return this->rgb_.y();
		}

		constexpr float B() const
		{
			return this->rgb_.z();
		}

		constexpr tuple RGB() const
		{
			return this->rgb_;
		}

	    public:
		/// some operators
		color& operator+=(color);
		color& operator-=(color);

		/// conversion from color -> string
		std::string stringify() const;
	};

	color operator+(color a, color b);
	color operator-(color a, color b);
	std::ostream& operator<<(std::ostream& os, color const& C);

	///
	/// epsilon equality comparison of two tuples
	///
	constexpr bool operator==(color lhs, color rhs)
	{
		// clang-format off
                return (epsilon_equal(lhs.R(), rhs.R())      &&
                        epsilon_equal(lhs.G(), rhs.G())      &&
                        epsilon_equal(lhs.B(), rhs.B()));
		// clang-format on
	}

	constexpr bool operator!=(color lhs, color rhs)
	{
		return !(lhs == rhs);
	}

	/// scalar multiplication
	constexpr color operator*(color a, double f)
	{
		return color(a.R() * f, a.G() * f, a.B() * f);
	}

	/// scalar addition
	constexpr color operator+(color a, double f)
	{
		return color(a.R() + f, a.G() + f, a.B() + f);
	}

	///
	/// hadamard/schur product of 'color * color', just multiply
	/// corresponding components from each color
	///
	constexpr color operator*(color a, color b)
	{
		return color(a.R() * b.R(),  // red
		             a.G() * b.G(),  // green
		             a.B() * b.B()); // blue
	}

	/// --------------------------------------------------------------------
	/// similar to the '*' operator above, we just divide instead of a
	/// multiply...
	constexpr color operator/(color a, double d)
	{
		return color(a.R() / d,  // red
		             a.G() / d,  // green
		             a.B() / d); // blue
	}

	/// --------------------------------------------------------------------
	/// return 'clamped' values of color. all color components must be
	/// between [0.0 ... 1.0]
	constexpr color clamp(color c)
	{
		/// the '0.0f/1.0f' is specifically there for type deduction
		return color(clamp_in_range(c.R(), 0.0f, 1.0f), clamp_in_range(c.G(), 0.0f, 1.0f),
		             clamp_in_range(c.B(), 0.0f, 1.0f));
	}

	/// some well known colors
	constexpr color color_black()
	{
		return color{0.0, 0.0, 0.0};
	}

	constexpr color color_white()
	{
		return color{1.0, 1.0, 1.0};
	}

	constexpr color color_red()
	{
		return color{1.0, 0.0, 0.0};
	}

	constexpr color color_green()
	{
		return color{0.0, 1.0, 0.0};
	}

	constexpr color color_blue()
	{
		return color{0.0, 0.0, 1.0};
	}

	constexpr color color_yellow()
	{
		return color{1.0, 0.8, 0.0};
	}

} // namespace raytracer

#endif // RAYTRACER_SRC_COLOR_HPP__
