/**
 * This file contains a class for a triangle object that will be used to test if the renderer is
 * able to render objects in the scene.
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

using namespace std;

Triangle::Triangle(shared_ptr<Settings> settings){
    // Call the parent constructor
    // Object();
    // Set up the vertices of the triangle
    vertices = {
        // These are of the form (position, normal, texture coordinates)
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

    // Temp
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // cout << "VAO After Gen: " << VAO << endl;
    // setVAO(VAO);
    // setVBO(VBO);
    // setEBO(EBO);
    // cout << "VBO After Gen: " << VBO << endl;
    // cout << "EBO After Gen: " << EBO << endl;

    // cout << "Size of Vertex: " << sizeof(Vertex) << endl;
    // cout << "Computed size: " << sizeof(glm::vec3) * 2 + sizeof(glm::vec2) << endl;
    // Bind the VAO
    glBindVertexArray(VAO);

    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, &positions[0], GL_STATIC_DRAW);

    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * 6, &indices[0], GL_STATIC_DRAW);

    // // Set the vertex attribute pointers
    // // Position
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    // glEnableVertexAttribArray(0);
    

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

Triangle::~Triangle(){
    // Do nothing
    // cout << "We are deleting the triangle" << endl;
    textures.clear();
}

void Triangle::render(glm::mat4 view, glm::mat4 projection){
    // Use the shader
    // cout << "Rendering the triangle" << endl;
    shader->use();

    // Get the active shader program
    GLint shaderProgram;
    glGetIntegerv(GL_CURRENT_PROGRAM, &shaderProgram);
    // cout << "Shader program: " << shaderProgram << endl;

    // Set the model, view and projection matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);
    shader->setVec3("colour", glm::vec3(1.0f, 0.5f, 0.31f));

    // cout << "The VAO is: " << getVAO() << endl;
    // Bind the VAO
    unsigned int boundedVAO;
    boundedVAO = getVAO();
    glBindVertexArray(boundedVAO);
    // output the data in the VAO
    GLint size;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    std::vector<float> vertexData(size / sizeof(float));
    glGetBufferSubData(GL_ARRAY_BUFFER, 0, size, vertexData.data());

    // cout << "Vertex Data: ";
    // for (const auto& i : vertexData) {
    //     // cout << i << " ";
    // }
    // cout << endl;

    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
    std::vector<unsigned int> indexData(size / sizeof(unsigned int));
    glGetBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, indexData.data());

    // cout << "Index Data: ";
    // for (const auto& i : indexData) {
    //     // cout << i << " ";
    // }
    // // cout << endl;
    // Draw the triangle
    GLint boundVAO;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
    // cout << "Bound VAO: " << boundVAO << endl;
    // We need to print the camera details
    try
    {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    // glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}

void Triangle::setupData(){
    // Generate the VAO, VBO and EBO
    // this->setVAO(20);
    // Texture temp = Texture();
    // addTexture(make_shared<Texture>(temp));
    // // cout << "VAO: " << getVAO() << endl;
}

void Triangle::updateData(){
    // Do nothing
}