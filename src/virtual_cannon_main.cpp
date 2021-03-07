/// c++ includes
#include <cstddef>

/// our includes
#include "canvas.hpp"
#include "logging.h"
#include "virtual_cannon.hpp"

/*
 * select default logging level depending on type of build. this can be changed
 * later to more appropriate values.
 **/
log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_INFO;

int main(int argc, char** argv)
{
	/// --------------------------------------------------------------------
	/// convenience alias
	namespace RT = raytracer;

	/// create projectile
	const auto start_point = RT::create_point(0.0, 1.0, 0.0);
	auto velocity          = RT::normalize(RT::create_vector(1.0, 1.8, 0.0)) * 11.25;
	projectile proj(start_point, velocity);

	/// create the environment
	const auto gravity = RT::create_vector(0.0, -0.1, 0.0);
	const auto wind    = raytracer::create_vector(-0.01, 0.0, 0.0);
	env_params env(gravity, wind);

	/// and the canvas where everything will be shown
	auto canvas = RT::canvas::create_binary(900, 550);

	int num_ticks = 0;
	for (; proj.position().y() > 0; ++num_ticks) {
		const auto proj_pos    = proj.position();
		const size_t proj_xpos = proj_pos.x();
		const size_t proj_ypos = proj_pos.y();

		/// ------------------------------------------------------------
		/// the (0, 0) of the canvas is at the top-left. however the
		/// viewing coordinates have (0, 0) at the bottom-left.
		///
		/// so, we subtract y-position of the projectil from
		/// canvas.height()
		canvas.write_pixel(proj_xpos, canvas.height() - proj_ypos, RT::color_red());

		/// next round
		proj = tick(env, proj);
	}

	/// now write the canvas data out.
	auto const img_fname = "virtual-cannon.ppm";
	canvas.write(img_fname);

	LOG_INFO("image-file: '%s' generated", img_fname);

	return 0;
}
