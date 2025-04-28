/**
 * @file Screen.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Screen class.
 * @details This class is used to render the offscreen framebuffer to the screen. It will have the
 * texture of the framebuffer.
 * @version 1.0
 * @date 2025
 * 
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

/**
 * @brief Construct a new Screen object with the default values
 * 
 * @details This constructor will create a screen object with the default values for the screen.
 * The default values are:
 * - quadVertices: The vertices of the screen quad to cover the entire screen
 * - quadTexCoords: The texture coordinates of the screen quad to map across the whole texture
 * - shader: The shader for the screen
 * - screenTexture: The texture for the screen
 *
 * 
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * 
 */
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

/**
 * @brief Construct a new Screen object with the given screen texture
 * 
 * @details This constructor will create a screen object with the given screen texture. It will
 * also set up the vertices and texture coordinates for the screen quad.
 * 
 * @param inScreenTexture [in] unsigned int The screen texture to use
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * 
 */
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

/**
 * @brief Default destructor for the Screen class allowing for standard cleanup
 * 
 */
Screen::~Screen(){
    // Do nothing
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Renders the screen to the window
 * 
 * @details This function will render the screen to the window. It will bind the framebuffer and
 * then render the screen quad with the texture of the framebuffer.
 * 
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] vector<shared_ptr<Light>> The lights in the scene
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether this is a water pass or not
 * @param isShadowPass [in] bool Whether this is a shadow pass or not
 * @param plane [in] glm::vec4 The plane for the water reflection
 * 
 * @return void
 */
void Screen::render(
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

/**
 * @brief Sets up the data for the screen
 * 
 * @details This function will set up the data for the screen. In this case, there is no data to
 * set up.
 * 
 * @return void
 * 
 */
void Screen::setupData(){
}

/**
 * @brief Updates the data for the screen
 * 
 * @details This function will update the data for the screen. In this case, there is no data to
 * update.
 * 
 * @param regenerate [in] bool Whether to regenerate the data or not
 * 
 * @return void
 * 
 */
void Screen::updateData(bool){
    // Nothing to update
}

