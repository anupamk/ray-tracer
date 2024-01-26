/// c++ includes
#include <stdexcept>

/// 3rd-party includes
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"

/// our includes
#include "common/include/logging.h"
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"
#include "utils/utils.hpp"

log_level_t GLOBAL_LOG_LEVEL_NOW = LOG_LEVEL_FATAL;

/// point-tuple test
TEST_CASE("testing tuple's point functionality")
{
        raytracer::tuple t1(0.0, 0.0, 0.0, raytracer::tuple_type_t::POINT);

        CHECK(t1.x() == 0.0);
        CHECK(t1.y() == 0.0);
        CHECK(t1.z() == 0.0);
        CHECK(t1.is_point() == true);
        CHECK(t1.is_vector() == false);
}

/// vector-tuple test
TEST_CASE("testing tuple's vector functionality")
{
        raytracer::tuple t1(0.0, 0.0, 0.0, raytracer::tuple_type_t::VECTOR);

        CHECK(t1.x() == 0.0);
        CHECK(t1.y() == 0.0);
        CHECK(t1.z() == 0.0);
        CHECK(t1.is_point() == false);
        CHECK(t1.is_vector() == true);
}

/// tuple::create_point(...) test
TEST_CASE("test tuple::create_point(...) interface")
{
        raytracer::tuple pt = raytracer::create_point(1.0, 2.0, 3.0);

        CHECK(pt.x() == 1.0);
        CHECK(pt.y() == 2.0);
        CHECK(pt.z() == 3.0);
        CHECK(pt.is_point() == true);
        CHECK(pt.is_vector() == false);
}

/// tuple::create_vector(...) test
TEST_CASE("test tuple::create_vector(...) interface")
{
        raytracer::tuple vec = raytracer::create_vector(1.0, 2.0, 3.0);

        CHECK(vec.x() == 1.0);
        CHECK(vec.y() == 2.0);
        CHECK(vec.z() == 3.0);
        CHECK(vec.is_point() == false);
        CHECK(vec.is_vector() == true);
}

TEST_CASE("test tuple::operator==(...) interface for points")
{
        /// trivial comparison
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple pt_2 = raytracer::create_point(1.0, 2.0, 3.0);

                auto const pts_eq = (pt_1 == pt_2) ? true : false;
                CHECK(pts_eq == true);
        }

        /// good approx comparison
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0 + raytracer::EPSILON * 0.1, 2.0, 3.0);
                raytracer::tuple pt_2 = raytracer::create_point(1.0, 2.0, 3.0);

                auto const pts_eq = (pt_1 == pt_2) ? true : false;
                CHECK(pts_eq == true);
        }

        /// bad approx comparison
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0 + raytracer::EPSILON * 10, 2.0, 3.0);
                raytracer::tuple pt_2 = raytracer::create_point(1.0, 2.0, 3.0);

                auto const pts_eq = (pt_1 == pt_2) ? true : false;
                CHECK(pts_eq == false);
        }
}

TEST_CASE("test tuple::operator==(...) interface for vectors")
{
        /// trivial comparison
        {
                raytracer::tuple v_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple v_2 = raytracer::create_vector(1.0, 2.0, 3.0);

                auto const vec_eq = (v_1 == v_2) ? true : false;
                CHECK(vec_eq == true);
        }

        /// good approx comparison
        {
                raytracer::tuple v_1 = raytracer::create_vector(1.0 + raytracer::EPSILON * 0.1, 2.0, 3.0);
                raytracer::tuple v_2 = raytracer::create_vector(1.0, 2.0, 3.0);

                auto const vec_eq = (v_1 == v_2) ? true : false;
                CHECK(vec_eq == true);
        }

        /// bad approx comparison
        {
                raytracer::tuple v_1 = raytracer::create_vector(1.0 + raytracer::EPSILON * 10, 2.0, 3.0);
                raytracer::tuple v_2 = raytracer::create_vector(1.0, 2.0, 3.0);

                auto const vec_eq = (v_1 == v_2) ? true : false;
                CHECK(vec_eq == false);
        }
}

/// test operator+=(...)
TEST_CASE("tuple::operator+=(...) test")
{
        /// point + vector
        {
                raytracer::tuple pt_1  = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);

                pt_1 += vec_1;

                CHECK(pt_1.x() == 3.0);
                CHECK(pt_1.y() == 5.0);
                CHECK(pt_1.z() == 7.0);
                CHECK(pt_1.is_point() == true);
                CHECK(pt_1.is_vector() == false);
        }

        /// vector + vector
        {
                raytracer::tuple vec_0 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);

                vec_0 += vec_1;

                CHECK(vec_0.x() == 3.0);
                CHECK(vec_0.y() == 5.0);
                CHECK(vec_0.z() == 7.0);
                CHECK(vec_0.is_point() == false);
                CHECK(vec_0.is_vector() == true);
        }
}

