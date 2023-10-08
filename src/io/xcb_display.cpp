/*
 * implement the raytracer xcb_display for online rendering of images.
 **/

#include "io/xcb_display.hpp"

/// system includes
#include <memory>
#include <xcb/xproto.h> /// xcb_create_window

/// our inclues
#include "common/include/assert_utils.h"
#include "common/include/logging.h"

namespace raytracer
{

        /// --------------------------------------------------------------------
        /// create a pixel of a specific color
        xcb_pixel_t::xcb_pixel_t(uint32_t x, uint32_t y, uint32_t color)
            : x(x)
            , y(y)
            , color(color)
        {
        }

        /// --------------------------------------------------------------------
        /// create a display instance
        std::unique_ptr<xcb_display> xcb_display::create_display(uint32_t x_dim, uint32_t y_dim)
        {
                auto new_xcb_display = new xcb_display(x_dim, y_dim);
                return std::unique_ptr<xcb_display>(new_xcb_display);
        }

        /// --------------------------------------------------------------------
        /// destroy a xcb_display instance
        xcb_display::~xcb_display()
        {
                xcb_disconnect(connection_);
        }

        /// --------------------------------------------------------------------
        /// plot a pixel
        void xcb_display::plot_pixel(uint32_t x, uint32_t y, uint32_t color)
        {
                xcb_gcontext_t const fg_graphics_ctxt = xcb_generate_id(connection_);
                uint32_t const prop_names             = (XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES);
                uint32_t const prop_values[2]         = {color, 0};

                xcb_create_gc(connection_, fg_graphics_ctxt, win_id_, prop_names, prop_values);

                /// ------------------------------------------------------------
                /// plot, and show it
                xcb_point_t tmp_pt = {
                        .x = static_cast<int16_t>(x),
                        .y = static_cast<int16_t>(y),
                };

                xcb_poly_point(connection_, XCB_COORD_MODE_ORIGIN, win_id_, fg_graphics_ctxt,
                               1,        /// number-of-points
                               &tmp_pt); /// point-itself

                xcb_flush(connection_);
        }

        /// --------------------------------------------------------------------
        /// plot a pixel
        void xcb_display::plot_pixel(xcb_pixel_t const& pixel)
        {
                plot_pixel(pixel.x, pixel.y, pixel.color);
        }

        /// --------------------------------------------------------------------
        /// private methods only from this point onwards ↓
        /// --------------------------------------------------------------------

        /// --------------------------------------------------------------------
        /// create an instance of xcb_display
        xcb_display::xcb_display(uint32_t x_dim, uint32_t y_dim)
            : win_x_dim_(x_dim)
            , win_y_dim_(y_dim)
            , connection_(init_xcb_connection())
            , screen_(init_x11_screen())
            , win_id_(create_window())
        {
        }

        /// --------------------------------------------------------------------
        /// initialize an xcb connection
        xcb_connection_t* xcb_display::init_xcb_connection() const
        {
                xcb_connection_t* new_connection = xcb_connect(NULL,  /// which-display
                                                               NULL); /// screen-0

                ASSERT(new_connection != nullptr);

                /// ------------------------------------------------------------
                /// ensure that connection is ok.
                if (xcb_connection_has_error(new_connection)) {
                        xcb_disconnect(new_connection);

                        new_connection = nullptr;
                        ASSERT_FAIL("Error opening display !");
                }

                return new_connection;
        }

        /// --------------------------------------------------------------------
        /// initialize x11 screen
        xcb_screen_t* xcb_display::init_x11_screen() const
        {
                auto xcb_screen = xcb_setup_roots_iterator(xcb_get_setup(connection_)).data;
                ASSERT(xcb_screen != nullptr);

                return xcb_screen;
        }

        /// --------------------------------------------------------------------
        /// create a window
        xcb_window_t xcb_display::create_window() const
        {
                auto const win_id = xcb_generate_id(connection_);

                /// ------------------------------------------------------------
                /// property names and their corresponding values
                uint32_t const prop_names = (XCB_CW_BACK_PIXEL | /// window-bg-color
                                             XCB_CW_EVENT_MASK); /// intersting-events

                uint32_t const prop_values[2] = {
                        screen_->black_pixel,
                        XCB_EVENT_MASK_EXPOSURE,
                };

                /// ------------------------------------------------------------
                /// create a unmapped (not-displayed-yet) window as a child of
                /// the root window
                xcb_create_window(connection_,                   /// the-connection
                                  XCB_COPY_FROM_PARENT,          /// same-depth-as-parent
                                  win_id,                        /// window-id
                                  screen_->root,                 /// parent-window
                                  0, 0,                          /// x, y coordinate of the window
                                  win_x_dim_,                    /// window-width
                                  win_y_dim_,                    /// window-height
                                  win_border_width_,             /// border-width
                                  XCB_WINDOW_CLASS_INPUT_OUTPUT, /// input-output
                                  screen_->root_visual,          /// from-parent
                                  prop_names,                    /// properties
                                  prop_values);                  /// values

                /// ------------------------------------------------------------
                /// map (display) the window
                xcb_map_window(connection_, win_id);
                xcb_flush(connection_);

                return win_id;
        }

} // namespace raytracer
