#define GLM_ENABLE_EXPERIMENTAL

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/rotate_vector.hpp"

#include "camera.hpp"
#include "display.hpp"
#include "shader_loader.hpp"

namespace camera {

    const GLfloat TRANSLATION_SPEED = 0.05f;
    const GLfloat ROTATION_SPEED = 0.005f;

    // Camera attributes for gluLookAt()
    glm::vec3 camera;
    glm::vec3 target;
    glm::vec3 up;

    // Camera axes
    glm::vec3 n_axis;
    glm::vec3 u_axis;
    glm::vec3 v_axis;

    GLfloat near_clip, far_clip;

    /*
     * Resets the camera such that the model is fully visible and centered in the window.
     */
    void ResetCamera() {
        // Align camera eye with x and y midpoints of the model
        camera[0] = (display::max_x + display::min_x) / 2;
        camera[1] = (display::max_y + display::min_y) / 2;

        // Place the camera some ways away from nearest z coordinate
        camera[2] = abs(display::max_z - display::min_z) * 2 + display::max_z;

        // Point camera towards 3D midpoint of the model
        target[0] = camera[0];
        target[1] = camera[1];
        target[2] = (display::max_z + display::min_z) / 2;

        // Initialize up vector to global y axis
        up[0] = 0.0f;
        up[1] = 1.0f;
        up[2] = 0.0f;

        near_clip = (camera[2] - display::max_z) / 2; // halfway between camera and model
        far_clip = (camera[2] - display::min_z) * 1.5f; // include whole model, with buffer
    }

    /*
     * Updates the camera axes based on camera/target positions and up axis.
     */
    void UpdateCameraAxes() {
        n_axis[0] = camera[0] - target[0];
        n_axis[1] = camera[1] - target[1];
        n_axis[2] = camera[2] - target[2];
        n_axis = glm::normalize(n_axis);

        u_axis = glm::cross(up, n_axis);
        v_axis = glm::cross(n_axis, u_axis);
    }

    /*
     * Translates the camera along its u, v, or n axis.
     */
    void TranslateCamera(char axis_id, bool is_positive) {
        UpdateCameraAxes();

        static glm::vec3 offset = { 0, 0, 0 };

        glm::vec3 axis;
        switch (axis_id) {
            case 'u':
                axis = u_axis;
                break;
            case 'v':
                axis = v_axis;
                break;
            case 'n':
                axis = n_axis;
                break;
            default:
                return;
        }

        offset = axis * TRANSLATION_SPEED;
        if (is_positive) {
            camera += offset;
            target += offset;
        } else {
            camera -= offset;
            target -= offset;
        }
    }

    /*
     * Rotates the camera by the given angle around its u, v, or n axis.
     */
    void RotateCamera(char axis_id, float angle) {
        UpdateCameraAxes();

        static glm::vec3 line_of_sight;
        line_of_sight[0] = target[0] - camera[0];
        line_of_sight[1] = target[1] - camera[1];
        line_of_sight[2] = target[2] - camera[2];

        GLfloat length = glm::length(line_of_sight);

        switch (axis_id) {
            case 'u':
                // look up/down
                line_of_sight = glm::rotate(line_of_sight, angle, u_axis);
                line_of_sight *= (length / glm::length(line_of_sight));
                target = line_of_sight + camera;
                up = glm::rotate(up, angle, u_axis);
                up = glm::normalize(up);
                break;
            case 'v':
                // look right/left
                line_of_sight = glm::rotate(line_of_sight, angle, v_axis);
                line_of_sight *= (length / glm::length(line_of_sight));
                target = line_of_sight + camera;
                break;
            case 'n':
                // tilt CW/CCW
                up = glm::rotate(up, angle, n_axis);
                up = glm::normalize(up);
                break;
            default:
                return;
        }
    }

    /*
     * Calculates the model view matrix for the vertex shader.
     */
    void CalculateModelViewMatrix() {
        UpdateCameraAxes();

        glm::vec3 origin(camera[0], camera[1], camera[2]);
        origin = -1.0F * origin;

        glm::vec3 translate;
        translate[0] = dot(origin, u_axis);
        translate[1] = dot(origin, v_axis);
        translate[2] = dot(origin, n_axis);

        GLfloat *p = shader_loader::model_view_matrix;
        *p++ = u_axis[0];       *p++ = v_axis[0];       *p++ = n_axis[0];       *p++ = 0.0f;
        *p++ = u_axis[1];       *p++ = v_axis[1];       *p++ = n_axis[1];       *p++ = 0.0f;
        *p++ = u_axis[2];       *p++ = v_axis[2];       *p++ = n_axis[2];       *p++ = 0.0f;
        *p++ = translate[0];    *p++ = translate[1];    *p++ = translate[2];    *p++ = 1.0f;
    }

    /*
     * Calculates the projection matrix for the vertex shader.
     */
    void CalculateProjectionMatrix() {
        GLfloat l = -display::max_dimension / 4;
        GLfloat r = display::max_dimension / 4;
        GLfloat b = -display::max_dimension / 4;
        GLfloat t = display::max_dimension / 4;
        GLfloat n = near_clip;
        GLfloat f = far_clip;

        GLfloat *q = shader_loader::projection_matrix;
        *q++ = 2 * n / (r - l);     *q++ = 0.0f;                *q++ = 0.0f;                    *q++ = 0.0f;
        *q++ = 0.0f;                *q++ = 2 * n / (t - b);     *q++ = 0.0f;                    *q++ = 0.0f;
        *q++ = (r + l) / (r - l);   *q++ = (t + b) / (t - b);   *q++ = -(f + n) / (f - n);      *q++ = -1.0f;
        *q++ = 0.0f;                *q++ = 0.0f;                *q++ = -2 * f * n / (f - n);    *q++ = 0.0f;
    }

    /*
     * Increments near clip, enforcing (near_clip <= far_clip).
     */
    void IncreaseNearClip() {
        GLfloat new_near_clip = near_clip + display::CLIP_CHANGE_SPEED;
        near_clip = (new_near_clip >= far_clip) ? far_clip : new_near_clip;
    }

    /*
     * Decrements far clip, enforcing (far_clip >= near_clip).
     */
    void DecreaseFarClip() {
        GLfloat new_far_clip = far_clip - display::CLIP_CHANGE_SPEED;
        far_clip = (new_far_clip <= near_clip) ? near_clip : new_far_clip;
    }

}