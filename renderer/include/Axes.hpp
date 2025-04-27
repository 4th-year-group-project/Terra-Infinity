/**
 * @file Axes.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Axes class.
 * @version 0.1
 * @date 2025
 *
 */
#ifndef AXES_HPP
#define AXES_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <GLFW/glfw3.h>
#endif

#include "IRenderable.hpp"
#include "Object.hpp"

#include "Vertex.hpp"
#include "Settings.hpp"

using namespace std;

/**
 * @brief Class that will be used to render the cardinal axes within the scene
 * starting from the origin.
 *
 * This will be used as a point of reference within the scene whilst developing.
 *
 * @details
 * The axes are rendered as lines with different colors for each axis.
 * The X axis is red, the Y axis is green, and the Z axis is blue.
 * The axes are rendered in a 3D space with a length of 1 unit.
*/
class Axes : public Object, public IRenderable{
private:
    vector<Vertex> vertices;
    vector<unsigned int> indices;

public:
    Axes(Settings settings);
    ~Axes();

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

#endif // AXES_HPP
