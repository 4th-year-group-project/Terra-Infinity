
#include <string>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <stdio.h>
#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
#endif

#include <load_obj.h>

bool loadObj(
    const char * path,
    std::vector <glm::vec3> & out_vertices,
    std::vector <glm::vec3> & out_normals,
    // We want a vector of triple integers
    std::vector <unsigned int> & out_indices
) {
    // std::vector< unsigned int > vertexIndices;
    // std::vector< unsigned int > normalIndices;
    // std::vector< glm::vec3 > temp_vertices;
    // std::vector< glm::vec3 > temp_normals;
    std::cout << "Loading obj file: " << path << std::endl;
    FILE * file = fopen(path, "r");
    if( file == NULL ){
        printf("Impossible to open the file !\n");
        return false;
    }

    while( 1 ) {
        char lineHeader[128];
        // read the first word of the line
        int res = fscanf(file, "%s", lineHeader);
        // std::cout <<"Line Header:" <<  lineHeader << std::endl;
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        // else : parse lineHeader
        if ( strcmp( lineHeader, "v" ) == 0 ){
            glm::vec3 vertex;
            int matches = fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            if (matches != 3) {
                std::cerr << "Failed to read vertex coordinates\n";
                return false;
            }
            out_vertices.push_back(vertex);

        } else if (strcmp(lineHeader, "vn") == 0){
            glm::vec3 normal;
            int matches = fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
            if (matches != 3) {
                std::cerr << "Failed to read vertex normals\n";
                return false;
            }
            out_normals.push_back(normal);
        } else if ( strcmp( lineHeader, "f" ) == 0 ){
            unsigned int vertexIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d//%d %d//%d %d//%d\n", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1], &vertexIndex[2], &normalIndex[2]);
            if (matches != 6) {
                printf("File can't be read by our simple parser : ( Try exporting with other options\n");
                return false;
            }
            // Print out the read values
            // std::cout << "Vertex indices: " << vertexIndex[0] << " " << vertexIndex[1] << " " << vertexIndex[2] << std::endl;
            // std::cout << "Normal indices: " << normalIndex[0] << " " << normalIndex[1] << " " << normalIndex[2] << std::endl;
            // We have to subtract one from the indices as they are 1 indexed in the obj file
            vertexIndex[0] -= 1;
            vertexIndex[1] -= 1;
            vertexIndex[2] -= 1;
            // std::cout << "Vertex indices: " << vertexIndex[0] << " " << vertexIndex[1] << " " << vertexIndex[2] << std::endl;
            // std::cout << "Normal indices: " << normalIndex[0] << " " << normalIndex[1] << " " << normalIndex[2] << std::endl;
            out_indices.push_back(vertexIndex[0]);
            out_indices.push_back(vertexIndex[1]);
            out_indices.push_back(vertexIndex[2]);
            // std::cout << "index buffer at 0: " << out_indices.at(0)[0] << out_indices.at(0)[1] << out_indices.at(0)[2] << std::endl;
        }
    }

    // for( unsigned int i=0; i < normalIndices.size(); i++ ) {
    //     unsigned int normalIndex = normalIndices[i];
    //     glm::vec3 normal = temp_normals[ normalIndex-1 ];
    //     out_normals.push_back(normal);
    // }

    return true;
};