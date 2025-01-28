#ifndef OBJECT_HPP
#define OBJECT_HPP


/*
    This is an abstract class for a renderable object that can be rendered by implementing the
    IRenderable "interface". This class will hold the information that is required for the object
    to be successfully rendered with OpenGL.
*/

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glad/glad.h>
#endif

// #include <IRenderable.hpp>

using namespace std;

class Object {
protected:
    unsigned int VAO; // The vertex array object for the object
    unsigned int VBO; // The vertex buffer object for the object
    unsigned int EBO; // The element buffer object for the object
    // Shader shader; // The shader that will be used to render the object
    // vector<Texture> textures; // The textures that will be used to render the object
public:
    Object() {
        VAO = 0;
        VBO = 0;
        EBO = 0;
    }

    unsigned int getVAO() { return VAO; }
    unsigned int getVBO() { return VBO; }
    unsigned int getEBO() { return EBO; }
    // Shader getShader() { return shader; }
    // vector<Texture> getTextures() { return textures; }

    void setVAO(unsigned int inVAO) { VAO = inVAO; }
    void setVBO(unsigned int inVBO) { VBO = inVBO; }
    void setEBO(unsigned int inEBO) { EBO = inEBO; }
    // void setShader(Shader inShader) { shader = inShader; }
    // void setTextures(vector<Texture> inTextures) { textures = inTextures; }
    // void addTexture(Texture inTexture) { textures.push_back(inTexture); }
};

#endif
