#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

namespace camera {

    extern const GLfloat TRANSLATION_SPEED;
    extern const GLfloat ROTATION_SPEED;

    extern glm::vec3 camera;
    extern glm::vec3 target;
    extern glm::vec3 up;

    extern glm::vec3 n_axis;
    extern glm::vec3 u_axis;
    extern glm::vec3 v_axis;

    extern GLfloat near_clip, far_clip;

    void ResetCamera();
    void UpdateCameraAxes();
    void TranslateCamera(char axis_id, bool is_positive);
    void RotateCamera(char axis_id, float angle);
    void CalculateModelViewMatrix();
    void CalculateProjectionMatrix();
    void IncreaseNearClip();
    void DecreaseFarClip();

}

#endif