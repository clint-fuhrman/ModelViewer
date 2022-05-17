#pragma once

#ifndef KEYBOARD_H
#define KEYBOARD_H

namespace keyboard {

    extern bool is_key_pressed[256];
    extern bool is_increase_mode;

    void InitializeKeyboardInput();
    void HandleKeyPress(unsigned char key, int x, int y);
    void HandleKeyRelease(unsigned char key, int x, int y);

}

#endif
