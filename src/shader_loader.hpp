#pragma once

#ifndef SHADERLOADER_H
#define SHADERLOADER_H

namespace shader_loader {

    extern GLuint vertex_shader_id, fragment_shader_id, program_id;
    extern GLuint vbo_coordinates, vbo_normals, vao, ebo;
    extern GLfloat projection_matrix[16], model_view_matrix[16];

    void ReadShaderFile(const GLchar* shaderPath, std::string& shaderCode);
    void SetShaders();
    void InitBufferObject(void);

}

#endif
