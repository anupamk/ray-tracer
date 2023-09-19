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
#include "io/multi_threaded_work_generator.hpp"
#include "io/raytracer_renderer.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{

        /// --------------------------------------------------------------------
        /// this is the function that gets invoked for rendering a bunch of work
        /// items.
        static void coloring_worker(int thread_id,
                                    moodycamel::ConcurrentQueue<render_work_items>&, /// queue-of-work
                                    world const&,                                    /// scene-details
                                    canvas&);                                        /// canvas-details

        /*
         * this implements a multi threaded rendering of scene in a world 'W'
         * that is looked at by a camera 'C' with the generated image stored in
         * a file 'dst_fname'
         **/
        canvas multi_threaded_renderer(uint32_t num_hw_threads, world W, camera C)
        {
                PROFILE_SCOPE;

                canvas rendered_canvas = canvas::create_ascii(C.hsize(), C.vsize());
                auto work_queue        = generate_scanline_work_queue(num_hw_threads, C);

                /// ------------------------------------------------------------
                /// start the coloring threads
                std::vector<std::thread> rendering_threads(num_hw_threads);

                for (auto i = 0; i < num_hw_threads; i++) {
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
        static void
        coloring_worker(int thread_id,
                        moodycamel::ConcurrentQueue<render_work_items>& work_queue, /// queue-of-work
                        world const& W,                                             /// scene-details
                        canvas& dst_canvas)                                         /// canvas-details
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
