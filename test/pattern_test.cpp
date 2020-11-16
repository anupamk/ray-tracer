/// c++ includes
#include "plane.hpp"
#include "point_light.hpp"
#include <memory>

/// 3rdparty testing library
#include "doctest.h"

/// ----------------------------------------------------------------------------
/// our includes, first what we are testing
#include "pattern_interface.hpp"
#include "solid_pattern.hpp"
#include "striped_pattern.hpp"
#include "gradient_pattern.hpp"
#include "ring_pattern.hpp"
#include "checkers_pattern.hpp"

/// ... and then everything else
#include "matrix_transformations.hpp"
#include "color.hpp"
#include "sphere.hpp"
#include "material.hpp"
#include "tuple.hpp"
#include "phong_illumination.hpp"

/// convenience
namespace RT   = raytracer;
using RT_XFORM = RT::matrix_transformations_t;

/// ----------------------------------------------------------------------------
/// define a trivial test pattern
class test_pattern final : public RT::pattern_interface
{
    public:
	RT::color color_at_point(RT::tuple const& pt) const override
	{
		return RT::color(pt.x(), pt.y(), pt.z());
	}
};

/// ----------------------------------------------------------------------------
/// pattern tranformation matrix
TEST_CASE("pattern::pattern(...) test")
{
	/// --------------------------------------------------------------------
	/// default pattern have identity matrix as it's transform
	auto test_pattern  = std::make_shared<RT::solid_pattern>(RT::color_blue());
	auto exp_ident_mat = RT::fsize_dense2d_matrix_t::create_identity_matrix(4);

	CHECK(test_pattern->transform() == exp_ident_mat);

	/// --------------------------------------------------------------------
	/// apply transformation matrix to a pattern
	auto xlate_mat = RT_XFORM::create_3d_translation_matrix(1.0, 2.0, 3.0);
	test_pattern->transform(xlate_mat);

	CHECK(test_pattern->transform() == xlate_mat);
}

/// ----------------------------------------------------------------------------
/// color of a pattern at a specific point on a shape
TEST_CASE("pattern::color_at_point(...) test")
{
	auto s_01 = std::make_shared<RT::sphere>();
	s_01->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

	auto pattern_01 = std::make_shared<test_pattern>();
	s_01->set_material(RT::material().set_pattern(pattern_01));

	auto exp_color = RT::color(1.0, 1.5, 2.0);
	auto got_color = pattern_01->color_at_shape(s_01, RT::create_point(2.0, 3.0, 4.0));

	CHECK(exp_color == got_color);
}

/// ----------------------------------------------------------------------------
/// color of a transformed pattern at a specific point on a shape
TEST_CASE("pattern::color_at_point(...) test")
{
	auto s_01 = std::make_shared<RT::sphere>();

	auto pattern_01 = std::make_shared<test_pattern>();
	pattern_01->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

	s_01->set_material(RT::material().set_pattern(pattern_01));

	auto exp_color = RT::color(1.0, 1.5, 2.0);
	auto got_color = pattern_01->color_at_shape(s_01, RT::create_point(2.0, 3.0, 4.0));

	CHECK(exp_color == got_color);
}

/// ----------------------------------------------------------------------------
/// color of a transformed pattern and a transformed object at a specific point
/// on a shape
TEST_CASE("pattern::color_at_point(...) test")
{
	auto s_01 = std::make_shared<RT::sphere>();
	s_01->transform(RT_XFORM::create_3d_scaling_matrix(2.0, 2.0, 2.0));

	auto pattern_01 = std::make_shared<test_pattern>();
	pattern_01->transform(RT_XFORM::create_3d_translation_matrix(0.5, 1.0, 1.5));

	s_01->set_material(RT::material().set_pattern(pattern_01));

	auto exp_color = RT::color(0.75, 0.5, 0.25);
	auto got_color = pattern_01->color_at_shape(s_01, RT::create_point(2.5, 3.0, 3.5));

	CHECK(exp_color == got_color);
}

