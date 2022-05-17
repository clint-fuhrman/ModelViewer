#include "GL/glew.h"
#include "GL/freeglut.h"

#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream> 
#include <sstream>

#include "camera.hpp"
#include "display.hpp"

namespace shader_loader {

    GLuint vertex_shader_id, fragment_shader_id, program_id;
    GLuint vbo_coordinates, vbo_normals, vao, ebo;
    GLfloat projection_matrix[16], model_view_matrix[16];

    /*
     * Reads the given shader file and stores it as a string at the given address.
     */
    void ReadShaderFile(const GLchar* shaderFilePath, std::string& shaderString) {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::badbit);

        try {
            shaderFile.open(shaderFilePath);
            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            shaderFile.close();
            shaderString = shaderStream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
    }

    /*
     * Creates a program with custom vertex and fragment shaders.
     */
    void SetShaders() {
        char *vs = NULL, *fs = NULL;

        vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
        fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

        // Get each shader as a string
        std::string vertex_shader_string;
        std::string fragment_shader_string;
        ReadShaderFile("src\\vertexshader.txt", vertex_shader_string);
        ReadShaderFile("src\\fragmentshader.txt", fragment_shader_string);
        const GLchar *vertex_shader_source = vertex_shader_string.c_str();
        const GLchar *fragment_shader_source = fragment_shader_string.c_str();
        glShaderSource(vertex_shader_id, 1, &vertex_shader_source, NULL);
        glShaderSource(fragment_shader_id, 1, &fragment_shader_source, NULL);

        // Compile the shaders and attach them to a new program
        glCompileShader(vertex_shader_id);
        glCompileShader(fragment_shader_id);
        program_id = glCreateProgram();
        glAttachShader(program_id, vertex_shader_id);
        glAttachShader(program_id, fragment_shader_id);
        glLinkProgram(program_id);
        glUseProgram(program_id);

        // Specify uniform locations
        GLint model_view_matrix_location = glGetUniformLocation(program_id, "modelViewMatrix");
        GLint projection_matrix_location = glGetUniformLocation(program_id, "projectionMatrix");
        GLint current_color_location = glGetUniformLocation(program_id, "currentColor");
        GLint smooth_shading_location = glGetUniformLocation(program_id, "smoothShading");
        GLint light_direction_location = glGetUniformLocation(program_id, "lightDirection");
        GLint light_on_location = glGetUniformLocation(program_id, "lightOn");
        GLint half_vector_location = glGetUniformLocation(program_id, "halfVector");

        glUniformMatrix4fv(model_view_matrix_location, 1, GL_FALSE, model_view_matrix);
        glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix);
        glUniform4f(current_color_location, display::red, display::green, display::blue, 1.0);
        glUniform1i(smooth_shading_location, display::smooth_shading);
        glUniform3fv(light_direction_location, 1, display::light_position);
        glUniform1i(light_on_location, display::light_on);
        glUniform3fv(half_vector_location, 1, display::half_vector);

        glDeleteShader(vertex_shader_id);
        glDeleteShader(fragment_shader_id);
    }

    /*
     * Initializes the VAO for the shader display function to use.
     */
    void InitBufferObject(void) {
        glGenBuffers(1, &vbo_coordinates);
        glGenBuffers(1, &vbo_normals);
        glGenBuffers(1, &ebo);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Buffer vertex coordinates
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_coordinates);
        glBufferData(GL_ARRAY_BUFFER, (display::vertex_coordinates).size() * sizeof(GLfloat),
            &(display::vertex_coordinates[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        // Buffer vertex normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_normals);
        glBufferData(GL_ARRAY_BUFFER, (display::vertex_normals).size() * sizeof(GLfloat),
            &(display::vertex_normals[0]), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, (display::face_vertices).size() * sizeof(GLuint),
            &(display::face_vertices[0]), GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calculate the initial projection and model view matrices
        camera::CalculateProjectionMatrix();
        camera::CalculateModelViewMatrix();
    }

}