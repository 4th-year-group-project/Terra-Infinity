/**
 * This file contains the implementation for the sun class which will be used to light the scene
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

#include "Sun.hpp"
#include "Light.hpp"
#include "Settings.hpp"
#include "IRenderable.hpp"
#include "Object.hpp"
#include "Vertex.hpp"
#include "Shader.hpp"
#include "Settings.hpp"

using namespace std;

Sun::Sun(
    glm::vec3 position,
    glm::vec3 colour,
    glm::vec3 ambient,
    glm::vec3 diffuse,
    glm::vec3 specular,
    float radius,
    Settings settings
): Light(position, colour, ambient, diffuse, specular), radius(radius)
{
    // Create the vertices of the sun cube
    vertices = {
        Vertex(
            glm::vec3(-radius, -radius, radius),
            glm::vec3(-0.57735026919, -0.57735026919, 0.57735026919),
            glm::vec2(0.0f, 0.0f)
        ),
        Vertex(
            glm::vec3(radius, -radius, radius),
            glm::vec3(0.57735026919, -0.57735026919, 0.57735026919),
            glm::vec2(1.0f, 0.0f)
        ),
        Vertex(
            glm::vec3(radius, radius, radius),
            glm::vec3(0.57735026919, 0.57735026919, 0.57735026919),
            glm::vec2(1.0f, 1.0f)
        ),
        Vertex(
            glm::vec3(-radius, radius, radius),
            glm::vec3(-0.57735026919, 0.57735026919, 0.57735026919),
            glm::vec2(0.0f, 1.0f)
        ),
        Vertex(
            glm::vec3(-radius, -radius, -radius),
            glm::vec3(-0.57735026919, -0.57735026919, -0.57735026919),
            glm::vec2(0.0f, 0.0f)
        ),
        Vertex(
            glm::vec3(radius, -radius, -radius),
            glm::vec3(0.57735026919, -0.57735026919, -0.57735026919),
            glm::vec2(1.0f, 0.0f)
        ),
        Vertex(
            glm::vec3(radius, radius, -radius),
            glm::vec3(0.57735026919, 0.57735026919, -0.57735026919),
            glm::vec2(1.0f, 1.0f)
        ),
        Vertex(
            glm::vec3(-radius, radius, -radius),
            glm::vec3(-0.57735026919, 0.57735026919, -0.57735026919),
            glm::vec2(0.0f, 1.0f)
        )
    };

    indices = {
        0, 1, 2, 2, 3, 0,  // Front face
        1, 5, 6, 6, 2, 1,  // Right face
        5, 4, 7, 7, 6, 5,  // Back face
        4, 0, 3, 3, 7, 4,  // Left face
        3, 2, 6, 6, 7, 3,  // Top face
        4, 5, 1, 1, 0, 4   // Bottom face
    };

    // Set the model matrix
    model = glm::translate(glm::mat4(1.0f), position);
    // Set the normal matrix
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    string shaderRoot = getenv("SHADER_ROOT");
    shader = make_shared<Shader>(
        shaderRoot + settings.getFilePathDelimitter() + "sun_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "sun_shader.fs"
    );

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Sun::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
){
    // Use the shader
    shader->use();

    // Set the model, view and projection matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);

    // Bind the VAO
    glBindVertexArray(VAO);
    // Draw the sun
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}
#pragma GCC diagnostic pop

void Sun::setupData(){
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

    // Unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sun::updateData(bool){
    // We are going to update the position of the 
    // Do nothing as the sun is static
}

