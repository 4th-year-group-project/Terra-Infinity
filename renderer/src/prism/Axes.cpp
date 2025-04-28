/**
 * @file Axes.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Axes class, which is used to render the cardinal axes
 * within the scene starting from the origin. The axes are rendered as lines with different colors for each axis.
 * @version 1.0
 * @date 2025
 *
 */

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
#include "Shader.hpp"
#include "Vertex.hpp"
#include "Settings.hpp"
#include "Axes.hpp"

using namespace std;

/**
 * @brief Construct a new Axes object
 *
 * @param settings (Settings) The standard renderer settings object
 */
Axes::Axes(Settings settings){
    vertices = {
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(15.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 15.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.0f, 0.0f, 15.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f))
    };

    indices = {
        0, 1,
        2, 3,
        4, 5
    };

    string shaderRoot = getenv("SHADER_ROOT");
    Shader axesShader = Shader(
        shaderRoot + settings.getFilePathDelimitter() + "axis_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "axis_shader.fs"
    );
    setShader(make_shared<Shader>(axesShader));

    model = glm::mat4(1.0f);
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    setupData();
}

/**
 * @brief Destroy the Axes:: Axes object
 *
 * @details This destructor will delete the VAO, VBO and EBO buffers.
 */
Axes::~Axes(){
    // Do nothing
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Renders the axes in the scene
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
void Axes::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
){
    // Use the shader
    shader->use();
    // Set the model, view and projection matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    // Bind the VAO
    glBindVertexArray(VAO);
    // Draw the axes
    glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop

/**
 * @brief Sets up the data for a renderable object
 *
 * @details This function will set up the vertex array object (VAO), vertex buffer object (VBO) and element buffer 
 * object (EBO) for the renderable object. It also sets the vertex attribute pointers for the position, normal and
 * texture coordinates.
 *
 * @return void
 *
 */
void Axes::setupData(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}

/**
 * @brief Updates the data for a renderable object
 *
 * @details This function will update the data for the renderable object. It is currently empty and does not do anything.
 *
 * @return void
 *
 */
void Axes::updateData(bool){
    // Do nothing
}
