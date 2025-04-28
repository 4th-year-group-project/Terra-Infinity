/**
 * @file Ocean.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Ocean class.
 * @version 1.0
 * @date 2025
 * 
 */

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif

#include "Ocean.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

using namespace std;

/**
 * @brief Construct a new Ocean object
 * 
 * @details This constructor will create an ocean object with the given parameters. The ocean will
 * be created at the given ocean quad origin and will be rendered at the given world coordinates.
 * 
 * @param inOceanQuadOrigin [in] std::vector<float> The origin of the ocean quad
 * @param inWorldCoords [in] std::vector<float> The world coordinates of the ocean
 * @param inSettings [in] std::shared_ptr<Settings> The settings object
 * @param inShader [in] std::shared_ptr<Shader> The shader object
 * @param inReflectionBuffer [in] std::shared_ptr<WaterFrameBuffer> The reflection buffer
 * @param inRefractionBuffer [in] std::shared_ptr<WaterFrameBuffer> The refraction buffer
 * @param inOceanTextures [in] std::vector<std::shared_ptr<Texture>> The ocean textures
 */
Ocean::Ocean(
    vector<float> inOceanQuadOrigin,
    vector<float> inWorldCoords,
    shared_ptr<Settings> inSettings,
    shared_ptr<Shader> inShader,
    shared_ptr<WaterFrameBuffer> inReflectionBuffer,
    shared_ptr<WaterFrameBuffer> inRefractionBuffer,
    vector<shared_ptr<Texture>> inOceanTextures
):
    settings(inSettings),
    oceanQuadOrigin(inOceanQuadOrigin),
    worldCoords(inWorldCoords),
    reflectionBuffer(inReflectionBuffer),
    refractionBuffer(inRefractionBuffer),
    oceanTextures(inOceanTextures),
    waveSpeed(0.03f),
    currentTime(0.0f),
    previousTime(0.0f),
    moveFactor(0.0f)
{
    seaLevel = settings->getSeaLevel();
    size = settings->getSubChunkSize();
    float worldSeaLevel = seaLevel * settings->getMaximumHeight();
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

/**
 * @brief Set up the render buffers for the ocean object
 * 
 * @details This function will set up the render buffers for the ocean object. This includes
 * creating the VAO, VBO and EBO buffers and binding them correctly. It will also set up the
 * vertex attribute pointers.
 * 
 * @return void
 */
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
/**
 * @brief Renders the ocean in the scene
 *
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] std::vector<std::shared_ptr<Light>> The lights in the scene
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether the water pass is being rendered
 * @param isShadowPass [in] bool Whether the shadow pass is being rendered
 * @param plane [in] glm::vec4 The plane used for the water pass
 *
 * @return void
 *
 */
void Ocean::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
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
    shader->setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 0.81f));
    shader->setVec3("material.specular", glm::vec3(0.6f, 0.6f, 0.6f));
    shader->setFloat("material.shininess", 20.0f);

    // Set the fog parameters
    shader->setFloat("fogParams.fogStart", settings->getFogStart());
    shader->setFloat("fogParams.fogEnd", settings->getFogEnd());
    shader->setFloat("fogParams.fogDensity", settings->getFogDensity());
    shader->setVec3("fogParams.fogColour", settings->getFogColor());

    // Set the clipping plane if it exists
    shader->setVec4("clippingPlane", plane);
    shader->setFloat("nearPlane", 0.1);
    shader->setFloat("farPlane", static_cast<float>((settings->getRenderDistance() -1.25) * settings->getSubChunkSize()));

    // using the reflection and refraction buffers we want to render set texture uniforms
    shader->setInt("reflectionTexture", 0);
    shader->setInt("refractionTexture", 1);
    shader->setInt("depthTexture", 2);

    currentTime = static_cast<float>(glfwGetTime());
    // Calculate how much of 1s has passed since the last frame

    moveFactor += waveSpeed *  (currentTime - previousTime);
    moveFactor = fmod(moveFactor, 1.0f);
    previousTime = currentTime;

    shader->setFloat("moveFactor", moveFactor);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, reflectionBuffer->getColourTexture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refractionBuffer->getColourTexture());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, refractionBuffer->getDepthTexture());

    for (size_t i = 0; i < oceanTextures.size(); i++) {
        shader->setInt(oceanTextures[i]->getName(), 3 + i);
        glActiveTexture(GL_TEXTURE3 + i);
        glBindTexture(GL_TEXTURE_2D, oceanTextures[i]->getId());
    }

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop

/**
 * @brief Update the ocean data
 * 
 * @details This function will update the ocean data. In this case, the ocean is static and
 * does not need to be updated.
 * 
 * @param regenerate [in] bool Whether to regenerate the data or not
 * 
 * @return void
 * 
 */
void Ocean::updateData(bool) {
    // Do nothing as the ocean is static
}
