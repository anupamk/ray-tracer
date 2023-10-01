/*
 * implement the raytracer canvas rendering and rendering related routines.
 **/

/// c++ includes
#include <algorithm>
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <typeindex>
#include <utility>

/// sdl2 includes
#include <SDL2/SDL.h>

/// our includes
#include "common/include/benchmark.hpp"
#include "common/include/string_utils.hpp"
#include "io/canvas.hpp"
#include "primitives/color.hpp"
#include "utils/utils.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// generate work in scanline ordering
        moodycamel::ConcurrentQueue<render_work_items> canvas::gen_scanline_work_queue() const
        {
                auto const total_pixels_per_thread = the_camera_.hsize() / num_render_cores_;
                auto const total_work_items =
                        (the_camera_.hsize() / total_pixels_per_thread) * (the_camera_.vsize());

                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                /// ------------------------------------------------------------
                /// generate work for the workers...
                for (uint32_t y = 0; y < the_camera_.vsize(); y++) {
                        for (uint32_t x = 0; x < the_camera_.hsize(); x += total_pixels_per_thread) {
                                render_work_items work_item;
                                work_item.work_list.reserve(total_pixels_per_thread);

                                for (int i = 0; i < total_pixels_per_thread; i++) {
                                        render_work_item tmp{
                                                x + i,                               /// x-pixel
                                                y,                                   /// y-pixel
                                                the_camera_.ray_for_pixel(x + i, y), /// ray-at-(x,y)
                                        };

                                        work_item.work_list.push_back(tmp);
                                }

                                wq.enqueue(work_item);
                        }
                }

                LOG_INFO("multi-threaded-renderer information: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, total-work-items: {%d}, "
                         "work-queue-items: {%ld} (approx.)",
                         num_render_cores_, total_pixels_per_thread, total_work_items, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate work-queue in hilber-curve order
        moodycamel::ConcurrentQueue<render_work_items> canvas::gen_hilbert_work_queue() const
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

                auto d2xy = [&](int n, int d, uint32_t& x, uint32_t& y) -> void {
                        uint32_t rx, ry, s, t = d;
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

                auto find_largest_2pow = [](uint32_t x) -> uint32_t {
                        uint32_t pow2 = 0;

                        do {
                                pow2 = x;
                                x    = x & (x - 1);
                        } while (x);

                        return pow2;
                };

                auto const total_points = the_camera_.hsize() * the_camera_.vsize();
                auto const order        = find_largest_2pow(total_points);

                auto const total_pixels_per_thread = the_camera_.hsize() / num_render_cores_;
                auto const total_work_items =
                        (the_camera_.hsize() / total_pixels_per_thread) * (the_camera_.vsize());

                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                for (uint32_t D = 0; D < order;) {
                        render_work_items work_item;
                        work_item.work_list.reserve(total_pixels_per_thread);

                        for (uint32_t i = 0; i < total_pixels_per_thread; i++, D++) {
                                uint32_t x = 0;
                                uint32_t y = 0;

                                d2xy(order, D, x, y);

                                render_work_item tmp{
                                        x,                               /// x-pixel
                                        y,                               /// y-pixel
                                        the_camera_.ray_for_pixel(x, y), /// ray-at-(x,y)
                                };

                                work_item.work_list.push_back(tmp);
                        }

                        wq.enqueue(work_item);
                }

                LOG_INFO("multi-threaded-renderer information: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, total-work-items: {%d}, "
                         "work-queue-items: {%ld} (approx.)",
                         num_render_cores_, total_pixels_per_thread, total_work_items, wq.size_approx());

                return wq;
        }

        /// ------------------------------------------------------------
        /// multi-threaded rendering of a canvas
        void canvas::mt_renderer()
        {
                PROFILE_SCOPE;

                auto work_queue = gen_scanline_work_queue();
                /// auto work_queue = generate_hilbert_work_queue(num_hw_threads, C);

                /// ------------------------------------------------------------
                /// start the coloring threads
                std::vector<std::thread> rendering_threads(num_render_cores_);

                for (auto i = 0; i < num_render_cores_; i++) {
                        rendering_threads[i] = std::thread(&canvas::coloring_worker, /// rendering-function
                                                           *this,                    /// this object
                                                           i,                        /// thread-id
                                                           std::ref(work_queue));    /// work-queue

                        /// ----------------------------------------------------
                        /// force-pin threads to cores...
                        cpu_set_t cpuset;
                        CPU_ZERO(&cpuset);
                        CPU_SET(i, &cpuset);

                        auto retval = pthread_setaffinity_np(rendering_threads[i].native_handle(),
                                                             sizeof(cpuset), &cpuset);
                        if (retval != 0) {
                                LOG_ERROR("failed to set affinity of thread:%d to core:%d", i, i);
                        }
                }

                /// wait for all of them to terminate
                std::for_each(rendering_threads.begin(), rendering_threads.end(),
                              std::mem_fn(&std::thread::join));

                return;
        }

        /// --------------------------------------------------------------------
        /// worker-thread function
        void canvas::coloring_worker(int thread_id,
                                     moodycamel::ConcurrentQueue<render_work_items>& work_queue)
        {
                bool all_done          = false;
                size_t pixels_rendered = 0;
                size_t jobs_completed  = 0;

                /// do some work
                do {
                        render_work_items rw;
                        if (work_queue.try_dequeue(rw)) {
                                for (auto work : rw.work_list) {
                                        auto r_color = the_world_.color_at(work.r);
                                        write_pixel(work.x, work.y, r_color);

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
        }

} // namespace raytracer
