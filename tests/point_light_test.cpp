/// 3rdparty testing library
#include "common/include/logging.h"
#include "doctest/doctest.h"
#include "primitives/color.hpp"
#include "primitives/point_light.hpp"
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// convenience
namespace RT = raytracer;

TEST_CASE("test basic attributes of point_light type")
{
        auto const color = RT::color(1.0, 1.0, 1.0);
        auto const pos   = RT::create_point(0.0, 0.0, 0.0);
        auto const light = RT::point_light(pos, color);

        CHECK(light.get_color() == color);
        CHECK(light.position() == pos);
}
