/*
 * implement various methods for the config_render_params class
 **/

#include "io/render_params.hpp"

/// c++ includes
#include <cstdint>
#include <ostream>
#include <sstream>
#include <string>

/// 3rd-party libraries
#include "concurrentqueue/concurrentqueue.h"

/// our includes
#include "io/canvas.hpp"
#include "io/world.hpp"
#include "primitives/matrix.hpp"
#include "primitives/ray.hpp"

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// create an empty instance, defaults are already initialized
        config_render_params::config_render_params()
        {
        }

        bool config_render_params::online() const
        {
                return online_;
        }

        uint8_t config_render_params::hw_threads() const
        {
                return hw_threads_;
        }

        bool config_render_params::benchmark() const
        {
                return benchmark_;
        }

        uint32_t config_render_params::benchmark_rounds() const
        {
                return benchmark_rounds_;
        }

        bool config_render_params::benchmark_discard_initial() const
        {
                return benchmark_discard_initial_;
        }

        uint32_t config_render_params::benchmark_num_discard_initial() const
        {
                return benchmark_num_discards_;
        }

        rendering_style config_render_params::render_style() const
        {
                return render_style_;
        }

        /// --------------------------------------------------------------------
        /// show progress of rendering as pixels are colored ?
        config_render_params&& config_render_params::online(bool val)
        {
                online_ = val;
                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// number of h/w threads used for rendering
        config_render_params&& config_render_params::hw_threads(uint8_t val)
        {
                hw_threads_ = val;
                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// should we benchmark the whole process ?
        config_render_params&& config_render_params::benchmark(bool val)
        {
                benchmark_ = val;
                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// how many samples / iterations for benchmark ?
        config_render_params&& config_render_params::benchmark_rounds(uint32_t val)
        {
                benchmark_rounds_ = val;
                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// how many samples to discard ?
        config_render_params&& config_render_params::benchmark_discard_initial(uint32_t val)
        {
                benchmark_discard_initial_ = (val > 0) ? true : false;
                benchmark_num_discards_    = val;

                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// how are we scanning across the whole scene ?
        config_render_params&& config_render_params::render_style(rendering_style const& val)
        {
                render_style_ = val;
                return std::move(*this);
        }

        /// --------------------------------------------------------------------
        /// stringified representation of rendering parameters
        std::string config_render_params::stringify() const
        {
                std::stringstream ss("");

                ss << "{";

                ss << "show-as-we-go: '" << str_boolean(this->online_) << "', "
                   << "hw-threads: '" << this->hw_threads_ << "', "
                   << "rendering-style: '" << stringify_rendering_style(render_style_) << "'";

                if (this->benchmark_) {
                        ss << ", "
                           << "benchmark: '" << str_boolean(this->benchmark_) << "', "
                           << "benchmark-iterations: '" << this->benchmark_rounds_ << "'";

                        if (this->benchmark_discard_initial_) {
                                ss << ", "
                                   << "benchmark-discard-initial: '"
                                   << str_boolean(this->benchmark_discard_initial_) << "', "
                                   << "benchmark-discards: '" << this->benchmark_num_discards_ << "'";
                        }
                }

                ss << "}";

                return ss.str();
        }

        /// --------------------------------------------------------------------
        /// stringified representation of rendering style
        std::string stringify_rendering_style(rendering_style const& S)
        {
                switch (S) {
                case rendering_style::RENDERING_STYLE_SCANLINE:
                        return "RENDERING_STYLE_SCANLINE";

                case rendering_style::RENDERING_STYLE_HILBERT:
                        return "RENDERING_STYLE_HILBERT";

                case rendering_style::RENDERING_STYLE_TILE:
                        return "RENDERING_STYLE_TILE";

                case rendering_style::RENDERING_STYLE_INVALID:
                default:
                        break;
                }

                return "RENDERING_STYLE_INVALID";
        }

} // namespace raytracer
