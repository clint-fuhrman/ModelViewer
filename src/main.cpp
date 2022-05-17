#define GLM_ENABLE_EXPERIMENTAL

#include "GL/freeglut.h"

#include "display.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "object_loader.hpp"

void main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitContextFlags(GLUT_COMPATIBILITY_PROFILE);

    if (!object_loader::LoadObject(display::model_path)) {
        printf("Failed to load \"%s\"\n", display::model_path);
        exit(1);
    }

    display::InitializeFixedPipelineWindow();
    display::InitializeCustomShadersWindow();

    mouse::InitializeMouseInput();
    keyboard::InitializeKeyboardInput();

    glutTimerFunc((unsigned int)(1000.0 / display::FRAMERATE), display::Timer, 0);
    glutMainLoop();
}
