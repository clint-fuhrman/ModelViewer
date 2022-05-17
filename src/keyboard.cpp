#include <stdlib.h>

#include "display.hpp"
#include "object_loader.hpp"
#include "camera.hpp"
#include "keyboard.hpp"

namespace keyboard {

    bool is_key_pressed[256] = { false };

    bool is_increase_mode = true;

    void InitializeKeyboardInput() {
        glutKeyboardFunc(keyboard::HandleKeyPress);
        glutKeyboardUpFunc(keyboard::HandleKeyRelease);
    }

    void HandleKeyPress(unsigned char key, int x, int y) {
        // Escape key; terminate program
        if (key == 27) exit(0);

        // Camera translation
        if (key == 'w' || key == 'W') is_key_pressed['w'] = true;
        if (key == 's' || key == 'S') is_key_pressed['s'] = true;
        if (key == 'a' || key == 'A') is_key_pressed['a'] = true;
        if (key == 'd' || key == 'D') is_key_pressed['d'] = true;

        // Space; reset camera
        if (key == ' ')	camera::ResetCamera();

        // Color controls
        if (key == 'r' || key == 'R') is_key_pressed['r'] = true;
        if (key == 'g' || key == 'G') is_key_pressed['g'] = true;
        if (key == 'b' || key == 'B') is_key_pressed['b'] = true;

        // Clipping controls
        if (key == 'n' || key == 'N') is_key_pressed['n'] = true;
        if (key == 'f' || key == 'F') is_key_pressed['f'] = true;

        // Toggle increase/decrease mode for color and clipping controls
        if (key == 't') is_increase_mode = !is_increase_mode;

        // Toggle lighting modes
        if (key == 'l' || key == 'L')
            display::light_on == 2 ? display::light_on = 0 : display::light_on++;

        // Toggle smooth/flat shading
        if (key == 'p') display::smooth_shading = !display::smooth_shading;

        // Set polygon rendering mode
        if (key == '1') display::render_mode = SOLID;
        else if (key == '2') display::render_mode = WIREFRAME;
        else if (key == '3') display::render_mode = POINTS;
    }

    void HandleKeyRelease(unsigned char key, int x, int y) {
        if (key == 'w' || key == 'W') is_key_pressed['w'] = false;
        if (key == 's' || key == 'S') is_key_pressed['s'] = false;
        if (key == 'a' || key == 'A') is_key_pressed['a'] = false;
        if (key == 'd' || key == 'D') is_key_pressed['d'] = false;

        if (key == 'r' || key == 'R') is_key_pressed['r'] = false;
        if (key == 'g' || key == 'G') is_key_pressed['g'] = false;
        if (key == 'b' || key == 'B') is_key_pressed['b'] = false;

        if (key == 'n' || key == 'N') is_key_pressed['n'] = false;
        if (key == 'f' || key == 'F') is_key_pressed['f'] = false;
    }

}