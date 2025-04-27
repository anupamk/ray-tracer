/*
 * @file
 *        camera_render.cpp
 *
 * @purpose
 *        implements various rendering routines for the camera
 **/

/// system includes
#include <pthread.h>
#include <sched.h>
#include <stddef.h>

/// c++ includes
#include <algorithm>
#include <cstdint>
#include <functional>
#include <memory>
#include <thread>
#include <utility>
#include <vector>

/// our includes
#include "common/include/assert_utils.h"
#include "common/include/benchmark.hpp"
#include "common/include/logging.h"
#include "concurrentqueue/concurrentqueue.h"
#include "io/camera.hpp"
#include "io/canvas.hpp"
#include "io/render_params.hpp"
#include "io/world.hpp"
#include "io/xcb_display.hpp"
#include "platform_utils/thread_utils.hpp"
#include "primitives/color.hpp"

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
                render_params_ = rendering_params;

                /// ------------------------------------------------------------
                /// dump some information about how the rendering will be
                /// performed.
                LOG_INFO("rendering parameters: '%s'", render_params_.stringify().c_str());

                auto bm_params = [&]() -> benchmark_t<> {
                        if (!rendering_params.benchmark()) {
                                return {""};
                        }

                        return {"benchmarking camera::render(...)",                /// user-defined-message
                                rendering_params.benchmark_rounds(),               /// iterations
                                rendering_params.benchmark_num_discard_initial()}; /// to-discount-cache-effects
                }();

                /// ------------------------------------------------------------
                /// render the scene
                auto rendered_canvas = bm_params.benchmark(&camera::perform_rendering, *this,
                                                           the_world); /// the-scenery

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
        canvas camera::perform_rendering(world const& the_world) const
        {
                /// ------------------------------------------------------------
                /// for 'show-as-we-go'
                auto x11_display = [&]() -> std::unique_ptr<xcb_display> {
                        if (render_params_.online()) {
                                return xcb_display::create_display(horiz_size_, vert_size_);
                        }

                        return nullptr;
                }();

                /// ------------------------------------------------------------
                /// create work-queue of 'render_work_items' using various
                /// rendering parameters that are passed.
                auto work_q =
                        [&](rendering_style const& S) -> moodycamel::ConcurrentQueue<render_work_items> {
                        switch (S) {
                        case rendering_style::RENDERING_STYLE_SCANLINE:
                                return scanline_work_queue();

                        case rendering_style::RENDERING_STYLE_HILBERT:
                                return hilbert_work_queue();

                        case rendering_style::RENDERING_STYLE_TILE:
                                return tile_work_queue();

                        default:
                        case rendering_style::RENDERING_STYLE_INVALID:
                                break;
                        }

                        ASSERT_FAIL("invalid / unknown rendering style");

                        /// not-reached
                        return moodycamel::ConcurrentQueue<render_work_items>{};
                }(render_params_.render_style());

                /// ------------------------------------------------------------
                /// destination canvas on which the world will be rendered.
                auto dst_canvas = canvas::create_binary(horiz_size_, vert_size_);

                /// ------------------------------------------------------------
                /// let the painters ... paint !
                auto const hw_threads = render_params_.hw_threads();
                std::vector<std::thread> rendering_threads(hw_threads);

                for (uint32_t i = 0; i < hw_threads; i++) {
                        rendering_threads[i] = std::thread(&camera::pixel_painter, /// rendering-function
                                                           *this,                  /// instance
                                                           i,                      /// thread-id
                                                           std::ref(work_q),       /// work-queue
                                                           the_world,              /// the world
                                                           std::ref(dst_canvas),   /// canvas
                                                           std::ref(x11_display)); /// x11-display

                        /// ----------------------------------------------------
                        /// try to force || pin threads to cores...
                        auto retval = platform_utils::thread_utils::set_thread_affinity(
                                rendering_threads[i].native_handle(), i);

                        if (retval != 0) {
                                /// --------------------------------------------
                                /// an error for sure. not a fatal one though.
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
        void camera::pixel_painter(int thread_id, moodycamel::ConcurrentQueue<render_work_items>& work_queue,
                                   world const& W, canvas& dst_canvas,
                                   std::unique_ptr<xcb_display>& x11_display)
        {
                bool all_done                   = false;
                size_t pixels_rendered          = 0;
                size_t jobs_completed           = 0;
                static double const pixel_delta = render_params_.antialias() ? 0.5 : 0.0;

                do {
                        render_work_items rw;

                        if (work_queue.try_dequeue(rw)) {
                                for (auto const& work : rw.work_list) {
                                        /// ------------------------------------
                                        /// compute the color at (x, y) and
                                        /// update the canvas with that
                                        /// information
                                        auto r_color = adaptively_color_a_pixel_at(W,            /// world
                                                                                   work.x,       /// x
                                                                                   work.y,       /// y
                                                                                   pixel_delta); /// delta

                                        dst_canvas.write_pixel(work.x, work.y, r_color);

                                        if (x11_display != nullptr) {
                                                /// ----------------------------
                                                /// no locking is needed.
                                                ///
                                                /// this is because each thread
                                                /// handles different / distinct
                                                /// set of pixels.
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
        /// adaptively compute pixel color at a specific point (x, y).
        color camera::adaptively_color_a_pixel_at(world const& W, double x, double y, double delta) const
        {
                /// ------------------------------------------------------------
                /// break out of recursion. we have reached the desired
                /// color-difference.
                if (delta < config_render_params::AA_COLOR_DIFF_THRESHOLD) {
                        return pixel_color_at(W, x, y);
                }

                /*
                 * 4-corners + 1-center, for a total of 5 points per pixel
                 * (marked by 'x' in the ascii-art below) whose colors we are
                 * going to compute, and merge into the final color.
                 *
                 *            |
                 *    (-1,1)  |    (1,1)
                 *       x----+----x
                 *       |    |    |
                 *  -----+----x----+----- (xy-center-color)
                 *       |    |    |
                 *       x----+----x
                 *    (-1,-1) |    (1, -1)
                 *            |
                 *
                 * so, the ordering of the dx[...], and dy[...] arrays (below)
                 * is *significant*, with dx[i] and dy[i] *together* defining
                 * a single point of a pixel as described above.
                 **/
                static constexpr uint8_t points_per_pixel = 5;
                static constexpr double dx[]              = {1.0, 1.0, -1.0, -1.0};
                static constexpr double dy[]              = {1.0, -1.0, -1.0, 1.0};

                auto const xy_center_color = pixel_color_at(W, x, y);
                auto pixel_color           = xy_center_color * (1.0 / points_per_pixel);

                for (int i = 0; i < 4; i++) {
                        auto const c_x = x + dx[i] * delta;
                        auto const c_y = y + dy[i] * delta;

                        /// ----------------------------------------------------
                        /// this is the adaptive color sampling part.
                        ///
                        /// just compute the difference between corner, and
                        /// center colors. when that difference is more than the
                        /// threshold, recursively sub-divide that quarter of
                        /// the pixel, and recompute.
                        ///
                        /// this approach is superior to the brute-force (simple
                        /// to implement, and wasteful from computation
                        /// perspective) approach of *always* projecting a fixed
                        /// number of random rays per pixel, and sampling the
                        /// colors.
                        auto corner_color    = pixel_color_at(W, c_x, c_y);
                        auto color_diff      = xy_center_color - corner_color;
                        float component_diff = std::fabs(color_diff.R() + color_diff.G() + color_diff.B());

                        if (component_diff > config_render_params::AA_COLOR_DIFF_THRESHOLD) {
                                corner_color = adaptively_color_a_pixel_at(W, c_x, c_y, delta / 2.0);
                        }

                        pixel_color += corner_color * (1.0 / points_per_pixel);
                }

                return pixel_color;
        }

        /// --------------------------------------------------------------------
        /// simple computation of pixel color at a specific point (x,y)
        color camera::pixel_color_at(world const& W, double x, double y) const
        {
                return W.color_at(ray_for_pixel(x, y));
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in scanline
        /// order.
        moodycamel::ConcurrentQueue<render_work_items> camera::scanline_work_queue() const
        {
                /*
                 * an instance of 'render_work_items' has 'PIXELS_PER_WORK_ITEM'
                 * worth of pixels that will be rendered at a time.
                 *
                 * each thread picks a new 'work_item' from the work-queue once
                 * it is done with the current one.
                 **/
                static constexpr uint32_t PIXELS_PER_WORK_ITEM = 256;

                const long total_pixel_count        = horiz_size_ * vert_size_;
                const auto work_items_and_remainder = std::div(total_pixel_count, PIXELS_PER_WORK_ITEM);

                const auto total_work_items =
                        (work_items_and_remainder.quot + work_items_and_remainder.rem ? 1 : 0);

                uint32_t x_pixel = 0;
                uint32_t y_pixel = 0;

                /// ------------------------------------------------------------
                /// generate work for the workers
                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                /// ------------------------------------------------------------
                /// integer division worth items first
                render_work_items work_item;
                const long int_div_pixels = work_items_and_remainder.quot * PIXELS_PER_WORK_ITEM;
                for (uint32_t pixel_count = 0; pixel_count < int_div_pixels; pixel_count++) {
                        /// ----------------------------------------------------
                        /// we now have pixels for a single work-item, add it to
                        /// the work-queue.
                        if ((pixel_count % PIXELS_PER_WORK_ITEM) == 0) {
                                wq.enqueue(work_item);
                                work_item.work_list.clear();
                        }

                        render_work_item tmp{double(x_pixel), double(y_pixel)};
                        work_item.work_list.emplace_back(tmp);

                        /// ----------------------------------------------------
                        /// scanline processing of pixels making up this image
                        x_pixel += 1;
                        if (x_pixel >= horiz_size_) {
                                x_pixel = 0;
                                y_pixel += 1;
                        }
                }

                /// ------------------------------------------------------------
                /// and then the lone straggler
                render_work_items straggler_work_item;
                for (uint32_t pixel_count = int_div_pixels; pixel_count < total_pixel_count; pixel_count++) {
                        render_work_item tmp{double(x_pixel), double(y_pixel)};
                        straggler_work_item.work_list.emplace_back(tmp);

                        x_pixel += 1;
                }
                wq.enqueue(straggler_work_item);

                LOG_INFO("scanline-work-queue info: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, work-queue length: {%ld} (approx.)",
                         render_params_.hw_threads(), PIXELS_PER_WORK_ITEM, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in scanline
        /// order.
        moodycamel::ConcurrentQueue<render_work_items> camera::scanline_work_queue_1() const
        {
                uint32_t const total_pixels_per_thread = horiz_size_ / render_params_.hw_threads();
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
                                                double(x + i), /// x-pixel
                                                double(y)      /// y-pixel
                                        };

                                        work_item.work_list.emplace_back(tmp);
                                }

                                wq.enqueue(work_item);
                        }
                }

                LOG_INFO("scanline-work-queue info: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, work-queue length: {%ld} (approx.)",
                         render_params_.hw_threads(), total_pixels_per_thread, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in
        /// hilbert-curve order.
        moodycamel::ConcurrentQueue<render_work_items> camera::hilbert_work_queue() const
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
                                        double(hilbert_x + i), /// x-pixel
                                        double(hilbert_y)      /// y-pixel
                                };

                                work_item.work_list.emplace_back(tmp);
                        }

                        wq.enqueue(work_item);
                }

                LOG_INFO("hilbert-curve work-queue info: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, work-queue-length: {%ld} (approx.)",
                         render_params_.hw_threads(), pixels_per_thread, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate a work-queue comprising 'rendering work items' in tiling
        /// order.
        moodycamel::ConcurrentQueue<render_work_items> camera::tile_work_queue() const
        {
                /*
                 * tile dimensions
                 **/
                auto const hw_threads     = render_params_.hw_threads();
                uint16_t const tile_dim_x = horiz_size_ / hw_threads;
                uint16_t const tile_dim_y = vert_size_ / hw_threads;

                /// ------------------------------------------------------------
                /// tiles create a (X, Y) grid over the entire canvas. this
                /// function is called to generate rendering work for a tile
                /// located at a specific row (R) and a specific column (C)
                auto const gen_tile_xy_work = [&](uint32_t R, uint32_t C) -> render_work_items {
                        uint32_t const start_x = C * tile_dim_x;
                        uint32_t const end_x   = start_x + tile_dim_x;

                        uint32_t const start_y = R * tile_dim_y;
                        uint32_t const end_y   = start_y + tile_dim_y;

                        render_work_items work_item;
                        work_item.work_list.reserve(tile_dim_x * tile_dim_y);

                        for (uint32_t y = start_y; y < end_y; y++) {
                                for (uint32_t x = start_x; x < end_x; x++) {
                                        render_work_item tmp{
                                                double(x), /// x-pixel
                                                double(y), /// y-pixel
                                        };

                                        work_item.work_list.emplace_back(tmp);
                                }
                        }

                        return work_item;
                };

                /// ------------------------------------------------------------
                /// in the ensuing 'tile-matrix' how many rows + column are
                /// there.
                uint32_t const num_rows    = vert_size_ / tile_dim_y;
                uint32_t const num_cols    = horiz_size_ / tile_dim_x;
                uint32_t const total_tiles = num_cols * num_rows;

                moodycamel::ConcurrentQueue<render_work_items> wq(total_tiles);

                /// ------------------------------------------------------------
                /// generate work for the workers
                for (uint32_t r = 0; r < num_rows; ++r) {
                        for (uint32_t c = 0; c < num_cols; ++c) {
                                auto work_item = gen_tile_xy_work(r, c);
                                wq.enqueue(work_item);
                        }
                }

                LOG_INFO(
                        "tile-work-queue info: "
                        "total-threads: {%d}, tile-dimensions: {x:%d, y:%d} pixels, work-queue length: {%ld} (approx.)",
                        hw_threads, tile_dim_x, tile_dim_y, wq.size_approx());

                return wq;
        }

} // namespace raytracer
