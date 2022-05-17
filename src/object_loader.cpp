#define _CRT_SECURE_NO_DEPRECATE

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <iostream>
#include <algorithm>

#include "GL/freeglut.h"

#include "camera.hpp"
#include "display.hpp"
#include "object_loader.hpp"

namespace object_loader {

    std::vector<GLfloat> face_normals;
    std::vector<GLfloat> face_areas;
    std::vector<std::list<GLuint>> member_faces;

    /*
     * Reads the vertex/face data from the given file. Returns true for successful load; false otherwise.
     */
    bool LoadObject(char *filepath) {
        FILE *fp;
        fp = fopen(filepath, "r");

        if (fp == NULL) {
            printf("Can't open \"%s\"\n", filepath);
            return false;
        }

        int line_count = 1;

        while (!(feof(fp))) {
            char ch;
            fscanf(fp, "%c", &ch);

            if (ch == VERTEX) {
                GLfloat x, y, z; // coordinates for this vertex
                int values_read = fscanf(fp, "%f %f %f\n", &x, &y, &z);

                if (values_read != 3) {
                    printf("Less than 3 values on line %d\n", line_count);
                    return false;
                }

                display::vertex_coordinates.push_back(x);
                display::vertex_coordinates.push_back(y);
                display::vertex_coordinates.push_back(z);

                display::max_x = std::max(x, display::max_x);
                display::max_y = std::max(y, display::max_y);
                display::max_z = std::max(z, display::max_z);

                display::min_x = std::min(x, display::min_x);
                display::min_y = std::min(y, display::min_y);
                display::min_z = std::min(z, display::min_z);
            } else if (ch == FACE) {
                GLuint v1, v2, v3; // vertex indices for this face
                int values_read = fscanf(fp, "%d %d %d\n", &v1, &v2, &v3);

                if (values_read != 3) {
                    printf("Less than 3 values on line %d\n", line_count);
                    return false;
                }

                display::face_vertices.push_back(v1 - 1);
                display::face_vertices.push_back(v2 - 1);
                display::face_vertices.push_back(v3 - 1);
            } else {
                fscanf(fp, "\n"); // character other than 'v' or 'f'; skip line
            }

            line_count++;
        }

        display::max_dimension = std::max(abs(display::max_x - display::min_x), abs(display::max_y - display::min_y));

        ProcessFaces();
        return true;
    }

    /*
     * Calculates the normal and area of each face, and builds a map of
     * vertices to the faces containing them.
     */
    void ProcessFaces() {
        int num_faces = display::face_vertices.size();
        int num_vertices = display::vertex_coordinates.size() / 3;

        // Reserve space for every vertex in the map of vertices to faces
        member_faces.resize(num_vertices);

        for (int i = 0; i < num_faces; i += 3) {
            int ind0 = display::face_vertices[i];
            int ind1 = display::face_vertices[i + 1];
            int ind2 = display::face_vertices[i + 2];

            // Add this face to the lists of faces for each member vertex
            member_faces[ind0].push_back(i / 3);
            member_faces[ind1].push_back(i / 3);
            member_faces[ind2].push_back(i / 3);

            // Calculate the normal for this face
            glm::vec3 v0(display::vertex_coordinates[ind0 * 3],
                display::vertex_coordinates[ind0 * 3 + 1],
                display::vertex_coordinates[ind0 * 3 + 2]);

            glm::vec3 v1(display::vertex_coordinates[ind1 * 3],
                display::vertex_coordinates[ind1 * 3 + 1],
                display::vertex_coordinates[ind1 * 3 + 2]);

            glm::vec3 v2(display::vertex_coordinates[ind2 * 3],
                display::vertex_coordinates[ind2 * 3 + 1],
                display::vertex_coordinates[ind2 * 3 + 2]);

            glm::vec3 edge1(v1 - v0);
            glm::vec3 edge2(v2 - v0);

            glm::vec3 crossProd(glm::cross(edge1, edge2));
            glm::vec3 normal(glm::normalize(crossProd));

            face_normals.push_back(normal.x);
            face_normals.push_back(normal.y);
            face_normals.push_back(normal.z);

            face_areas.push_back(0.5f * glm::length(crossProd));
        }

        CalculateVertexNormals();
    }


    /*
     * For each vertex, loops through member faces to calculate normal.
     * Uses the average of adjacent face normals, weighted by face area,
     * to implement smooth Gouraud shading.
     */
    void CalculateVertexNormals() {
        int num_vertices = display::vertex_coordinates.size() / 3;

        for (int i = 0; i < num_vertices; i++) {
            glm::vec3 vertNormal(0.0f, 0.0f, 0.0f);

            while (!member_faces[i].empty()) {
                GLuint faceIndex = member_faces[i].front();
                member_faces[i].pop_front();

                glm::vec3 faceNormal(face_normals[faceIndex * 3],
                    face_normals[faceIndex * 3 + 1],
                    face_normals[faceIndex * 3 + 2]);

                vertNormal += face_areas[faceIndex] * faceNormal;
            }

            vertNormal = glm::normalize(vertNormal);

            display::vertex_normals.push_back(vertNormal.x);
            display::vertex_normals.push_back(vertNormal.y);
            display::vertex_normals.push_back(vertNormal.z);
        }
    }

}