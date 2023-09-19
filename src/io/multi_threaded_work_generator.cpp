#include "io/multi_threaded_work_generator.hpp"
#include "common/include/logging.h"

namespace raytracer
{
        /*
         * @brief
         *    generate work in scanline order. a 'scanline' just sweeps across
         *    the canvas from left -> right, top -> bottom.
         **/
        moodycamel::ConcurrentQueue<render_work_items> generate_scanline_work_queue(uint32_t num_hw_threads,
                                                                                    camera const& C)
        {
                auto const total_pixels_per_thread = C.hsize() / num_hw_threads;
                auto const total_work_items        = (C.hsize() / total_pixels_per_thread) * (C.vsize());

                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                /// ------------------------------------------------------------
                /// generate work for the workers...
                for (uint32_t y = 0; y < C.vsize(); y++) {
                        for (uint32_t x = 0; x < C.hsize(); x += total_pixels_per_thread) {
                                render_work_items work_item;
                                work_item.work_list.reserve(total_pixels_per_thread);

                                for (int i = 0; i < total_pixels_per_thread; i++) {
                                        render_work_item tmp{
                                                x + i,                     /// x-pixel
                                                y,                         /// y-pixel
                                                C.ray_for_pixel(x + i, y), /// ray-at-(x,y)
                                        };

                                        work_item.work_list.push_back(tmp);
                                }

                                wq.enqueue(work_item);
                        }
                }

                LOG_INFO("multi-threaded-renderer information: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, total-work-items: {%d}, "
                         "work-queue-items: {%ld} (approx.)",
                         num_hw_threads, total_pixels_per_thread, total_work_items, wq.size_approx());

                return wq;
        }

        /// --------------------------------------------------------------------
        /// generate work-queue in hilber-curve order
        moodycamel::ConcurrentQueue<render_work_items> generate_hilbert_work_queue(uint32_t num_hw_threads,
                                                                                   camera const& C)
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

                auto const total_points = C.hsize() * C.vsize();
                auto const order        = find_largest_2pow(total_points);

                auto const total_pixels_per_thread = C.hsize() / num_hw_threads;
                auto const total_work_items        = (C.hsize() / total_pixels_per_thread) * (C.vsize());

                moodycamel::ConcurrentQueue<render_work_items> wq(total_work_items);

                for (uint32_t D = 0; D < order;) {
                        render_work_items work_item;
                        work_item.work_list.reserve(total_pixels_per_thread);

                        for (uint32_t i = 0; i < total_pixels_per_thread; i++, D++) {
                                uint32_t x = 0;
                                uint32_t y = 0;

                                d2xy(order, D, x, y);

                                render_work_item tmp{
                                        x,                     /// x-pixel
                                        y,                     /// y-pixel
                                        C.ray_for_pixel(x, y), /// ray-at-(x,y)
                                };

                                work_item.work_list.push_back(tmp);
                        }

                        wq.enqueue(work_item);
                }

                LOG_INFO("multi-threaded-renderer information: "
                         "total-threads: {%d}, pixels-per-thread: {%d}, total-work-items: {%d}, "
                         "work-queue-items: {%ld} (approx.)",
                         num_hw_threads, total_pixels_per_thread, total_work_items, wq.size_approx());

                return wq;
        }

} // namespace raytracer
