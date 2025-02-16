/**
 * This file contains the class for the screen class. This is a renderable object that will be used
 * to render the offscreen framebuffer to the screen. This will have the texture of the framebuffer.
 */

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
#include "Screen.hpp"
#include "Settings.hpp"

using namespace std;

Screen::Screen(shared_ptr<Settings> settings){
    // Set up the vertices of the screen quad
    quadVertices = {
        glm::vec2(-1.0f, 1.0f), // Top-left
        glm::vec2(-1.0f, -1.0f), // Bottom-left
        glm::vec2(1.0f, -1.0f), // Bottom-right

        glm::vec2(-1.0f, 1.0f), // Top-left
        glm::vec2(1.0f, -1.0f), // Bottom-right
        glm::vec2(1.0f, 1.0f) // Top-right
    };

    // Set up the texture coordinates of the screen quad
    quadTexCoords = {
        glm::vec2(0.0f, 1.0f), // Top-left
        glm::vec2(0.0f, 0.0f), // Bottom-left
        glm::vec2(1.0f, 0.0f), // Bottom-right

        glm::vec2(0.0f, 1.0f), // Top-left
        glm::vec2(1.0f, 0.0f), // Bottom-right
        glm::vec2(1.0f, 1.0f) // Top-right
    };

    string shaderRoot = getenv("SHADER_ROOT");
    Shader screenShader = Shader(
        shaderRoot + settings->getFilePathDelimitter() + "screen_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "screen_shader.fs"
    );
    setShader(make_shared<Shader>(screenShader));
    // Generate the VAO and VBO (We are not using the EBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        quadVertices.size() * sizeof(glm::vec2) + quadTexCoords.size() * sizeof(glm::vec2),
        NULL,
        GL_STATIC_DRAW
    );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        quadVertices.size() * sizeof(glm::vec2),
        &quadVertices[0]
    );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        quadVertices.size() * sizeof(glm::vec2),
        quadTexCoords.size() * sizeof(glm::vec2),
        &quadTexCoords[0]
    );
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(1);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(quadVertices.size() * sizeof(glm::vec2)));
}

Screen::Screen(const unsigned int inScreenTexture, shared_ptr<Settings> settings){
    screenTexture = inScreenTexture;
    // Now we call the other constructor
    quadVertices = {
        glm::vec2(-1.0f, 1.0f), // Top-left
        glm::vec2(-1.0f, -1.0f), // Bottom-left
        glm::vec2(1.0f, -1.0f), // Bottom-right

        glm::vec2(-1.0f, 1.0f), // Top-left
        glm::vec2(1.0f, -1.0f), // Bottom-right
        glm::vec2(1.0f, 1.0f) // Top-right
    };
    quadTexCoords = {
        glm::vec2(0.0f, 1.0f), // Top-left
        glm::vec2(0.0f, 0.0f), // Bottom-left
        glm::vec2(1.0f, 0.0f), // Bottom-right

        glm::vec2(0.0f, 1.0f), // Top-left
        glm::vec2(1.0f, 0.0f), // Bottom-right
        glm::vec2(1.0f, 1.0f) // Top-right
    };

    string shaderRoot = getenv("SHADER_ROOT");
    this->shader = make_shared<Shader>(
        shaderRoot + settings->getFilePathDelimitter() + "screen_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "screen_shader.fs"
    );
    // Generate the VAO and VBO (We are not using the EBO)
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        quadVertices.size() * sizeof(glm::vec2) + quadTexCoords.size() * sizeof(glm::vec2),
        NULL,
        GL_STATIC_DRAW
    );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        quadVertices.size() * sizeof(glm::vec2),
        &quadVertices[0]
    );
    glBufferSubData(
        GL_ARRAY_BUFFER,
        quadVertices.size() * sizeof(glm::vec2),
        quadTexCoords.size() * sizeof(glm::vec2),
        &quadTexCoords[0]
    );
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);
    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)(quadVertices.size() * sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);

}

Screen::~Screen(){
    // Do nothing
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Screen::render(glm::mat4 view, glm::mat4 projection){
    // Use the shader
    shader->use();
    shader->setInt("screenTexture", 0);

    // Bind the VAO
    glBindVertexArray(VAO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    shader->deactivate();
}
#pragma GCC diagnostic pop

void Screen::setupData(){
}

void Screen::updateData(){
    // Nothing to update
}

