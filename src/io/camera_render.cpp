/*
 * @file
 *        camera_render.cpp
 *
 * @purpose
 *        implements various rendering routines for the camera
 **/

/// system includes
#include <pthread.h>
#include <sched.h> /// for cpu_set_t

/// c++ includes
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <string>
#include <thread>
#include <vector>

/// 3rd-party libraries
#include "concurrentqueue/concurrentqueue.h"

/// our includes
#include "common/include/benchmark.hpp"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/render_params.hpp"
#include "io/world.hpp"
#include "primitives/matrix.hpp"
#include "primitives/ray.hpp"
#include "primitives/tuple.hpp"
#include "utils/execution_profiler.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// this is the top-level rendering routine.
        ///
        /// it returns a canvas instance, which is the result of rendering
        /// 'the_world' with supplied 'rendering_params'.
        canvas camera::render(world const& the_world, config_render_params const& rendering_params) const
        {
                ASSERT(rendering_params.render_style() != rendering_style::RENDERING_STYLE_INVALID);

                /// ------------------------------------------------------------
                /// dump some information about how the rendering will be
                /// performed.
                LOG_INFO("rendering parameters: '%s'", rendering_params.stringify().c_str());

                auto bm_params = [&]() -> benchmark_t<> {
                        if (!rendering_params.benchmark()) {
                                return {""};
                        }

                        return {"benchmarking camera::render(...)",                /// user-defined-message
                                rendering_params.benchmark_rounds(),               /// iterations
                                rendering_params.benchmark_num_discard_initial()}; /// to-discount-cache-effects
                }();

                auto rendered_canvas = bm_params.benchmark(&camera::perform_rendering, *this,
                                                           the_world, /// the-scenery
                                                           rendering_params);

                bm_params.show_stats();
                return rendered_canvas;
        }

        /*
         * only private functions from this point onwards
         **/

        /// --------------------------------------------------------------------
        /// this is the main rendering routine. rendering parameters are
        /// unpacked, and applied to the rendering operation that this function
        /// launches.
        canvas camera::perform_rendering(world const& the_world,
                                         config_render_params const& rendering_params) const
        {
                /// ------------------------------------------------------------
                /// for 'show-as-we-go'
                auto x11_display = [&]() -> std::unique_ptr<xcb_display> {
                        if (rendering_params.online()) {
                                return xcb_display::create_display(horiz_size_, vert_size_);
                        }

                        return nullptr;
                }();

                /// ------------------------------------------------------------
                /// create work-queue of 'render_work_items' using various
                /// rendering parameters that are passed.
                auto const hw_threads = rendering_params.hw_threads();
                auto work_q =
                        [&](rendering_style const& S) -> moodycamel::ConcurrentQueue<render_work_items> {
                        switch (S) {
                        case rendering_style::RENDERING_STYLE_SCANLINE:
                                return scanline_work_queue(hw_threads);

                        case rendering_style::RENDERING_STYLE_HILBERT:
                                return hilbert_work_queue(hw_threads);

                        default:
                        case rendering_style::RENDERING_STYLE_INVALID:
                                break;
                        }

                        ASSERT_FAIL("invalid / unknown rendering style");

                        /// not-reached
                        return moodycamel::ConcurrentQueue<render_work_items>{};
                }(rendering_params.render_style());

                /// ------------------------------------------------------------
                /// destination canvas on which the world will be rendered.
                auto dst_canvas = canvas::create_binary(horiz_size_, vert_size_);

                /// ------------------------------------------------------------
                /// let the painters ... paint !
                std::vector<std::thread> rendering_threads(hw_threads);
                for (uint32_t i = 0; i < hw_threads; i++) {
                        rendering_threads[i] = std::thread(pixel_painter,          /// rendering-function
                                                           i,                      /// thread-id
                                                           std::ref(work_q),       /// work-queue
                                                           the_world,              /// the world
                                                           std::ref(dst_canvas),   /// canvas
                                                           std::ref(x11_display)); /// x11-display

                        /// ----------------------------------------------------
                        /// force || pin threads to cores...
                        cpu_set_t cpuset;
                        CPU_ZERO(&cpuset);
                        CPU_SET(i, &cpuset);

                        auto retval = pthread_setaffinity_np(rendering_threads[i].native_handle(),
                                                             sizeof(cpuset), &cpuset);
                        if (retval != 0) {
                                LOG_ERROR("failed to set affinity of thread:%d to core:%d", i, i);
                        }
                }

                /// ------------------------------------------------------------
                /// ... and wait for all of them to terminate
                std::for_each(rendering_threads.begin(), rendering_threads.end(),
                              std::mem_fn(&std::thread::join));

                return dst_canvas;
        }

        /*
         * this function is the workhorse for rendering a bunch of work-items
         * picked from a concurrent queue
         **/
        void
        camera::pixel_painter(int thread_id,                                              /// id-of-thread
                              moodycamel::ConcurrentQueue<render_work_items>& work_queue, /// queue-of-work
                              world const& W,                                             /// rendered-world
                              canvas& dst_canvas,                                         /// on-canvas
                              std::unique_ptr<xcb_display>& x11_display)                  /// x11-display
        {
                bool all_done          = false;
                size_t pixels_rendered = 0;
                size_t jobs_completed  = 0;

                do {
                        render_work_items rw;

                        if (work_queue.try_dequeue(rw)) {
                                for (auto work : rw.work_list) {
                                        /// ------------------------------------
                                        /// compute the color at (x, y) and
                                        /// update the canvas with that
                                        /// information
                                        auto r_color = W.color_at(work.r);
                                        dst_canvas.write_pixel(work.x, work.y, r_color);

                                        if (x11_display != nullptr) {
                                                x11_display->plot_pixel(work.x, work.y, r_color.rgb_u32());
                                        }

                                        pixels_rendered += 1;
                                }

                                jobs_completed += 1;
                        }

                        /// ----------------------------------------------------
                        /// are there more items in work-queue ?
                        auto items_in_q = work_queue.size_approx();
                        if (items_in_q == 0) {
                                all_done = true;
                        }
                } while (!all_done);

                /// ------------------------------------------------------------
                /// this thread is done. dump some stats...
                LOG_DEBUG("thread: %d done, pixels rendered: %ld, total jobs: %ld",
                          thread_id,       /// thread-id
                          pixels_rendered, /// pixels-rendered
                          jobs_completed); /// how-many-total-jobs

                return;
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in scanline
        /// order.
        moodycamel::ConcurrentQueue<render_work_items> camera::scanline_work_queue(uint32_t hw_threads) const
        {
                uint32_t const total_pixels_per_thread = horiz_size_ / hw_threads;
                uint32_t const total_work_items = (horiz_size_ / total_pixels_per_thread) * (vert_size_);

                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                /// ------------------------------------------------------------
                /// generate work for the workers...
                for (uint32_t y = 0; y < vert_size_; y++) {
                        for (uint32_t x = 0; x < horiz_size_; x += total_pixels_per_thread) {
                                render_work_items work_item;
                                work_item.work_list.reserve(total_pixels_per_thread);

                                for (uint32_t i = 0; i < total_pixels_per_thread; i++) {
                                        render_work_item tmp{
                                                x + i,                   /// x-pixel
                                                y,                       /// y-pixel
                                                ray_for_pixel(x + i, y), /// ray-at-(x,y)
                                        };

                                        work_item.work_list.emplace_back(tmp);
                                }

                                wq.enqueue(work_item);
                        }
                }

                LOG_INFO("scanline-work-queue info: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, work-queue length: {%ld} (approx.)",
                         hw_threads, total_pixels_per_thread, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in
        /// hilbert-curve order.
        moodycamel::ConcurrentQueue<render_work_items> camera::hilbert_work_queue(uint32_t hw_threads) const
        {
                auto rot = [](uint32_t n, uint32_t& x, uint32_t& y, uint32_t rx, uint32_t ry) -> void {
                        if (ry == 0) {
                                if (rx == 1) {
                                        x = n - 1 - x;
                                        y = n - 1 - y;
                                }

                                std::swap(x, y);
                        }
                };

                /// ------------------------------------------------------------
                /// an 'n x n' grid (where n is power-of-2) is assumed. for a
                /// point on with 'hilbert-index' we want to find the
                /// corrresponding (x, y) coordinates
                auto conv_hilbert_index_to_xy = [&](uint32_t const n, uint32_t const hilbert_index,
                                                    uint32_t& x, uint32_t& y) -> void {
                        uint32_t rx, ry, s, t = hilbert_index;
                        x = y = 0;

                        for (s = 1; s < n; s *= 2) {
                                rx = 1 & (t / 2);
                                ry = 1 & (t ^ rx);
                                rot(s, x, y, rx, ry);

                                x += s * rx;
                                y += s * ry;
                                t /= 4;
                        }
                };

                /// ------------------------------------------------------------
                /// largest power-of-2 ≥ 'N'
                auto find_largest_pow2_gte = [](uint32_t N) -> uint32_t {
                        --N;

                        N |= N >> 1;
                        N |= N >> 2;
                        N |= N >> 4;
                        N |= N >> 8;
                        N |= N >> 16;

                        return N + 1;
                };

                /// ------------------------------------------------------------
                /// so we have a grid of points of size (max_n by max_n)
                /// large. by virtue of this construction, this grid *must* be a
                /// superset of grid of points (horiz_size_ by vert_size_)
                const uint32_t max_n = find_largest_pow2_gte(std::max(horiz_size_, vert_size_));

                /// ------------------------------------------------------------
                /// points on the hilbert curve range from [0 .. max_n^2)
                const uint32_t min_hilbert_index = 0;
                const uint32_t max_hilbert_index = (max_n * max_n);

                /// ------------------------------------------------------------
                /// each thread gets to work with 'pixels_per_thread' number of
                /// points on the hilbert curve
                const uint32_t pixels_per_thread = (horiz_size_ * vert_size_) / 256;

                moodycamel::ConcurrentQueue<render_work_items> wq(horiz_size_ * vert_size_);

                /// ------------------------------------------------------------
                /// now create work-items from points on this curve
                for (uint32_t hilbert_index = min_hilbert_index; hilbert_index < max_hilbert_index;) {
                        render_work_items work_item;

                        for (uint32_t i = 0; i < pixels_per_thread; i++, hilbert_index++) {
                                uint32_t hilbert_x = 0;
                                uint32_t hilbert_y = 0;

                                conv_hilbert_index_to_xy(max_n, hilbert_index, hilbert_x, hilbert_y);

                                /// --------------------------------------------
                                /// ignore points outside the range. '- 1'
                                /// because we start counting from '0'
                                if (hilbert_x > (horiz_size_ - 1) || (hilbert_y > vert_size_ - 1)) {
                                        continue;
                                }

                                render_work_item tmp{
                                        hilbert_x,                           /// x-pixel
                                        hilbert_y,                           /// y-pixel
                                        ray_for_pixel(hilbert_x, hilbert_y), /// ray
                                };

                                work_item.work_list.push_back(tmp);
                        }

                        wq.enqueue(work_item);
                }

                LOG_INFO("hilbert-curve work-queue info: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, work-queue-length: {%ld} (approx.)",
                         hw_threads, pixels_per_thread, wq.size_approx());

                return wq;
        }

} // namespace raytracer
