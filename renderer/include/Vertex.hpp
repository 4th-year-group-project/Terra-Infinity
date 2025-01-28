#ifndef VERTEX_HPP
#define VERTEX_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

/*
    This class will hold all of the information for a vertex that is to be rendered by the
    application. It stores the position of the vertex, the normal of the vertex, the texture
    coordinates
*/
class Vertex
{
private:
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
public:
    Vertex(
        glm::vec3 inPosition,
        glm::vec3 inNormal,
        glm::vec2 inTexCoords
    ):
        position(inPosition),
        normal(inNormal),
        texCoords(inTexCoords) {};
    Vertex(){
        position = glm::vec3(0.0f, 0.0f, 0.0f);
        normal = glm::vec3(0.0f, 0.0f, 0.0f);
        texCoords = glm::vec2(0.0f, 0.0f);
    }
    glm::vec3 getPosition() { return position; }
    glm::vec3 getNormal() { return normal; }
    glm::vec2 getTexCoords() { return texCoords; }
    void setPosition(glm::vec3 inPosition) { position = inPosition; }
    void setNormal(glm::vec3 inNormal) { normal = inNormal; }
    void setTexCoords(glm::vec2 inTexCoords) { texCoords = inTexCoords; }
    ~Vertex() {};
};

#endif
