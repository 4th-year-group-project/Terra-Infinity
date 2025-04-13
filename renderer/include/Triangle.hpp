/**
 * This file contains a class for a triangle object that will be used to test if the renderer is
 * able to render objects in the scene.
 */

#ifndef TRIANGLE_HPP
#define TRIANGLE_HPP

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
#include "Light.hpp"

using namespace std;

class Triangle : public Object, virtual public IRenderable{
private:
    vector<Vertex> vertices; // The vertices of the triangle
    vector<unsigned int> indices; // The indices of the triangle
public:
    Triangle(shared_ptr<Settings> settings);
    ~Triangle();

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

#endif //TRIANGLE_HPP