/// test operator+=(...)
TEST_CASE("tuple::operator+(...) test")
{
        /// point + vector
        {
                raytracer::tuple pt_1  = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const pt_new      = pt_1 + vec_1;

                CHECK(pt_new.x() == 3.0);
                CHECK(pt_new.y() == 5.0);
                CHECK(pt_new.z() == 7.0);
                CHECK(pt_new.is_point() == true);
                CHECK(pt_new.is_vector() == false);
        }

        /// vector + vector
        {
                raytracer::tuple vec_0 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const vec_new     = vec_0 + vec_1;

                CHECK(vec_new.x() == 3.0);
                CHECK(vec_new.y() == 5.0);
                CHECK(vec_new.z() == 7.0);
                CHECK(vec_new.is_point() == false);
                CHECK(vec_new.is_vector() == true);
        }
}

/// test operator-=(...)
TEST_CASE("tuple::operator-=(...) test")
{
        /// point - vector
        {
                raytracer::tuple pt_1  = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);

                pt_1 -= vec_1;

                CHECK(pt_1.x() == -1.0);
                CHECK(pt_1.y() == -1.0);
                CHECK(pt_1.z() == -1.0);
                CHECK(pt_1.is_point() == true);
                CHECK(pt_1.is_vector() == false);
        }

        /// point - point
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple pt_2 = raytracer::create_point(2.0, 3.0, 4.0);

                pt_1 -= pt_2;

                CHECK(pt_1.x() == -1.0);
                CHECK(pt_1.y() == -1.0);
                CHECK(pt_1.z() == -1.0);
                CHECK(pt_1.is_point() == false);
                CHECK(pt_1.is_vector() == true);
        }

        /// vector - vector
        {
                raytracer::tuple vec_0 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);

                vec_0 -= vec_1;

                CHECK(vec_0.x() == -1.0);
                CHECK(vec_0.y() == -1.0);
                CHECK(vec_0.z() == -1.0);
                CHECK(vec_0.is_point() == false);
                CHECK(vec_0.is_vector() == true);
        }
}

/// test binary operator-=(...)
TEST_CASE("tuple::operator-(...) test")
{
        /// point - vector
        {
                raytracer::tuple pt_1  = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);

                auto const pt_0 = pt_1 - vec_1;

                CHECK(pt_0.x() == -1.0);
                CHECK(pt_0.y() == -1.0);
                CHECK(pt_0.z() == -1.0);
                CHECK(pt_0.is_point() == true);
                CHECK(pt_0.is_vector() == false);
        }

        /// point - point
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0, 2.0, 3.0);
                raytracer::tuple pt_2 = raytracer::create_point(2.0, 3.0, 4.0);
                auto const pt_0       = pt_1 - pt_2;

                CHECK(pt_0.x() == -1.0);
                CHECK(pt_0.y() == -1.0);
                CHECK(pt_0.z() == -1.0);
                CHECK(pt_0.is_point() == false);
                CHECK(pt_0.is_vector() == true);
        }

        /// vector - vector
        {
                raytracer::tuple vec_0 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const vec_ret     = vec_0 - vec_1;

                CHECK(vec_ret.x() == -1.0);
                CHECK(vec_ret.y() == -1.0);
                CHECK(vec_ret.z() == -1.0);
                CHECK(vec_ret.is_point() == false);
                CHECK(vec_ret.is_vector() == true);
        }
}

/// test scalar operations
TEST_CASE("tuple::operator[*,/,-]() test")
{
        /// multiplication
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0, 2.0, 3.0);
                auto const pt_2       = pt_1 * 2.0;

                CHECK(pt_2.x() == 2.0);
                CHECK(pt_2.y() == 4.0);
                CHECK(pt_2.z() == 6.0);
                CHECK(pt_2.is_point() == true);
                CHECK(pt_2.is_vector() == false);

                raytracer::tuple vec_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                auto const vec_2       = vec_1 * 2.0;

                CHECK(vec_2.x() == 2.0);
                CHECK(vec_2.y() == 4.0);
                CHECK(vec_2.z() == 6.0);
                CHECK(vec_2.is_point() == false);
                CHECK(vec_2.is_vector() == true);
        }

        /// division
        {
                raytracer::tuple pt_1 = raytracer::create_point(2.0, 4.0, 6.0);
                auto const pt_2       = pt_1 / 2.0;

                CHECK(pt_2.x() == 1.0);
                CHECK(pt_2.y() == 2.0);
                CHECK(pt_2.z() == 3.0);
                CHECK(pt_2.is_point() == true);
                CHECK(pt_2.is_vector() == false);

                raytracer::tuple vec_1 = raytracer::create_vector(2.0, 4.0, 6.0);
                auto const vec_2       = vec_1 / 2.0;

                CHECK(vec_2.x() == 1.0);
                CHECK(vec_2.y() == 2.0);
                CHECK(vec_2.z() == 3.0);
                CHECK(vec_2.is_point() == false);
                CHECK(vec_2.is_vector() == true);
        }

        /// negation
        {
                raytracer::tuple pt_1 = raytracer::create_point(1.0, 2.0, 3.0);
                auto const pt_minus_1 = -pt_1;

                CHECK(pt_minus_1.x() == -1.0);
                CHECK(pt_minus_1.y() == -2.0);
                CHECK(pt_minus_1.z() == -3.0);
                CHECK(pt_minus_1.is_point() == true);
                CHECK(pt_minus_1.is_vector() == false);

                raytracer::tuple vec_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                auto const vec_minus_1 = -vec_1;

                CHECK(vec_minus_1.x() == -1.0);
                CHECK(vec_minus_1.y() == -2.0);
                CHECK(vec_minus_1.z() == -3.0);
                CHECK(vec_minus_1.is_point() == false);
                CHECK(vec_minus_1.is_vector() == true);
        }
}

