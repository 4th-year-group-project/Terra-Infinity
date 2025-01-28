/*
* Ocean.cpp
*
*  Created on: 24 Jan 2025
*  Author: Ethan Farrell

This class will hold the information to render a flat plane of water at the sea level of the world
for a particular subchunk. A combination of these will be used to render the ocean in the world.
*/

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glad/glad.h>
#endif

#include <Ocean.hpp>

using namespace std;

void Ocean::setupData() {
    // We need to create four vertices for the ocean quad which start at the ocean quad origin
    // and spans the size of the subchunk. The sea level will be multipled with the maximum height

    float worldSeaLevel = seaLevel * 192.0f;
    Vertex bottomLeft = Vertex(
        glm::vec3(oceanQuadOrigin[0], worldSeaLevel, oceanQuadOrigin[1]),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 0.0f)
    );
    Vertex topRight = Vertex(
        glm::vec3(oceanQuadOrigin[0] + size, worldSeaLevel, oceanQuadOrigin[1] + size),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
    );
    Vertex bottomRight = Vertex(
        glm::vec3(oceanQuadOrigin[0] + size, worldSeaLevel, oceanQuadOrigin[1]),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(1.0f, 0.0f)
    );
    Vertex topLeft = Vertex(
        glm::vec3(oceanQuadOrigin[0], worldSeaLevel, oceanQuadOrigin[1] + size),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec2(0.0f, 1.0f)
    );

    // Add the vertices to the vertices vector
    setVertices({bottomLeft, bottomRight, topLeft, topRight});
    // Add the indices to the indices vector with a CCW winding order
    setIndices({0, 1, 3, 0, 3, 2});

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3) * 2 + sizeof(glm::vec2), (void*)(sizeof(glm::vec3) * 2));
    glEnableVertexAttribArray(2);

}

void Ocean::render(glm::mat4 view, glm::mat4 projection) {
    //TODO: Bind the shader and the textures if required
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Ocean::updateData() {
    // Do nothing as the ocean is static
}
