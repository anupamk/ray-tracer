#pragma once


/// c++ includes
#include <stddef.h>

/// our includes
#include "io/canvas.hpp"          // for canvas
#include "patterns/material.hpp"  // for material

namespace raytracer
{
        /// --------------------------------------------------------------------
        /// forward declarations
        class color;
        
        /*
         * this defines a 'catalogue', if you will, of materials that can be
         * used in various settings, rather than discovering these over and over
         * again.
         **/

        /// --------------------------------------------------------------------
        /// "glassy" materials
        material create_material_transparent_glass();
        material create_material_colored_glass(color const&);

        /// --------------------------------------------------------------------
        /// "matte" material
        material create_material_matte(color const&);

        /// --------------------------------------------------------------------
        /// generate a noisy texture of a specific size.
        canvas generate_noisy_texture(size_t canvas_xpixels,    /// x-dim
                                      size_t canvas_ypixels,    /// y-dim
                                      color const& start_color, /// start-color
                                      color const& end_color);  /// end-color
} // namespace raytracer
