#pragma once

/*
 * a spartan implementation for 'online' rendering using x11/xcb primitives.
 **/

/// c++ includes
#include <memory>
#include <stdint.h>
#include <xcb/xcb.h>
#include <xcb/xproto.h>

namespace raytracer
{

        /*
         * use raw x11/xcb primitives for online rendering of images. this is
         * 'completely raw' i.e. use x11/xcb directly.
         *
         * hopefully should offer better performance when used from multiple
         * threads for 'online' rendering of images.
         *
         * for more information about xcb/x11 see
         *   https://xcb.freedesktop.org/
         **/
        class xcb_display final
        {
            private:
                /*
                 * x and y dimensions of the window. these are specified in
                 * 'pixels'.
                 **/
                uint32_t win_x_dim_ = {};
                uint32_t win_y_dim_ = {};

                /*
                 * width of the window-border
                 **/
                static constexpr uint32_t win_border_width_ = 5;

                /*
                 * xcb primitives
                 **/

                /*
                 * data that xcb needs to communicate with an X server instance.
                 **/
                xcb_connection_t* connection_ = nullptr;

                /*
                 * a screen that we are connected to.
                 **/
                xcb_screen_t* screen_ = nullptr;

                /*
                 * id of the window that will be created
                 **/
                xcb_window_t win_id_ = {};

            public:
                /// ------------------------------------------------------------
                /// create an instance of xcb-display
                static std::unique_ptr<xcb_display> create_display(uint32_t x_dim, uint32_t y_dim);

                /// ------------------------------------------------------------
                /// destroy an instance of xcb-display
                ~xcb_display();

                /*
                 * plot a point || pixel
                 **/
                void plot_pixel(uint32_t x, uint32_t y, uint32_t c);

            private:
                /* @brief
                 *     private constructor ∵ named-constructor idiom
                 **/
                xcb_display(uint32_t x_dim, uint32_t y_dim);

                /*
                 * initialize a xcb connection instance
                 **/
                xcb_connection_t* init_xcb_connection() const;

                /*
                 * initialize the first screen
                 **/
                xcb_screen_t* init_x11_screen() const;

                /*
                 * create a window
                 **/
                xcb_window_t create_window() const;
        };

} // namespace raytracer
