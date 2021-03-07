#ifndef RAYTRACER_PERLIN_NOISE_HPP__
#define RAYTRACER_PERLIN_NOISE_HPP__

#include "tuple.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>

#include "constants.hpp"
#include "pattern_interface.hpp"

namespace raytracer
{
	/*
	 * this class implements the improved 3d perlin noise.
	 **/
	class perlin_noise
	{
	    private:
		std::array<uint8_t, 512> p_;

	    private:
		/// --------------------------------------------------------------------
		/// fade curve : 6*t^5 - 15*t^4 + 10t^3
		static constexpr double fade(double t)
		{
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		/// --------------------------------------------------------------------
		/// linear interpolation : interpolates 'x' between 'min' and 'max' values
		static constexpr double lerp(double x, double min, double max)
		{
			return min + x * (max - min);
		}

		/// --------------------------------------------------------------------
		/// faster floor
		static constexpr int32_t fast_floor(double val)
		{
			int32_t int_val = static_cast<int32_t>(val);
			return (val < int_val) ? (int_val - 1) : (int_val);
		}

		/// --------------------------------------------------------------------
		/// this computes the dot product between pseudorandom gradient vector
		/// and the vector from input coordinate to the 8 sorrounding points in
		/// its unit cube
		///
		/// faster gradient computation: this implementation is from
		///  - http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html
		static constexpr double grad(uint8_t hash, double x, double y, double z)
		{
			// clang-format off
                        switch (hash & 0xF) {
                                case 0x0: return  x + y;
                                case 0x1: return -x + y;
                                case 0x2: return  x - y;
                                case 0x3: return -x - y;
                                case 0x4: return  x + z;
                                case 0x5: return -x + z;
                                case 0x6: return  x - z;
                                case 0x7: return -x - z;
                                case 0x8: return  y + z;
                                case 0x9: return -y + z;
                                case 0xA: return  y - z;
                                case 0xB: return -y - z;
                                case 0xC: return  y + x;
                                case 0xD: return -y + z;
                                case 0xE: return  y - x;
                                case 0xF: return -y - z;
                                default:  break;            /// not reached
                        }
			// clang-format on

			return 0.0;
		}

	    public:
		/// --------------------------------------------------------------------
		/// initialize the permutation vector with reference values
		/// i.e. ken-perlin's implementation, or from a seed
		perlin_noise();
		perlin_noise(uint32_t seed);

	    public:
		/// ------------------------------------------------------------
		/// 1d noise interface
		double noise_1d(double x) const;
		double noise_1d_clamped(double x) const;
		double octave_noise_1d(double x, uint32_t octaves) const;
		double octave_noise_1d_clamped(double x, uint32_t octaves) const;
		double octave_noise_1d_clamped_01(double x, uint32_t octaves) const;

	    public:
		/// ------------------------------------------------------------
		/// 2d noise interface
		double noise_2d(double x, double y) const;
		double noise_2d_clamped(double x, double y) const;
		double octave_noise_2d(double x, double y, uint32_t octaves) const;
		double octave_noise_2d_clamped(double x, double y, uint32_t octaves) const;
		double octave_noise_2d_clamped_01(double x, double y, uint32_t octaves) const;

	    public:
		/// ------------------------------------------------------------
		/// 3d noise interface
		double noise_3d(double x, double y, double z) const;
		double noise_3d_clamped(double x, double y, double z) const;
		double octave_noise_3d(double x, double y, double z, uint32_t octaves) const;
		double octave_noise_3d_clamped(double x, double y, double z, uint32_t octaves) const;
		double octave_noise_3d_clamped_01(double x, double y, double z, uint32_t octaves) const;
	};

