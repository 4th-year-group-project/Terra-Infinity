#ifndef LOAD_OBJ_H
#define LOAD_OBJ_H

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

bool loadObj(
    const char * path,
    std::vector <glm::vec3> & out_vertices,
    std::vector <glm::vec3> & out_normals,
    std::vector <unsigned int*> & out_indices
);

#endif // LOAD_OBJ_H
