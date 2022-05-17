#pragma once

#ifndef MOUSE_H
#define MOUSE_H

namespace mouse {

    extern bool is_left_pressed;
    extern bool is_right_pressed;

    void InitializeMouseInput();
    void HandleMouseButton(int button, int state, int x, int y);
    void HandleMouseMove(int x, int y);

}

#endif