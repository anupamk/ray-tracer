/*
 * implement the raytracer triangle
 **/

#include "shapes/triangle.hpp"

/// c++ includes
#include <cmath>
#include <optional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/// our includes
#include "patterns/material.hpp"
#include "primitives/intersection_record.hpp"
#include "primitives/ray.hpp"
#include "shapes/aabb.hpp"
#include "utils/badge.hpp"
#include "utils/constants.hpp"

namespace raytracer
{
        triangle::triangle(tuple pt_1, tuple pt_2, tuple pt_3, bool cast_shadow)
            : shape_interface(cast_shadow)
            , pt_1_(std::move(pt_1))
            , pt_2_(std::move(pt_2))
            , pt_3_(std::move(pt_3))
            , e1_(pt_2_ - pt_1_)
            , e2_(pt_3_ - pt_1_)
            , n_1_(normalize(cross(e2_, e1_)))
            , n_2_(n_1_)
            , n_3_(n_1_)
        {
        }

        triangle::triangle(tuple pt_1, tuple pt_2, tuple pt_3, tuple n_1, tuple n_2, tuple n_3,
                           bool cast_shadow)
            : shape_interface(cast_shadow)
            , pt_1_(std::move(pt_1))
            , pt_2_(std::move(pt_2))
            , pt_3_(std::move(pt_3))
            , e1_(pt_2_ - pt_1_)
            , e2_(pt_3_ - pt_1_)
            , n_1_(std::move(n_1))
            , n_2_(std::move(n_2))
            , n_3_(std::move(n_3))
        {
        }

        tuple triangle::p1() const
        {
                return pt_1_;
        }

        tuple triangle::p2() const
        {
                return pt_2_;
        }

        tuple triangle::p3() const
        {
                return pt_3_;
        }

        tuple triangle::e1() const
        {
                return e1_;
        }

        tuple triangle::e2() const
        {
                return e2_;
        }

        tuple triangle::n1() const
        {
                return n_1_;
        }

        tuple triangle::n2() const
        {
                return n_2_;
        }

        tuple triangle::n3() const
        {
                return n_3_;
        }

        /// --------------------------------------------------------------------
        /// compute intersection of a ray with the triangle
        std::optional<intersection_records> triangle::intersect(the_badge<ray_t>, ray_t const& R) const
        {
                return compute_intersections_(R);
        }

        /// --------------------------------------------------------------------
        /// this function is called to return the normal at a point 'P' on
        /// the surface of the triangle
        tuple triangle::normal_at_local(tuple const& P, intersection_record const& xs) const
        {
                return (this->n_2_ * xs.u() + this->n_3_ * xs.v() + this->n_1_ * (1 - xs.u() - xs.v()));
        }

        /// --------------------------------------------------------------------
        /// return 'true' iff the ray 'R' can intersect this triangle before
        /// 'distance'.
        ///
        /// return 'false' otherwise
        bool triangle::has_intersection_before(the_badge<ray_t> b, ray_t const& R, double distance) const
        {
                auto maybe_xs_records = compute_intersections_(R);

                if (!maybe_xs_records.has_value()) {
                        return false;
                }

                auto const& xs_records = maybe_xs_records.value();
                for (auto const& xs_i : xs_records) {
                        auto const where = xs_i.where();
                        if ((where >= EPSILON) && (where < distance)) {
                                return true;
                        }
                }

                return false;
        }

        /// --------------------------------------------------------------------
        /// return the bounding box for this instance of the triangle.
        aabb triangle::bounds_of() const
        {
                aabb triangle_bb{};

                triangle_bb.add_point(pt_1_);
                triangle_bb.add_point(pt_2_);
                triangle_bb.add_point(pt_3_);

                return triangle_bb;
        }

        /// --------------------------------------------------------------------
        /// stringified representation of triangle's information
        std::string triangle::stringify() const
        {
                std::stringstream ss("");

                ss << "ray-tracer-triangle: {"
                   << "(pt_1: " << this->p1() << ", "
                   << "pt_2: " << this->p2() << ", "
                   << "pt_3: " << this->p3() << "), "
                   << "e1: " << this->e1() << ", "
                   << "e2: " << this->e2() << ", "
                   << "n1: " << this->n1() << ", "
                   << "n2: " << this->n2() << ", "
                   << "n3: " << this->n3() << ", "
                   << "material: " << this->get_material() << ", "
                   << "grouped: " << this->is_grouped() << "}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// all the points, and normals (at those points) must be equal
        bool operator==(triangle const& lhs, triangle const& rhs)
        {
                return ((lhs.p1() == rhs.p1()) && (lhs.p2() == rhs.p2()) && (lhs.p3() == rhs.p3()) &&
                        (lhs.n1() == rhs.n1()) && (lhs.n2() == rhs.n2()) && (lhs.n3() == rhs.n3()));
        }

        /*
         * only private functions from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// this function is called to compute the result of a ray 'R'
        /// intersecting a triangle.
        ///
        /// to actually explain the Möller–Trumbore Triangle intersection we
        /// need to explain what barycentric coordinates are, and where all this
        /// is coming from. for now we just provide trivial comments.
        std::optional<intersection_records> triangle::compute_intersections_(ray_t const& R) const
        {
                /// get a vector orthogonal to both 'R' and edge e2
                auto const ray_dir_cross_e2 = cross(R.direction(), e2());

                /// now compute cosine between the other edge and this
                /// orthogonal vector
                auto const det = dot(e1(), ray_dir_cross_e2);

                /// ray is parallel to the triangle
                if (std::fabs(det) < EPSILON) {
                        return std::nullopt;
                }

                /// ray misses the p1-p3 edge
                auto const f            = 1.0 / det;
                auto const p1_to_origin = R.origin() - p1();
                auto const u            = f * dot(p1_to_origin, ray_dir_cross_e2);
                if ((u < 0.0) || (u > 1.0)) {
                        return std::nullopt;
                }

                auto const origin_cross_e1 = cross(p1_to_origin, e1());
                auto const v               = f * dot(R.direction(), origin_cross_e1);

                /// ray misses p2-p3 and p1-p2 edges
                if ((v < 0.0) || ((u + v) > 1.0)) {
                        return std::nullopt;
                }

                /// we have intersection
                auto const t = f * dot(e2(), origin_cross_e1);
                return intersection_records{intersection_record(t, shared_from_this(), u, v)};
        }

} // namespace raytracer
