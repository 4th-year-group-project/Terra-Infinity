/**
 * This file contains a temp class that will be used to render the cardinal axes within the scene
 * starting from the origin. This will be used as a point of reference within the scene.
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
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;
};

#endif // AXES_HPP