	/// ----------------------------------------------------------------------------
	/// initiliaze lattice-grid with canonical values
	inline perlin_noise::perlin_noise()
	{
		// clang-format off
                std::array<uint8_t, 256> const ref_p = {
                        151, 160, 137,  91,  90,  15, 131,  13,	201,  95,  96,  53, 194, 233,   7, 225,
                        140,  36, 103,  30,  69, 142,   8,  99,	 37, 240,  21,	10,  23, 190,   6, 148,
                        247, 120, 234,  75,   0,  26, 197,  62,  94, 252, 219, 203, 117,  35,  11,  32,
                        57 , 177,  33,	88, 237, 149,  56,  87, 174,  20, 125, 136, 171, 168,  68, 175,
                        74 , 165,  71, 134, 139,  48,  27, 166,  77, 146, 158, 231,  83, 111, 229, 122,
                        60 , 211, 133, 230, 220, 105,  92,  41,  55,  46, 245,  40, 244, 102, 143,  54,
                        65 ,  25,  63, 161,   1, 216,  80,  73, 209,  76, 132, 187, 208,  89,  18, 169,
                        200, 196, 135, 130, 116, 188, 159,  86, 164, 100, 109, 198, 173, 186,   3,  64,
                        52 , 217, 226, 250, 124, 123,   5, 202,  38, 147, 118, 126, 255,  82,  85, 212,
                        207, 206,  59, 227,  47,  16,  58,  17, 182, 189,  28,  42, 223, 183, 170, 213,
                        119, 248, 152,   2,  44, 154, 163,  70,	221, 153, 101, 155, 167,  43, 172,   9,
                        129,  22,  39, 253,  19,  98, 108, 110,  79, 113, 224, 232, 178, 185, 112, 104,
                        218, 246,  97, 228, 251,  34, 242, 193, 238, 210, 144,  12, 191, 179, 162, 241,
                        81 ,  51, 145, 235, 249,  14, 239, 107,  49, 192, 214,  31, 181, 199, 106, 157,
                        184,  84, 204, 176, 115, 121,  50,  45, 127,   4, 150, 254, 138, 236, 205,  93,
                        222, 114,  67,  29,  24,  72, 243, 141, 128, 195,  78,  66, 215,  61, 156, 180
                };
		// clang-format on

		/// --------------------------------------------------------------------
		std::copy(ref_p.begin(), ref_p.end(), p_.begin());
		std::copy(ref_p.begin(), ref_p.end(), p_.begin() + ref_p.size());
	}

	/// ----------------------------------------------------------------------------
	/// initiliaze lattice-grid with a user-specified seed
	inline perlin_noise::perlin_noise(uint32_t seed)
	{
		/// fill up an array from [0 .. 255] sequentially, and then
		/// shuffle it.
		std::array<uint8_t, 256> ref_p;
		std::iota(ref_p.begin(), ref_p.end(), 0);

		std::default_random_engine engine(seed);
		std::shuffle(ref_p.begin(), ref_p.end(), engine);

		/// ------------------------------------------------------------
		/// copy it
		std::copy(ref_p.begin(), ref_p.end(), p_.begin());
		std::copy(ref_p.begin(), ref_p.end(), p_.begin() + ref_p.size());
	}

