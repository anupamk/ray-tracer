#pragma once

/// c++ includes
#include <limits>

/// our includes
#include "primitives/tuple.hpp"
#include "utils/constants.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class ray_t;
        class fsize_dense2d_matrix_t;

        /*
         * @brief
         *    this class defines an abstract (not in the usual sense of abstract
         *    classes in programming languages) shape called:
         *
         *       'axis-aligned-bounding-box' or 'aabb' for short.
         *
         *    this helps us accelerate scene renderings by our raytracer
         *    essentially by reducing the amount of work that needs to be
         *    performed.
         *
         *    typically, pixel colors are computed by intersection of a ray with
         *    a set of shapes that describe the scene. when number of objects
         *    comprising a scene is large, this process becomes unweildy.
         *
         *    instead we enclose the objects within an abstract bounding box,
         *    and perform expensive ray-object-intersection iff ray intersects
         *    the bounding box.
         *
         *    these bounding boxes are aligned to the coordinate axis. this is
         *    because computing ray-axis-aligned-bounding-box intersection is
         *    computationally quite cheap.
         **/
        class aabb final
        {
            private:
                tuple min_ = create_point(INF, INF, INF);
                tuple max_ = create_point(-INF, -INF, -INF);

            public:
                /*
                 * @brief
                 *    create a default initialized aabb instance
                 **/
                aabb();

                /*
                 * @brief
                 *    create an aabb instance with specified min and max points.
                 **/
                aabb(tuple min_point, tuple max_point);

            public:
                /*
                 * @brief
                 *    get the min+max points
                 **/
                tuple min() const;
                tuple max() const;

                /*
                 * @brief
                 *    add a point 'p' to this aabb instance.
                 *
                 *    the effect of adding a point is to adjust the dimensions
                 *    of the aabb just-enough to enclose the new point.
                 **/
                void add_point(tuple const& p);

                /*
                 * @brief
                 *    add a bounding box 'b' to this aabb instance.
                 *
                 *    the effect of adding a bounding-box is to resize the
                 *    old-box to contain the new-box
                 **/
                void add_box(const aabb& new_bb);

                /*
                 * @brief
                 *    does this bounding-box instance contain the point 'p' ?
                 *
                 *    when this returns 'true' it implies that the point 'p' is
                 *    contained within this instance of the bounding-box.
                 *
                 *    when this returns 'false' it implies that the point 'p' is
                 *    not contained within this instance of the bounding-box.
                 **/
                bool contains(tuple const& p) const;

                /*
                 * @brief
                 *    does this bounding-box instance contain the another
                 *    bounding-box 'b' ?
                 *
                 *    when this returns 'true' it implies that the bounding-box
                 *    'b' is contained within this bounding-box.
                 *
                 *    when this returns 'false' it implies that the bounding-box
                 *    'b' is not contained within this bounding-box.
                 *
                 *    a box is contained within another box when its extremeties
                 *    both lie within the other box.
                 **/
                bool contains(aabb const& b) const;

                /*
                 * @brief
                 *    apply a transformation matrix to this instance of the
                 *    bounding box.
                 *
                 *    remember that we are dealing with 'axis-aligned' boxes, so
                 *    just applying the transformation-matrix to the 'min' and
                 *    'max' of this bounding-box is *not* sufficient.
                 *
                 *    transformation-matrix needs to be applied to *all* (8)
                 *    points of the box. a new bounding box is then constructed
                 *    with these transformed points.
                 **/
                aabb transform(fsize_dense2d_matrix_t const& mat) const;

                /*
                 * @brief
                 *    does the ray 'R' intersect the bounding box ?
                 *
                 * @return
                 *    'true' if it does, 'false' otherwise
                 **/
                bool intersects(ray_t const& R) const;

                /*
                 * @brief
                 *    split a bounding box into two non-overlapping boxes, such
                 *    that they cover the same volume as the original bounding
                 *    box.
                 *
                 *    when the box is 'perfect' i.e. equal dimension along all
                 *    axes, we split along the x-axis.
                 *
                 * @return
                 *    std::pair<left-bounding-box, right-bounding-box> the
                 *    'left' and 'right' box.
                 **/
                std::pair<aabb, aabb> split_bounds() const;

            private:
                /// ------------------------------------------------------------
                /// for each of the x,y,z axes check where a ray intersects
                /// corresponding planes.
                std::tuple<double, double> check_axes_(double origin, double direction, double min,
                                                       double max) const;
        };

} // namespace raytracer
