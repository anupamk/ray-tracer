/*
 * collection of routines to generate work for rendering threads.
 **/

#pragma once

/// c++ includes
#include <cstdint>
#include <vector>

/// 3rd-party libraries
#include "concurrentqueue/concurrentqueue.h"

/// our includes
#include "io/camera.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{

        /// --------------------------------------------------------------------
        /// a render-work item is a ray at a specific place on the canvas
        struct render_work_item final {
                uint32_t x;
                uint32_t y;
                ray_t r;
        };

        /// --------------------------------------------------------------------
        /// a bunch of render-work items is what is what gets handled by a
        /// single rendering thread
        struct render_work_items final {
            public:
                std::vector<render_work_item> work_list;
        };

        /*
         * @brief
         *    generate work in scanline order. a 'scanline' just sweeps across
         *    the canvas from left -> right, top -> bottom.
         **/
        moodycamel::ConcurrentQueue<render_work_items> generate_scanline_work_queue(uint32_t num_hw_threads,
                                                                                    camera const& C);

        /*
         * @brief
         *    generate work in hilbert-curve order. see
         *    https://en.wikipedia.org/wiki/Hilbert_curve for more information
         **/
        moodycamel::ConcurrentQueue<render_work_items> generate_hilbert_work_queue(uint32_t num_hw_threads,
                                                                                   camera const& C);

} // namespace raytracer
