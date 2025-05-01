/**
 * @file Triangle.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Triangle class.
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
#include "Triangle.hpp"
#include "Settings.hpp"
#include "Light.hpp"

using namespace std;

/**
 * @brief Construct a new Triangle object with the default values
 * 
 * @details This constructor will create a triangle at the origin with the default values for the triangle.
 * The default values are:
 * - vertices: The vertices of the triangle
 * - indices: The indices of the triangle
 * - shader: The shader for the triangle
 * - texture: The texture for the triangle
 * - model: The model matrix for the triangle
 * - normalMatrix: The normal matrix for the triangle
 * 
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * 
 */
Triangle::Triangle(shared_ptr<Settings> settings){
    // Call the parent constructor
    // Object();
    // Set up the vertices of the triangle
    vertices = {
        // These are of the form (position, normal, texture coordinates, biome id)
        Vertex(glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f))
    };

    // Set up the indices of the triangle
    indices = {
        0, 1, 3, // First triangle
        1, 2, 3 // Second triangle
    };

    string shaderRoot = getenv("SHADER_ROOT");

    Shader triangleShader = Shader(
        shaderRoot + settings->getFilePathDelimitter() + "triangle_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "triangle_shader.fs"
    );
    setShader(make_shared<Shader>(triangleShader));

    model = glm::mat4(1.0f);
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    vector<glm::vec3> positions = {
        glm::vec3(0.5f, 0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(-0.5f, 0.5f, 0.0f)
    };
    
    setupData();
}

/**
 * @brief Default destructor for the Triangle class allowing for the standard cleanup
 * 
 */
Triangle::~Triangle(){
    // Do nothing
    // cout << "We are deleting the triangle" << endl;
    textures.clear();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Renders the triangle in the scene
 * 
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] std::vector<std::shared_ptr<Light>> The lights in the scene
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether or not this is a water pass
 * @param isShadowPass [in] bool Whether or not this is a shadow pass
 * @param plane [in] glm::vec4 The clipping plane
 * 
 */
void Triangle::render(
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

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop

/**
 * @brief This function will set up the data for the triangle
 * 
 * @details This function will set up the data for the triangle. This includes setting up the
 * vertex array object, vertex buffer object, and element buffer object. It will also set up the
 * vertex attribute pointers for the position, normal, and texture coordinates.
 * 
 * @return void
 * 
 */
void Triangle::setupData(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // // Bind the VBO and EBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),  (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
}

/**
 * @brief This function will update the data for the triangle
 * 
 * @details This function will update the data for the triangle. This includes updating the
 * vertex array object, vertex buffer object, and element buffer object. It will also update the
 * vertex attribute pointers for the position, normal, and texture coordinates.
 * 
 * @param regenerate [in] bool Whether or not to regenerate the data
 * 
 * @return void
 * 
 */
void Triangle::updateData(bool regenerate, int frame_counter){
    // Do nothing
}