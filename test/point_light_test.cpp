#include "point_light.hpp"

/// 3rdparty testing library
#include "doctest.h"

/// convenience
namespace RT = raytracer;

TEST_CASE("test basic attributes of point_light type")
{
        auto const color = RT::color(1.0, 1.0, 1.0);
        auto const pos	 = RT::create_point(0.0, 0.0, 0.0);
        auto const light = RT::point_light(pos, color);

        CHECK(light.get_color() == color);
        CHECK(light.position() == pos);
}