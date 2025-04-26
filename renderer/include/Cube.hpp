/**
 * @file Cube.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Cube class.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef CUBE_HPP
#define CUBE_HPP

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
 * @brief Class that will be used to render a cube within the scene.
 *
 * @details These objects will be used as test objects to ensure that the confgiuration of the renderer
 * is correct and that the objects are being rendered correctly.
 * The cube is rendered as a 3D object with a length of 1 unit.
 */
class Cube : public Object, virtual public IRenderable{
private:
    vector<Vertex> vertices; // The vertices of the triangle
    vector<unsigned int> indices; // The indices of the triangle
public:
    Cube(shared_ptr<Settings> settings);
    ~Cube();

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

#endif //CUBE_HPP
