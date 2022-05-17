#pragma once

#ifndef OBJECTLOADER_H
#define OBJECTLOADER_H

#include <list>

namespace object_loader {

    #define VERTEX 'v'
    #define FACE 'f'

    extern std::vector<GLfloat> face_normals;
    extern std::vector<GLfloat> face_areas;
    extern std::vector<std::list<GLuint>> member_faces;

    bool LoadObject(char *filepath);
    void ProcessFaces();
    void CalculateVertexNormals();

}

#endif