/// ----------------------------------------------------------------------------
/// striped pattern tests
TEST_CASE("striped_pattern::color_at_point(...) test")
{
	auto sp = std::make_shared<RT::striped_pattern>(RT::color_white(), RT::color_black());

	/// --------------------------------------------------------------------
	/// striped pattern is constant along the y-axis
	auto got_ycolor_01 = sp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_ycolor_01 == RT::color_white());

	auto got_ycolor_02 = sp->color_at_point(RT::create_point(0.0, 1.0, 0.0));
	CHECK(got_ycolor_02 == RT::color_white());

	auto got_ycolor_03 = sp->color_at_point(RT::create_point(0.0, 2.0, 0.0));
	CHECK(got_ycolor_03 == RT::color_white());

	/// --------------------------------------------------------------------
	/// striped pattern is constant along the z-axis
	auto got_zcolor_01 = sp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_zcolor_01 == RT::color_white());

	auto got_zcolor_02 = sp->color_at_point(RT::create_point(0.0, 0.0, 1.0));
	CHECK(got_zcolor_02 == RT::color_white());

	auto got_zcolor_03 = sp->color_at_point(RT::create_point(0.0, 0.0, 2.0));
	CHECK(got_zcolor_03 == RT::color_white());

	/// --------------------------------------------------------------------
	/// striped pattern alternates along the x-axis
	auto got_xcolor_01 = sp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_xcolor_01 == RT::color_white());

	auto got_xcolor_02 = sp->color_at_point(RT::create_point(0.9, 0.0, 0.0));
	CHECK(got_xcolor_02 == RT::color_white());

	auto got_xcolor_03 = sp->color_at_point(RT::create_point(1.0, 0.0, 0.0));
	CHECK(got_xcolor_03 == RT::color_black());

	auto got_xcolor_04 = sp->color_at_point(RT::create_point(-0.1, 0.0, 0.0));
	CHECK(got_xcolor_04 == RT::color_black());

	auto got_xcolor_05 = sp->color_at_point(RT::create_point(-1.0, 0.0, 0.0));
	CHECK(got_xcolor_05 == RT::color_black());

	auto got_xcolor_06 = sp->color_at_point(RT::create_point(-1.1, 0.0, 0.0));
	CHECK(got_xcolor_06 == RT::color_white());
}

/// ----------------------------------------------------------------------------
/// gradient pattern tests
TEST_CASE("gradient_pattern::color_at_point(...) test")
{
	auto gp = std::make_shared<RT::gradient_pattern>(RT::color_white(), RT::color_black());

	auto got_color_01 = gp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_color_01 == RT::color_white());

	auto got_color_02 = gp->color_at_point(RT::create_point(0.25, 0.0, 0.0));
	CHECK(got_color_02 == RT::color(0.75, 0.75, 0.75));

	auto got_color_03 = gp->color_at_point(RT::create_point(0.50, 0.0, 0.0));
	CHECK(got_color_03 == RT::color(0.5, 0.5, 0.5));

	auto got_color_04 = gp->color_at_point(RT::create_point(0.75, 0.0, 0.0));
	CHECK(got_color_04 == RT::color(0.25, 0.25, 0.25));
}

/// ----------------------------------------------------------------------------
/// ring pattern tests
TEST_CASE("ring::color_at_point(...) test")
{
	auto rp = std::make_shared<RT::ring_pattern>(RT::color_white(), RT::color_black());

	auto got_color_01 = rp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_color_01 == RT::color_white());

	auto got_color_02 = rp->color_at_point(RT::create_point(1.0, 0.0, 0.0));
	CHECK(got_color_02 == RT::color_black());

	auto got_color_03 = rp->color_at_point(RT::create_point(0.0, 0.0, 1.0));
	CHECK(got_color_03 == RT::color_black());

	auto got_color_04 = rp->color_at_point(RT::create_point(0.708, 0.0, 0.708));
	CHECK(got_color_03 == RT::color_black());
}

/// ----------------------------------------------------------------------------
/// checkers pattern tests
TEST_CASE("checkers_pattern::color_at_point(...) test")
{
	auto cp = std::make_shared<RT::checkers_pattern>(RT::color_white(), RT::color_black());

	/// checkers should repeat along x-axis
	auto got_xcolor_01 = cp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_xcolor_01 == RT::color_white());

	auto got_xcolor_02 = cp->color_at_point(RT::create_point(0.99, 0.0, 0.0));
	CHECK(got_xcolor_02 == RT::color_white());

	auto got_xcolor_03 = cp->color_at_point(RT::create_point(1.01, 0.0, 0.0));
	CHECK(got_xcolor_03 == RT::color_black());

	/// checkers should repeat along y-axis
	auto got_ycolor_01 = cp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_ycolor_01 == RT::color_white());

	auto got_ycolor_02 = cp->color_at_point(RT::create_point(0.0, 0.99, 0.0));
	CHECK(got_ycolor_02 == RT::color_white());

	auto got_ycolor_03 = cp->color_at_point(RT::create_point(0.0, 1.01, 0.0));
	CHECK(got_ycolor_03 == RT::color_black());

	/// checkers should repeat along z-axis
	auto got_zcolor_01 = cp->color_at_point(RT::create_point(0.0, 0.0, 0.0));
	CHECK(got_zcolor_01 == RT::color_white());

	auto got_zcolor_02 = cp->color_at_point(RT::create_point(0.0, 0.0, 0.99));
	CHECK(got_zcolor_02 == RT::color_white());

	auto got_zcolor_03 = cp->color_at_point(RT::create_point(0.0, 0.0, 1.01));
	CHECK(got_zcolor_03 == RT::color_black());
}
