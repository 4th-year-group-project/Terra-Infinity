/**
 * @file Light.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Light class.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Object.hpp"
#include "Settings.hpp"

using namespace std;

// We need a forward declaration of the IRenderable class
class IRenderable;

/**
 * @brief This class is used to create an abstract light object which contains the basic information required of any
 * light source in the scene.
 *
 */
class Light : public Object{
private:
    glm::vec3 position; // The position of the light
    glm::vec3 colour; // The colour of the light
    glm::vec3 ambient; // The ambient colour of the light
    glm::vec3 diffuse; // The diffuse colour of the light
    glm::vec3 specular; // The specular colour of the light

public:
    Light(
        glm::vec3 position,
        glm::vec3 colour,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular
    ): position(position), colour(colour), ambient(ambient), diffuse(diffuse), specular(specular) {}

    glm::vec3 getPosition() { return position; }
    glm::vec3 getColour() { return colour; }
    glm::vec3 getSpecular() { return specular; }
    glm::vec3 getAmbient() { return ambient; }
    glm::vec3 getDiffuse() { return diffuse; }

    void setPosition(glm::vec3 inPosition) { position = inPosition; }
    void setColour(glm::vec3 inColour) { colour = inColour; }
    void setSpecular(glm::vec3 inSpecular) { specular = inSpecular; }
    void setAmbient(glm::vec3 inAmbient) { ambient = inAmbient; }
    void setDiffuse(glm::vec3 inDiffuse) { diffuse = inDiffuse; }

    virtual void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos,
        bool isWaterPass,
        bool isShadowPass,
        glm::vec4 plane
    ) = 0;
    virtual void setupData() = 0;
    virtual void updateData(bool regenerate, int frame_counter) = 0;
};

#endif // LIGHT_HPP
