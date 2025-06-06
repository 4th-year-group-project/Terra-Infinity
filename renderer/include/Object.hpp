/**
 * @file Object.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the abstract Object class.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glad/glad.h>
#endif

#include "Shader.hpp"
#include "Texture.hpp"

using namespace std;

/**
 * @brief This class is used to define the properities of an object that will be rendered to the screen. It is an
 * abstract class that will be extended by other classes to define specific objects.
 *
 */
class Object {
protected:
    unsigned int VAO; // The vertex array object for the object
    unsigned int VBO; // The vertex buffer object for the object
    unsigned int EBO; // The element buffer object for the object
    shared_ptr<Shader> shader; // The shader that the object will use
    vector<shared_ptr<Texture>> textures; // The textures that the object will use
    glm::mat4 model; // The model matrix of the object
    glm::mat3 normalMatrix; // The normal matrix of the object
public:
    Object() {
        VAO = 0;
        VBO = 0;
        EBO = 0;
        textures = vector<shared_ptr<Texture>>();
    }

    unsigned int getVAO() { return VAO; }
    unsigned int getVBO() { return VBO; }
    unsigned int getEBO() { return EBO; }
    shared_ptr<Shader> getShader() { return shader; }
    vector<shared_ptr<Texture>> getTextures() { return textures; }
    glm::mat4 getModel() { return model; }
    glm::mat3 getNormalMatrix() { return normalMatrix; }

    void setVAO(unsigned int inVAO) { VAO = inVAO; }
    void setVBO(unsigned int inVBO) { VBO = inVBO; }
    void setEBO(unsigned int inEBO) { EBO = inEBO; }
    void setShader(shared_ptr<Shader> inShader) { shader = inShader; }
    void setTextures(vector<shared_ptr<Texture>> inTextures) { textures = inTextures; }
    void addTexture(shared_ptr<Texture> inTexture) { textures.push_back(inTexture); }
    void setModel(glm::mat4 inModel) { model = inModel; }
    void setNormalMatrix(glm::mat3 inNormalMatrix) { normalMatrix = inNormalMatrix; }
};

#endif // OBJECT_HPP
