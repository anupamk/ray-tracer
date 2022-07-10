/*
 * implement multi-threaded rendering of a scene.
 **/

/// system includes
#include <pthread.h>
#include <sched.h> /// for cpu_set_t

/// c++ includes
#include <functional>
#include <thread>
#include <vector>

/// 3rd-party libraries
#include "concurrentqueue/concurrentqueue.h"

/// our includes
#include "common/include/logging.h"
#include "ray.hpp"
#include "raytracer_renderer.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// a render-work item is a ray at a specific place on the canvas
        class render_work_item
        {
            public:
                uint32_t x;
                uint32_t y;
                ray_t r;
        };

        /// --------------------------------------------------------------------
        /// a bunch of render-work items is what is what gets handled by a
        /// single rendering thread
        class render_work_items
        {
            public:
                std::vector<render_work_item> work_list;
        };

        /// --------------------------------------------------------------------
        /// convenience mostly
        namespace CQ = moodycamel;

        /// --------------------------------------------------------------------
        /// file specific declarations

        /// this is the function that gets invoked for rendering a bunch of work
        /// items.
        static void coloring_worker(int thread_id,
                                    CQ::ConcurrentQueue<render_work_items>&, /// queue-of-work
                                    world const&,                            /// scene-details
                                    canvas&);                                /// canvas-details

        /*
         * this implements a multi threaded rendering of scene in a world 'W'
         * that is looked at by a camera 'C' with the generated image stored in
         * a file 'dst_fname'
         **/
        canvas multi_threaded_renderer(world W, camera C)
        {
                PROFILE_SCOPE;

                canvas rendered_canvas = canvas::create_ascii(C.hsize(), C.vsize());

                /// --------------------------------------------------------------------
                /// concurrent queue contains multiple instances of render_work_items defined
                /// above.
                int const MAX_RENDERING_THREADS   = std::thread::hardware_concurrency();
                int const TOTAL_PIXELS_PER_THREAD = C.hsize() / MAX_RENDERING_THREADS;

                LOG_DEBUG("renderer information: total-threads:%d, pixels-per-thread:%d",
                          MAX_RENDERING_THREADS, TOTAL_PIXELS_PER_THREAD);

                CQ::ConcurrentQueue<render_work_items> work_queue(TOTAL_PIXELS_PER_THREAD * C.vsize());

                /// ------------------------------------------------------------
                /// generate work for the workers...
                for (uint32_t y = 0; y < C.vsize(); y++) {
                        for (uint32_t x = 0; x < C.hsize(); x += TOTAL_PIXELS_PER_THREAD) {
                                render_work_items work_item;
                                work_item.work_list.reserve(TOTAL_PIXELS_PER_THREAD);

                                for (int i = 0; i < TOTAL_PIXELS_PER_THREAD; i++) {
                                        render_work_item tmp{
                                                x + i,                    /// x-pixel
                                                y,                        /// y-pixel
                                                C.ray_for_pixel(x + i, y) /// ray-at-(x,y)
                                        };

                                        work_item.work_list.push_back(tmp);
                                }

                                work_queue.enqueue(work_item);
                        }
                }

                /// ------------------------------------------------------------
                /// start the coloring threads
                std::vector<std::thread> rendering_threads(MAX_RENDERING_THREADS);
                for (auto i = 0; i < MAX_RENDERING_THREADS; i++) {
                        rendering_threads[i] = std::thread(coloring_worker,            /// rendering-function
                                                           i,                          /// thread-id
                                                           std::ref(work_queue),       /// work-queue
                                                           W,                          /// the world
                                                           std::ref(rendered_canvas)); /// canvas

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

                return rendered_canvas;
        }

        /*
         * file specific functions from this point
         **/

        /*
         * this function is the workhorse for rendering a bunch of work-items
         * picked from a concurrent queue
         **/
        static void coloring_worker(int thread_id,
                                    CQ::ConcurrentQueue<render_work_items>& work_queue, /// queue-of-work
                                    world const& W,                                     /// scene-details
                                    canvas& dst_canvas)                                 /// canvas-details
        {
                bool all_done          = false;
                size_t pixels_rendered = 0;
                size_t jobs_completed  = 0;

                /// do some work
                do {
                        render_work_items rw;
                        if (work_queue.try_dequeue(rw)) {
                                for (auto work : rw.work_list) {
                                        auto r_color = W.color_at(work.r);
                                        dst_canvas.write_pixel(work.x, work.y, r_color);

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

} // namespace raytracer
