/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "patterns/align_check_pattern.hpp"
#include "primitives/color.hpp"
#include "primitives/uv_point.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// a convenience alias
namespace RT = raytracer;

/// ----------------------------------------------------------------------------
/// test align-check pattern
TEST_CASE("scenario : layout of align-check pattern")
{
        /// --------------------------------------------------------------------
        /// align-check-pattern colors
        RT::color const main_color = RT::color(1.0, 1.0, 1.0);
        RT::color const ul_color   = RT::color(1.0, 0.0, 0.0);
        RT::color const ur_color   = RT::color(1.0, 1.0, 0.0);
        RT::color const bl_color   = RT::color(0.0, 1.0, 0.0);
        RT::color const br_color   = RT::color(0.0, 1.0, 1.0);

        const auto uv_pattern = RT::align_check(main_color, ul_color, ur_color, bl_color, br_color);

        struct {
                RT::uv_point uv_pt;
                RT::color exp_color;
        } const all_tc[] = {
                // clang-format off
		{RT::uv_point(0.5, 0.5), main_color },
                {RT::uv_point(0.1, 0.9), ul_color   },
		{RT::uv_point(0.9, 0.9), ur_color   },
                {RT::uv_point(0.1, 0.1), bl_color   },
		{RT::uv_point(0.9, 0.1), br_color   },
                // clang-format on
        };

        for (auto const& tc : all_tc) {
                auto const got_color = uv_pattern.uv_pattern_color_at(tc.uv_pt);
                CHECK(got_color == tc.exp_color);
        }
}
