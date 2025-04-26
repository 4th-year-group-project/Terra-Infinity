/**
 * @file Screen.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Screen class.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

#include "IRenderable.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Settings.hpp"

using namespace std;

/**
 * @brief This class contains the definition for the screen class. It is a simple quad object that will be used
 * to render the final framebuffer to the screen.
 *
 * @details This object will allow for detailed post-processing effects to be applied in the final shader.
 *
 */
class Screen : public Object, public IRenderable{
private:
    vector<glm::vec2> quadVertices; // The vertices of screen quad
    vector<glm::vec2> quadTexCoords; // The texture coordinates of the screen quad
    unsigned int screenTexture; // The texture of the screen
public:
    Screen(shared_ptr<Settings> settings);
    Screen(const unsigned int inScreenTexture, shared_ptr<Settings> settings);
    ~Screen();

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos,
        bool isWaterPass,
        bool isShadowPass,
        glm::vec4 plane
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;

};

#endif // SCREEN_HPP
