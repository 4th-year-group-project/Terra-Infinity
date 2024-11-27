#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H 

#include <glm/glm.hpp>
#include <vector>

bool loadObj(
    const char * path,
    std::vector < glm::vec3 > & out_vertices,
    std::vector < glm::vec3 > & out_normals
);

#endif // LOAD_OBJ_H
