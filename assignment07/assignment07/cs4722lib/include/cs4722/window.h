//
// Created by Ben on 11/8/2020.
//
#pragma once

#include "GLFW/glfw3.h"

namespace cs4722 {
    /**
     * \brief
     * Create a window, size it, and position it on the primary monitor.
     * Set the window context to be the default context.
     *
     * The proportion of the screen used is based on the smaller dimension of screen width or screen height.
     * Usually, the full screen is not available, just a somewhat smaller working area.
     * In windows, that seems to just leave out the task bar.
     *
     * There is no check made that either the `screen_ratio` or the `aspect_ratio` is reasonable.
     *
     * @param title  Title to display on the window
     * @param screen_ratio Proportion of the available screen to use
     * @param aspect_ratio Ratio of the window width divided by the height
     * @return Pointer to the window created.
     */
    GLFWwindow *setup_window(const char *title, double screen_ratio, double aspect_ratio);

    /**
      * \brief
      * Create a window, size it, and position it on the primary monitor.
      * Set the window context to be the default context.
      *
      *  Use about 90% of the screen width and height.
      *  The window aspect ratio will be 16 / 9
      *
      * @param title  Title to display on the window
      * @return Pointer to the window created.
      */
    inline GLFWwindow *setup_window_9_16_9(const char *title) {
        return setup_window(title, 0.9, 16.0 / 9.0);
    }

    /**
      * \brief
      * Create a window, size it, and position it on the primary monitor.
      * Set the window context to be the default context.
      *
      *  Use about 90% of the screen width and height.
      *  The window will be square
      *
      * @param title  Title to display on the window
      * @return Pointer to the window created.
      */
    inline GLFWwindow *setup_window_9_1_1(const char *title) {
        return setup_window(title, 0.9, 1.0);
    }



    /**
     * Get the aspect ratio of a window.
     *
     * @return  The width to height ratio of 'window'.
     */
    double get_aspect_ratio(GLFWwindow *window);

    /**
     * Set the OpenGL version as specified.
     * @param major Major version, such as 4
     * @param minor Minor version, such as 3
     */
    void set_opengl_version(int major, int minor);

    inline void set_opengl_43() {
        set_opengl_version(4,3);
    }

    inline void set_opengl_45() {
        set_opengl_version(4,5);
    }

}