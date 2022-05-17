#include <stdio.h>

#include "GL/freeglut.h"

#include "camera.hpp"
#include "display.hpp"
#include "mouse.hpp"

namespace mouse {

    // Used to control camera tilt
    bool is_left_pressed = false;
    bool is_right_pressed = false;

    void InitializeMouseInput() {
        // Hide cursor & center it within the window
        glutSetCursor(GLUT_CURSOR_NONE);
        glutWarpPointer(display::WINDOW_WIDTH / 2, display::WINDOW_HEIGHT / 2);

        glutMouseFunc(mouse::HandleMouseButton);
        glutPassiveMotionFunc(mouse::HandleMouseMove);
    }

    /*
     * Handles mouse click and scroll events.
     *
     * Left/right clicks tilt the camera (rotation around the n axis).
     * Scrolling zooms the camera (translation along the n axis).
     */
    void HandleMouseButton(int button, int state, int x, int y) {
        if (button == GLUT_LEFT_BUTTON) {
            is_left_pressed = (state != GLUT_UP);
        }

        if (button == GLUT_RIGHT_BUTTON) {
            is_right_pressed = (state != GLUT_UP);
        }

        if (state == GLUT_UP) return; // disregard GLUT_UP events for scrolls

        if (button == 3) {
            camera::TranslateCamera('n', false); // zoom in
        } else if (button == 4) {
            camera::TranslateCamera('n', true); // zoom out
        }
    }

    /*
     * Handles mouse motions, which rotate the camera left or right.
     */
    void HandleMouseMove(int x, int y) {
        int origin_x = display::WINDOW_WIDTH / 2;
        int origin_y = display::WINDOW_HEIGHT / 2;

        if ((x == origin_x) && (y == origin_y)) return;

        int delta_x = x - origin_x; // positive if right drag, negative if left
        int delta_y = origin_y - y; // positive if up drag, negative if down

        GLfloat v_increment = (delta_x > 1) ? -camera::ROTATION_SPEED : camera::ROTATION_SPEED;
        camera::RotateCamera('v', v_increment);

        GLfloat u_increment = (delta_y > 1) ? camera::ROTATION_SPEED : -camera::ROTATION_SPEED;
        camera::RotateCamera('u', u_increment);

        // Reset cursor to the center of the window
        glutWarpPointer(display::WINDOW_WIDTH / 2, display::WINDOW_HEIGHT / 2);
    }

}