#pragma once

/// c++ includes
#include <cstdint>
#include <string>

/// our includes
#include "utils/utils.hpp"

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
                /// when true, enable antialiased rendering of a scene.
                ///
                /// when 'antialias_enabled == true', adaptively compute
                /// a pixel's color.
                ///
                /// the 'AA_COLOR_DIFF_THRESHOLD' constant is related.
                bool antialias_enabled_ = false;

                /// ------------------------------------------------------------
                /// rendering order
                rendering_style render_style_ = rendering_style::RENDERING_STYLE_SCANLINE;

            public:
                /// ------------------------------------------------------------
                /// see camera::adaptively_color_a_pixel_at(...) to get some
                /// more idea about what + how this is used.
                ///
                /// briefly though it defines threshold by which color
                /// differ between corner & center of a pixel triggering
                /// a recursive sub-division of the pixel to compute color
                /// sample there.
                static constexpr double AA_COLOR_DIFF_THRESHOLD = 0.05;

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
                bool antialias() const;

                /// ------------------------------------------------------------
                /// configure various properties
                config_render_params&& online(bool);
                config_render_params&& hw_threads(uint8_t);
                config_render_params&& benchmark(bool);
                config_render_params&& benchmark_rounds(uint32_t);
                config_render_params&& benchmark_discard_initial(uint32_t);
                config_render_params&& render_style(rendering_style const&);
                config_render_params&& antialias(bool);

            private:
                /// ------------------------------------------------------------
                /// validate benchmarking parameters, and determine their
                /// overall effect on benchmarking.
                ///
                /// see below for more details.
                ///
                /// for benchmarking a rendering process, the user can has
                /// following two knobs:
                ///
                ///    - 'benchmark_rounds_': how many iterations are
                ///    performed,
                ///
                ///    - 'benchmark_num_discards_': number of initial benchmark
                ///    rounds to discard
                ///
                /// the 'benchmark_num_discards_' drops initial 'n' benchmark
                /// results, to discount for cache etc. effects.
                ///
                /// now, these parameters are independently controlled. however
                /// they can have a cumulative effect, for example, if
                /// 'benchmark_num_discards_' ≥ 'benchmark_rounds_' then
                /// benchmark should be disabled.
                ///
                /// this function performs these checks and balances
                void validate_benchmark_state_();
        };

} // namespace raytracer
