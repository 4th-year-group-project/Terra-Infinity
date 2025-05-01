/**
 * @file Cube.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Cube class, which is used to render a cube within the scene.
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
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Cube.hpp"
#include "Settings.hpp"

using namespace std;

/**
 * @brief Construct a new Cube object with the default values
 *
 * @details This constructor will create a cube at the origin with the default values for the cube.
 * The default values are:
 * - vertices: The vertices of the cube
 * - indices: The indices of the cube
 * - shader: The shader for the cube
 * - texture: The texture for the cube
 * - model: The model matrix for the cube
 * - normalMatrix: The normal matrix for the cube
 *
 * @param settings [in] std::shared_ptr<Settings> The settings object
 *
 * @return void
 *
 */
Cube::Cube(shared_ptr<Settings> settings){
    vertices = {
            //  x,    y,    z,     nx,   ny,   nz,   u,   v
            // { -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f }, // 0 - Back bottom left
            // {  0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f }, // 1 - Back bottom right
            // {  0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f }, // 2 - Back top right
            // { -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f }, // 3 - Back top left

            // { -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f }, // 4 - Front bottom left
            // {  0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f }, // 5 - Front bottom right
            // {  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f }, // 6 - Front top right
            // { -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f }  // 7 - Front top left
        Vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),

        Vertex(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),
    };

    indices = {
        // Back face
        0, 1, 2,
        2, 3, 0,
        // Front face
        4, 5, 6,
        6, 7, 4,
        // Left face
        0, 3, 7,
        7, 4, 0,
        // Right face
        1, 5, 6,
        6, 2, 1,
        // Top face
        3, 2, 6,
        6, 7, 3,
        // Bottom face
        0, 1, 5,
        5, 4, 0
    };

    string shaderRoot = getenv("SHADER_ROOT");
    Shader cubeShader = Shader(
        shaderRoot + settings->getFilePathDelimitter() + "cube_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "cube_shader.fs"
    );
    setShader(make_shared<Shader>(cubeShader));

    model = glm::mat4(1.0f);
    // Scale the cube by a factor of 10 in all directions
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    setupData();
}

/**
 * @brief Default destructor for the Cube class
 *
 */
Cube::~Cube(){
    // Do nothing
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Renders the cube in the scene
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
void Cube::render(
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
    shader->setMat3("normalMatrix", normalMatrix);
    shader->setVec3("colour", glm::vec3(1.0f, 0.5f, 0.31f));

    // Bind the VAO
    glBindVertexArray(VAO);

    // Draw the cube
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop


/**
 * @brief Sets up the data for a renderable object
 *
 * @details This method will setup the data for the cube. This is used to setup the data for the
 * cube within the scene.
 *
 * @returns void
 *
 */
void Cube::setupData(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

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
 * @details This method will update the data for the cube. This is used to update the data for the
 * cube within the scene.
 *
 * @param regenerate [in] bool Whether to regenerate the data or not
 *
 * @returns void
 *
 */
void Cube::updateData(bool regenerate, int frame_counter){
    // Do nothing
    // cout << "We are updating the data for the cube" << endl;
}
