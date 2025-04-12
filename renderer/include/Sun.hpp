/**
 * This is a class that will represent the sun that needs to be rendered in the world.
 * The sun will be rendered as a point light source in the sky that will move across the sky very
 * slowly to simulate natural lighting.
 */

#ifndef SUN_HPP
#define SUN_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glad/glad.h>
#endif

#include "Light.hpp"
#include "Settings.hpp"
#include "IRenderable.hpp"
#include "Object.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Settings.hpp"

class Sun : public Light {
private:
    vector<Vertex> vertices; // The vertices of the sun
    vector<unsigned int> indices; // The indices of the sun
    float radius; // The radius of the sun cube

public:
    Sun(
        glm::vec3 position,
        glm::vec3 colour,
        glm::vec3 ambient,
        glm::vec3 diffuse,
        glm::vec3 specular,
        float radius,
        Settings settings
    );

    ~Sun() {};

    float getRadius() { return radius; }
    void setRadius(float inRadius) { radius = inRadius; }

    vector<Vertex> getVertices() { return vertices; }
    void setVertices(vector<Vertex> inVertices) { vertices = inVertices; }
    vector<unsigned int> getIndices() { return indices; }
    void setIndices(vector<unsigned int> inIndices) { indices = inIndices; }

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;

};

#endif // SUN_HPP