	/// ------------------------------------------------------------
	/// 1d noise implementation
	inline double perlin_noise::noise_1d(double x) const
	{
		return noise_3d(x, 0.0, 0.0);
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::noise_1d_clamped(double x) const
	{
		return noise_3d_clamped(x, 0.0, 0.0);
	}

	/// unclamped
	inline double perlin_noise::octave_noise_1d(double x, uint32_t octaves) const
	{
		return octave_noise_3d(x, 0.0, 0.0, octaves);
	}

	/// clamped to [-1.0 .. 1.0]
	inline double perlin_noise::octave_noise_1d_clamped(double x, uint32_t octaves) const
	{
		return octave_noise_3d_clamped(x, 0.0, 0.0, octaves);
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::octave_noise_1d_clamped_01(double x, uint32_t octaves) const
	{
		return octave_noise_3d_clamped_01(x, 0.0, 0.0, octaves);
	}

	/// --------------------------------------------------------------------
	/// 2d noise implementation
	inline double perlin_noise::noise_2d(double x, double y) const
	{
		return noise_3d(x, y, 0.0);
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::noise_2d_clamped(double x, double y) const
	{
		return noise_3d_clamped(x, y, 0.0);
	}

	/// unclamped
	inline double perlin_noise::octave_noise_2d(double x, double y, uint32_t octaves) const
	{
		return octave_noise_3d(x, y, 0.0, octaves);
	}

	/// clamped to [-1.0 .. 1.0]
	inline double perlin_noise::octave_noise_2d_clamped(double x, double y, uint32_t octaves) const
	{
		return octave_noise_3d_clamped(x, y, 0.0, octaves);
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::octave_noise_2d_clamped_01(double x, double y, uint32_t octaves) const
	{
		return octave_noise_3d_clamped_01(x, y, 0.0, octaves);
	}

	/// --------------------------------------------------------------------
	/// noise3d implementation : this is the _real_ deal, other *noise*
	/// functions just invoke corresponding *noise_3d* with appropriate y/z
	/// values defaulted to 0.0

	/// ----------------------------------------------------------------------------
	/// noise3d
	inline double perlin_noise::noise_3d(double x, double y, double z) const
	{
		int32_t const x_lo = fast_floor(x);
		int32_t const y_lo = fast_floor(y);
		int32_t const z_lo = fast_floor(z);

		const std::int32_t X = static_cast<std::int32_t>(x_lo) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(y_lo) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(z_lo) & 255;

		x -= x_lo;
		y -= y_lo;
		z -= z_lo;

		const double u = fade(x);
		const double v = fade(y);
		const double w = fade(z);

		const std::int32_t A = p_[X] + Y, AA = p_[A] + Z, AB = p_[A + 1] + Z;
		const std::int32_t B = p_[X + 1] + Y, BA = p_[B] + Z, BB = p_[B + 1] + Z;

		return lerp(w,
		            lerp(v, lerp(u, grad(p_[AA], x, y, z), grad(p_[BA], x - 1, y, z)),
		                 lerp(u, grad(p_[AB], x, y - 1, z), grad(p_[BB], x - 1, y - 1, z))),
		            lerp(v, lerp(u, grad(p_[AA + 1], x, y, z - 1), grad(p_[BA + 1], x - 1, y, z - 1)),
		                 lerp(u, grad(p_[AB + 1], x, y - 1, z - 1),
		                      grad(p_[BB + 1], x - 1, y - 1, z - 1))));
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::noise_3d_clamped(double x, double y, double z) const
	{
		return noise_3d(x, y, z) * 0.5 + 0.5;
	}

	/// unclamped
	inline double perlin_noise::octave_noise_3d(double x, double y, double z, uint32_t octaves) const
	{
		double retval    = 0.0;
		double amplitude = 1.0;

		for (uint32_t i = 0; i < octaves; i++) {
			retval    = retval + noise_3d(x, y, z) * amplitude;
			x         = x * 2;
			y         = y * 2;
			z         = z * 2;
			amplitude = amplitude / 2.0;
		}

		return retval;
	}

	/// clamped || normalized to [-1.0 .. 1.0]
	inline double perlin_noise::octave_noise_3d_clamped(double x, double y, double z,
	                                                    uint32_t octaves) const
	{
		double amplitude = 1.0;
		double weight    = 0.0;

		for (uint32_t i = 0; i < octaves; i++) {
			weight += amplitude;
			amplitude /= 2.0;
		}

		return octave_noise_3d(x, y, z, octaves) / weight;
	}

	/// clamped to [0.0 .. 1.0]
	inline double perlin_noise::octave_noise_3d_clamped_01(double x, double y, double z,
	                                                       uint32_t octaves) const
	{
		return std::clamp<double>(octave_noise_3d(x, y, z, octaves) * 0.5 + 0.5, 0.0, 1.0);
	}

} // namespace raytracer

#endif /// RAYTRACER_PERLIN_NOISE_HPP__
