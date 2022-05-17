#define GLM_ENABLE_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "camera.hpp"
#include "display.hpp"
#include "object_loader.hpp"
#include "keyboard.hpp"
#include "mouse.hpp"
#include "shader_loader.hpp"

namespace display {

    const int WINDOW_WIDTH = 500;
    const int WINDOW_HEIGHT = 500;
    const int LEFT_WINDOW_X = 250;
    const int LEFT_WINDOW_Y = 175;
    const int RIGHT_WINDOW_X = WINDOW_WIDTH + LEFT_WINDOW_X + 50;
    const int RIGHT_WINDOW_Y = 175;

    const GLfloat COLOR_CHANGE_SPEED = 0.01f;
    const GLfloat CLIP_CHANGE_SPEED = 0.025f;
    const double FRAMERATE = 60.0;

    const GLfloat MATERIAL_AMBIENT[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat MATERIAL_DIFFUSE[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    const GLfloat MATERIAL_SPECULAR[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    const GLfloat MATERIAL_SHININESS[] = { 50.0f };

    char model_path[100] = "models\\bunny.obj";

    std::vector<GLfloat> vertex_coordinates;
    std::vector<GLuint> face_vertices;
    std::vector<GLfloat> vertex_normals;

    // Bounding coordinates of the model
    GLfloat max_x = -10000, max_y = -10000, max_z = -10000;
    GLfloat min_x = 10000, min_y = 10000, min_z = 10000;
    GLfloat max_dimension = 0;

    // RGB light components
    GLfloat red = 0.5;
    GLfloat green = 0.5;
    GLfloat blue = 0.5;

    GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
    GLfloat half_vector[] = { 0.0f, 0.0f, 0.0f };

    unsigned light_on = ALL_ON;
    bool smooth_shading = true;

    // Default to solid polygon rendering
    char render_mode = SOLID;
    GLenum primitive_type = GL_TRIANGLES;

    int fixed_pipeline_window_id, custom_shaders_window_id;

    void InitializeFixedPipelineWindow() {
        glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        glutInitWindowPosition(LEFT_WINDOW_X, LEFT_WINDOW_Y);
        fixed_pipeline_window_id = glutCreateWindow("Fixed Function Pipeline");

        camera::ResetCamera();
        glutDisplayFunc(RenderWithFixedPipeline);
    }

    void InitializeCustomShadersWindow() {
        glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
        glutInitWindowPosition(RIGHT_WINDOW_X, RIGHT_WINDOW_Y);
        custom_shaders_window_id = glutCreateWindow("GLSL Custom Shaders");
        glewInit();

        shader_loader::InitBufferObject();
        shader_loader::SetShaders();

        glutDisplayFunc(RenderWithCustomShaders);
    }

    void RenderWithFixedPipeline() {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        GLfloat frustum_bound = max_dimension / 4;
        glFrustum(-frustum_bound, frustum_bound,
            -frustum_bound, frustum_bound,
            camera::near_clip, camera::far_clip);

        glMatrixMode(GL_MODELVIEW);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glLoadIdentity();

        gluLookAt(camera::camera[0], camera::camera[1], camera::camera[2],
            camera::target[0], camera::target[1], camera::target[2],
            camera::up[0], camera::up[1], camera::up[2]);

        glVertexPointer(3, GL_FLOAT, 3 * sizeof(GL_FLOAT), &vertex_coordinates[0]);
        glEnableClientState(GL_VERTEX_ARRAY);

        glNormalPointer(GL_FLOAT, 3 * sizeof(GL_FLOAT), &vertex_normals[0]);
        glEnableClientState(GL_NORMAL_ARRAY);

        glMaterialfv(GL_FRONT, GL_AMBIENT, MATERIAL_AMBIENT);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, MATERIAL_DIFFUSE);
        glMaterialfv(GL_FRONT, GL_SPECULAR, MATERIAL_SPECULAR);
        glMaterialfv(GL_FRONT, GL_SHININESS, MATERIAL_SHININESS);

        UpdatePolygonMode();
        UpdateFixedPipelineLighting();

        glColor3f(red, green, blue);
        glShadeModel(smooth_shading ? GL_SMOOTH : GL_FLAT);
        glDrawElements(primitive_type, face_vertices.size(), GL_UNSIGNED_INT, &face_vertices[0]);

        glFlush();
        glutSwapBuffers();
    }

    void UpdateFixedPipelineLighting() {
        if (light_on) {
            glEnable(GL_LIGHTING);

            GLfloat global_ambient[4] = { red, green, blue, 1.0 };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);

            if (light_on == ALL_ON) {
                glEnable(GL_LIGHT0);

                // All components of light change color together
                GLfloat light_ambient[] = { 0.2f * red, 0.2f * green, 0.2f * blue, 1.0 };
                GLfloat light_diffuse[] = { 0.8f * red, 0.8f * green, 0.8f * blue, 1.0 };
                GLfloat light_specular[] = { 0.5f * red, 0.5f * green, 0.5f * blue, 1.0 };

                glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
                glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
                glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
                glLightfv(GL_LIGHT0, GL_POSITION, light_position);
            }
        } else {
            // Turn off all lights
            GLfloat global_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
            glDisable(GL_LIGHT0);
        }
    }

    void RenderWithCustomShaders() {
        glUseProgram(shader_loader::program_id);

        glValidateProgram(shader_loader::program_id);
        GLint validate = 0;
        glGetProgramiv(shader_loader::program_id, GL_VALIDATE_STATUS, &validate);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

        camera::CalculateProjectionMatrix();
        camera::CalculateModelViewMatrix();

        GLint model_view_matrix_location = glGetUniformLocation(shader_loader::program_id, "modelViewMatrix");
        GLint projection_matrix_location = glGetUniformLocation(shader_loader::program_id, "projectionMatrix");

        glUniformMatrix4fv(model_view_matrix_location, 1, GL_FALSE, shader_loader::model_view_matrix);
        glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, shader_loader::projection_matrix);

        UpdatePolygonMode();
        UpdateShadingUniform();
        UpdateLightOnUniform();
        UpdateHalfVector();

        glBindVertexArray(shader_loader::vao);
        glDrawElements(GL_TRIANGLES, face_vertices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glutSwapBuffers();
    }

    /*
     * Handles input that may be executed simultaneously (such as translation in multiple directions),
     * and rerenders the model.
     */
    void Timer(int t) {
        // Translation along camera axes
        if (keyboard::is_key_pressed['a']) camera::TranslateCamera('u', false);	// left (-u)
        if (keyboard::is_key_pressed['d']) camera::TranslateCamera('u', true);	// right (+u)
        if (keyboard::is_key_pressed['w']) camera::TranslateCamera('v', true);	// up (+v)
        if (keyboard::is_key_pressed['s']) camera::TranslateCamera('v', false);	// down (-v)

        // Camera tilt
        if (mouse::is_left_pressed) camera::RotateCamera('n', camera::ROTATION_SPEED);
        else if (mouse::is_right_pressed) camera::RotateCamera('n', -camera::ROTATION_SPEED);

        // Color controls
        if (keyboard::is_key_pressed['r'] && keyboard::is_increase_mode) ColorUp(&red);
        if (keyboard::is_key_pressed['g'] && keyboard::is_increase_mode) ColorUp(&green);
        if (keyboard::is_key_pressed['b'] && keyboard::is_increase_mode) ColorUp(&blue);

        if (keyboard::is_key_pressed['r'] && !keyboard::is_increase_mode) ColorDown(&red);
        if (keyboard::is_key_pressed['g'] && !keyboard::is_increase_mode) ColorDown(&green);
        if (keyboard::is_key_pressed['b'] && !keyboard::is_increase_mode) ColorDown(&blue);

        // Clipping controls
        if (keyboard::is_key_pressed['n'] && keyboard::is_increase_mode) camera::IncreaseNearClip();
        if (keyboard::is_key_pressed['f'] && keyboard::is_increase_mode) camera::far_clip += CLIP_CHANGE_SPEED;

        if (keyboard::is_key_pressed['n'] && !keyboard::is_increase_mode) camera::near_clip -= CLIP_CHANGE_SPEED;
        if (keyboard::is_key_pressed['f'] && !keyboard::is_increase_mode) camera::DecreaseFarClip();

        glutSetWindow(fixed_pipeline_window_id);
        glutPostRedisplay();

        glutSetWindow(custom_shaders_window_id);
        glutPostRedisplay();

        glutTimerFunc((unsigned int)(1000.0 / FRAMERATE), Timer, 0);
    }

    void UpdatePolygonMode() {
        switch (render_mode) {
            case SOLID:
                primitive_type = GL_TRIANGLES;
                glPolygonMode(GL_FRONT, GL_FILL);
                break;
            case WIREFRAME:
                primitive_type = GL_TRIANGLES;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                break;
            case POINTS:
                primitive_type = GL_POINTS;
                glPolygonMode(GL_FRONT, GL_POINT);
                break;
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }

    /*
     * Increments specified color, enforcing 1.0 as the maximum value.
     */
    void ColorUp(GLfloat *color) {
        *color = (*color >= 1.0) ? 1.0 : *color + COLOR_CHANGE_SPEED;
        UpdateColorUniform();
    }

    /*
     * Decrements specified color, enforcing 0.0 as the minimum value.
     */
    void ColorDown(GLfloat *color) {
        *color = (*color <= 0.0) ? 0.0 : *color - COLOR_CHANGE_SPEED;
        UpdateColorUniform();
    }

    /*
     * Updates the "currentColor" uniform of fragmentshader.
     */
    void UpdateColorUniform() {
        GLint current_color_location = glGetUniformLocation(shader_loader::program_id, "currentColor");
        glUniform4f(current_color_location, red, green, blue, 1.0);
    }

    /*
     * Updates the "smoothShading" uniform of fragmentshader.
     */
    void UpdateShadingUniform() {
        GLint smooth_shading_location = glGetUniformLocation(shader_loader::program_id, "smoothShading");
        glUniform1i(smooth_shading_location, smooth_shading);
    }

    /*
     * Updates the "lightOn" uniform of fragmentshader.
     */
    void UpdateLightOnUniform() {
        GLint light_on_location = glGetUniformLocation(shader_loader::program_id, "lightOn");
        glUniform1i(light_on_location, light_on);
    }

    /*
     * Updates the "halfVector" uniform of fragmentshader.
     */
    void UpdateHalfVector() {
        glm::vec3 L(light_position[0], light_position[1], light_position[2]);
        glm::vec3 V(camera::target[0] - camera::camera[0],
            camera::target[1] - camera::camera[1],
            camera::target[2] - camera::camera[2]);

        L = normalize(L);
        V = -normalize(V);

        glm::vec3 H = glm::normalize(V + L);
        half_vector[0] = H[0]; half_vector[1] = H[1]; half_vector[2] = H[2];

        GLint half_vector_location = glGetUniformLocation(shader_loader::program_id, "halfVector");
        glUniform3fv(half_vector_location, 1, half_vector);
    }

    void ReinitializeShaders() {
        shader_loader::InitBufferObject();
    }

}