/// test magnitude of vector
TEST_CASE("magnitude(tuple)")
{
        /// magnitude of vector is well formed
        raytracer::tuple vec_1 = raytracer::create_vector(1.0, 0.0, 0.0);
        auto const vec_mag     = magnitude(vec_1);
        CHECK(vec_mag == 1.0);
}

/// test normalize of vector
TEST_CASE("normalize(tuple)")
{
        /// common usage first
        {
                raytracer::tuple vec_1     = raytracer::create_vector(1.0, 2.0, 3.0);
                auto const mag_norm_vec_1  = magnitude(normalize(vec_1));
                auto const mag_as_expected = raytracer::epsilon_equal(mag_norm_vec_1, 1.0);

                CHECK(mag_as_expected == true);
        }

        /// attempting to normalize a vector with '0.0' magnitude
        {
                raytracer::tuple vec_1 = raytracer::create_vector(0.0, 0.0, 0.0);

                try {
                        auto const norm_vec_1 = normalize(vec_1);
                }
                catch (std::runtime_error& e) {
                }
        }
}

/// test dot-product of vector
TEST_CASE("dot(tuple, tuple)")
{
        /// common usage first
        {
                raytracer::tuple vec_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_2 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const dot_prod    = dot(vec_1, vec_2);

                CHECK(dot_prod == 20.0);
        }
}

/// test cross-product of vector, vector
TEST_CASE("cross(tuple, tuple)")
{
        /// common usage first : vec_1 x vec_2
        {
                raytracer::tuple vec_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_2 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const cross_prod  = cross(vec_1, vec_2);

                CHECK(cross_prod.x() == -1.0);
                CHECK(cross_prod.y() == 2.0);
                CHECK(cross_prod.z() == -1.0);
                CHECK(cross_prod.is_point() == false);
                CHECK(cross_prod.is_vector() == true);
        }

        /// common usage first : vec_2 x vec_1
        {
                raytracer::tuple vec_1 = raytracer::create_vector(1.0, 2.0, 3.0);
                raytracer::tuple vec_2 = raytracer::create_vector(2.0, 3.0, 4.0);
                auto const cross_prod  = cross(vec_2, vec_1);

                CHECK(cross_prod.x() == 1.0);
                CHECK(cross_prod.y() == -2.0);
                CHECK(cross_prod.z() == 1.0);
                CHECK(cross_prod.is_point() == false);
                CHECK(cross_prod.is_vector() == true);
        }
}

TEST_CASE("test raytracer::reflect(...) correct results")
{
        namespace RT = raytracer;

        /// tc-01
        {
                auto const tc_in_vec    = RT::create_vector(1.0, -1.0, 0.0);
                auto const tc_in_norm   = RT::create_vector(0.0, 1.0, 0.0);
                auto const exp_refl_vec = RT::create_vector(1.0, 1.0, 0.0);
                auto const got_refl_vec = RT::reflect(tc_in_vec, tc_in_norm);

                CHECK(got_refl_vec == exp_refl_vec);
        }

        /// tc-02
        {
                auto const tc_in_vec    = RT::create_vector(0.0, -1.0, 0.0);
                auto const tc_in_norm   = RT::create_vector(RT::SQRT_2_BY_2F, RT::SQRT_2_BY_2F, 0.0);
                auto const exp_refl_vec = RT::create_vector(1.0, 0.0, 0.0);
                auto const got_refl_vec = RT::reflect(tc_in_vec, tc_in_norm);

                CHECK(got_refl_vec == exp_refl_vec);
        }
}
