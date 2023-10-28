#pragma once

/// c++ includes
#include <cstdint>
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
        /// rendering a canvas can proceed in one of the following 'styles'
        enum class rendering_style {
                RENDERING_STYLE_INVALID  = 0,
                RENDERING_STYLE_SCANLINE = 1,
                RENDERING_STYLE_HILBERT  = 2,
                RENDERING_STYLE_TILE     = 3,
        };

        /// --------------------------------------------------------------------
        /// stringified representation of the rendering style
        std::string stringify_rendering_style(rendering_style const& S);

        /// --------------------------------------------------------------------
        /// configure various rendering parameters
        class config_render_params final
        {
            private:
                /// ------------------------------------------------------------
                /// should the rendering progress be show while rendering ?
                ///
                /// when enabled, this seems to be adding approximately 7..10
                /// second delay. however, it ends up producing a cool looking
                /// effect. so yeah, totally worth it.
                ///
                /// the 'cool looking effect' is especially more pronounced when
                /// the 'rendering_style::RENDERING_STYLE_HILBERT' is used.
                bool online_ = false;

                /// ------------------------------------------------------------
                /// how many h/w threads are we using ?
                uint32_t hw_threads_ = max_cores();

                /// ------------------------------------------------------------
                /// should the rendering be benchmarked ? if yes, then following
                /// parameters are in effect:
                ///    - benchmark_rounds_          == number-of-iterations
                ///    - benchmark_discard_initial_ == discard-initial-results ?
                ///    - benchmark_num_discards_    == number-of-discards
                bool benchmark_                  = false;
                uint32_t benchmark_rounds_       = 0;
                bool benchmark_discard_initial_  = false;
                uint32_t benchmark_num_discards_ = 0;

                /// ------------------------------------------------------------
                /// rendering order
                rendering_style render_style_ = rendering_style::RENDERING_STYLE_SCANLINE;

            public:
                /// ------------------------------------------------------------
                /// create default instance
                config_render_params();

                /// ------------------------------------------------------------
                /// stringify the rendering parameters
                std::string stringify() const;

                /// ------------------------------------------------------------
                /// get various properties
                bool online() const;
                uint8_t hw_threads() const;
                bool benchmark() const;
                uint32_t benchmark_rounds() const;
                bool benchmark_discard_initial() const;
                uint32_t benchmark_num_discard_initial() const;
                rendering_style render_style() const;

                /// ------------------------------------------------------------
                /// configure various properties
                config_render_params&& online(bool);
                config_render_params&& hw_threads(uint8_t);
                config_render_params&& benchmark(bool);
                config_render_params&& benchmark_rounds(uint32_t);
                config_render_params&& benchmark_discard_initial(uint32_t);
                config_render_params&& render_style(rendering_style const&);
        };

} // namespace raytracer
