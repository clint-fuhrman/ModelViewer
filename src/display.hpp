#pragma once

#ifndef DISPLAY_H
#define DISPLAY_H

#include <vector>

#include "GL/freeglut.h"

namespace display {

    // Polygon rendering modes
    #define SOLID		1
    #define WIREFRAME	2
    #define POINTS		3

    // Lighting modes
    #define OFF			0
    #define GLOBAL_ON	1
    #define ALL_ON		2

    extern const int WINDOW_WIDTH;
    extern const int WINDOW_HEIGHT;
    extern const int LEFT_WINDOW_X;
    extern const int LEFT_WINDOW_Y;
    extern const int RIGHT_WINDOW_X;
    extern const int RIGHT_WINDOW_Y;

    extern const GLfloat COLOR_CHANGE_SPEED;
    extern const GLfloat CLIP_CHANGE_SPEED;
    extern const double FRAMERATE;

    extern const GLfloat MATERIAL_AMBIENT[];
    extern const GLfloat MATERIAL_DIFFUSE[];
    extern const GLfloat MATERIAL_SPECULAR[];
    extern const GLfloat MATERIAL_SHININESS[];

    extern char model_path[100];

    extern std::vector<GLfloat> vertex_coordinates;
    extern std::vector<GLuint> face_vertices;
    extern std::vector<GLfloat> vertex_normals;

    extern GLfloat max_x, max_y, max_z;
    extern GLfloat min_x, min_y, min_z;
    extern GLfloat max_dimension;

    extern GLfloat red, green, blue;

    extern GLfloat light_position[];
    extern GLfloat half_vector[];

    extern unsigned light_on;
    extern bool smooth_shading;

    extern char render_mode;
    extern GLenum primitive_type;

    extern int fixed_pipeline_window_id, custom_shaders_window_id;

    void InitializeFixedPipelineWindow();
    void InitializeCustomShadersWindow();
    void RenderWithFixedPipeline();
    void UpdateFixedPipelineLighting();
    void RenderWithCustomShaders();
    void Timer(int t);
    void UpdatePolygonMode();
    void ColorUp(GLfloat *color);
    void ColorDown(GLfloat *color);
    void UpdateColorUniform();
    void UpdateShadingUniform();
    void UpdateLightOnUniform();
    void UpdateHalfVector();
    void ReinitializeShaders();

}

#endif