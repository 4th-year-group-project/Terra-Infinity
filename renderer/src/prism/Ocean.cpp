/*
* Ocean.cpp
*
*  Created on: 24 Jan 2025
*  Author: Ethan Farrell

This class will hold the information to render a flat plane of water at the sea level of the world
for a particular subchunk. A combination of these will be used to render the ocean in the world.
*/

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

#include "Ocean.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

using namespace std;


Ocean::Ocean(
    vector<float> inOceanQuadOrigin,
    vector<float> inWorldCoords,
    Settings inSettings,
    shared_ptr<Shader> inShader
):
    settings(inSettings),
    oceanQuadOrigin(inOceanQuadOrigin),
    worldCoords(inWorldCoords)
{
    seaLevel = settings.getSeaLevel();
    size = settings.getSubChunkSize();
    float worldSeaLevel = seaLevel * settings.getMaximumHeight();
    Vertex bottomLeft = Vertex(
        glm::vec3(oceanQuadOrigin[0], worldSeaLevel, oceanQuadOrigin[1]),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f)
    );
    Vertex topRight = Vertex(
        glm::vec3(oceanQuadOrigin[0] + (size - 1), worldSeaLevel, oceanQuadOrigin[1] + (size - 1)),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
    );
    Vertex bottomRight = Vertex(
        glm::vec3(oceanQuadOrigin[0] + (size - 1), worldSeaLevel, oceanQuadOrigin[1]),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f)
    );
    Vertex topLeft = Vertex(
        glm::vec3(oceanQuadOrigin[0], worldSeaLevel, oceanQuadOrigin[1] + (size - 1)),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f)
    );
    setVertices({bottomLeft, bottomRight, topLeft, topRight});
    setIndices({0, 2, 1, 1, 2, 3});

    shader = inShader;

    model = glm::translate(glm::mat4(1.0f), glm::vec3(worldCoords[0], 0.0f, worldCoords[1]));
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    setupData();
}

void Ocean::setupData() {
    // We need to create four vertices for the ocean quad which start at the ocean quad origin
    // and spans the size of the subchunk. The sea level will be multipled with the maximum height

    // Generate the VAO, VBO and EBO buffers by binding them correctly
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // We pass in the vertex data as a single buffer
    // This should be ordered as position, normal, texCoords per vertex
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // We set the mesh EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // We now need to set the vertex attribute pointers
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) * 2));
    glEnableVertexAttribArray(2);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Ocean::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
) {
    shader->use();
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);

    // We know that the sun should be the first light in the list
    shared_ptr<Light> sun = lights[0];
    shader->setVec3("viewPos", viewPos);
    // shader->setVec3("light.position", sun->getPosition());
    shader->setVec3("light.position", glm::vec3(-0.2f, 1.0f, 0.3f));
    shader->setVec3("light.ambient", sun->getAmbient() * sun->getColour());
    shader->setVec3("light.diffuse", sun->getDiffuse() * sun->getColour());
    shader->setVec3("light.specular", sun->getSpecular());

    // Set the material uniforms
    shader->setVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->setVec3("material.diffuse", glm::vec3(0.5f, 0.5f, 1.0f));
    shader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->setFloat("material.shininess", 32.0f);

    // Set the fog parameters
    shader->setFloat("fogParams.fogStart", settings.getFogStart());
    shader->setFloat("fogParams.fogEnd", settings.getFogEnd());
    shader->setFloat("fogParams.fogDensity", settings.getFogDensity());
    shader->setVec3("fogParams.fogColour", settings.getFogColor());


    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop

void Ocean::updateData() {
    // Do nothing as the ocean is static
}
