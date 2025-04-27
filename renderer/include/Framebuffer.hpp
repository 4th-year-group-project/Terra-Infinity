/**
 * @file Framebuffer.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Framebuffer class.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef FRAMEBUFFER_HPP
#define FRAMEBUFFER_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

using namespace std;

/**
 * @brief This class is used to create a framebuffer object that can be used for targetted rendering.
 *
 * @details
 * This class creates a multisample framebuffer that implements anti-aliasing whilst allowing the colour buffer
 * to be used as a texture. It also contains a combined depth and stencil buffer which can be used for depth and stencil
 * testing.
 *
 * The framebuffer is used to render the scene to a texture, which can then be used for post-processing effects.
 */
class Framebuffer{
private:
    glm::vec2 size; // The size of the framebuffer
    int multiSamples; // The number of samples per pixel
    unsigned int framebuffer; // The ID of the framebuffer
    unsigned int textureColourBuffer; // The color buffer of the framebuffer
    unsigned int depthStencilBuffer; // The depth buffer of the framebuffer

    unsigned int screenBuffer; // The screen buffer of the framebuffer
    unsigned int screenTexture; // The screen texture of the framebuffer


public:
    Framebuffer(glm::vec2 size, int multiSamples);
    Framebuffer(){};

    // Framebuffer(): Framebuffer(glm::vec2(1920, 1080), 4){};
    ~Framebuffer();

    glm::vec2 getSize(){return size;}
    int getMultiSamples(){return multiSamples;}
    unsigned int getFramebuffer(){return framebuffer;}
    unsigned int getColorBuffer(){return textureColourBuffer;}
    unsigned int getDepthBuffer(){return depthStencilBuffer;}
    unsigned int getScreenBuffer(){return screenBuffer;}
    unsigned int getScreenTexture(){return screenTexture;}
    void bindMultiSample();
    void unbindMultiSample();
    void bindScreen();
    void unbindScreen();
    void clearMultiSample();
    void clearScreen();
    void resize(glm::vec2 size){this->size = size;}
    void setMultiSamples(int multiSamples){this->multiSamples = multiSamples;}
    void setFramebuffer(unsigned int framebuffer);
    void setTextureColourBuffer(unsigned int textureColourBuffer);
    void setDepthStencilBuffer(unsigned int newDepthStencilBuffer);
    void setScreenBuffer(unsigned int screenBuffer);
    void setScreenTexture(unsigned int screenTexture);

    void blitMultiToScreen();

    void framebufferSizeCallback(GLFWwindow* window, int width, int height);
};


#endif // FRAMEBUFFER_HPP
