/**
 * @file Framebuffer.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Framebuffer class.
 * @details This class will contain our multi-sample frame buffer object. This will hold the
 * multiple different buffers such as the color buffer, depth buffer, and stencil buffer. This will
 * be used to render the scene to the screen.
 * @version 1.0
 * @date 2025
 * 
 */
#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
#endif

#include <Framebuffer.hpp>

using namespace std;

/**
 * @brief Construct a new Framebuffer object with the given size and number of samples
 * 
 * 
 * @param size [in] glm::vec2 The size of the framebuffer
 * @param multiSamples [in] int The number of samples to use for multi-sampling
 */
Framebuffer::Framebuffer(glm::vec2 size, int multiSamples):
    size(size),
    multiSamples(multiSamples),
    framebuffer(0),
    textureColourBuffer(0),
    depthStencilBuffer(0),
    screenBuffer(0),
    screenTexture(0){
    this->size = size;
    this->multiSamples = multiSamples;
    cout << "Creating the framebuffer" << endl;
    unsigned int temp;
    glGenFramebuffers(1, &temp);
    glBindFramebuffer(GL_FRAMEBUFFER, temp);

    // Generate the texture colour buffer and attach it to the framebuffer
    glGenTextures(1, &textureColourBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColourBuffer);
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        multiSamples,
        GL_RGB,
        size.x,
        size.y,
        GL_TRUE
    );
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        textureColourBuffer,
        0
    );
    // Create a render buffer object that will store the depth and stencil attachments
    glGenRenderbuffers(1, &depthStencilBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        multiSamples,
        GL_DEPTH24_STENCIL8,
        size.x,
        size.y
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        depthStencilBuffer
    );

    // Ensure that the multi-sample framebuffer is set up correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    }

    // Configure the screen buffer (This will also allow for potential post-processing)
    glGenFramebuffers(1, &screenBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer);
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size.x,
        size.y,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        screenTexture,
        0
    );

    // Ensure that the screen framebuffer is set up correctly
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Screen framebuffer is not complete!" << endl;
    }
    // Bind the default framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    cout << "Framebuffer created" << endl;
}

/**
 * @brief Destroy the Framebuffer object
 * 
 * @details This destructor will destroy the framebuffer object. This allows default distruction
 * 
 */
Framebuffer::~Framebuffer(){
    
}

/**
 * @brief This function will bind the multi-sample framebuffer
 * 
 * @details This function will bind the multi-sample framebuffer. This is used to control the
 * target framebuffer.
 * 
 * @return void
 */
void Framebuffer::bindMultiSample(){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

/**
 * @brief This function will unbind the multi-sample framebuffer
 * 
 * @details This function will unbind the multi-sample framebuffer. This is used to control the
 * target framebuffer.
 * 
 * @return void
 */
void Framebuffer::unbindMultiSample(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will bind the screen framebuffer
 * 
 * @details This function will bind the screen framebuffer. This is used to control the
 * target framebuffer.
 * 
 * @return void
 */
void Framebuffer::bindScreen(){
    glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer);
}

/**
 * @brief This function will unbind the screen framebuffer
 * 
 * @details This function will unbind the screen framebuffer. This is used to control the
 * target framebuffer.
 * 
 * @return void
 */
void Framebuffer::unbindScreen(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will clear the multi-sample framebuffer
 * 
 * @details This function ensures that the framebuffer is cleared and ready for rendering the
 * next frame.
 * 
 * @return void
 */
void Framebuffer::clearMultiSample(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

/**
 * @brief This function will clear the screen framebuffer
 * 
 * @details This function ensures that the framebuffer is cleared and ready for rendering the
 * next frame.
 * 
 * @return void
 */
void Framebuffer::clearScreen(){
    glClear(GL_COLOR_BUFFER_BIT);
}

/**
 * @brief This function will set the framebuffer to the given framebuffer
 * 
 * @details This function will set the framebuffer to the given framebuffer. This is used to
 * control the target framebuffer.
 * 
 * @param framebuffer [in] unsigned int The framebuffer to set
 * 
 * @return void
 */
void Framebuffer::setFramebuffer(unsigned int framebuffer){
    // Delete the old framebuffer
    glDeleteFramebuffers(1, &this->framebuffer);
    // Set the new framebuffer
    this->framebuffer = framebuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        textureColourBuffer,
        0
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        depthStencilBuffer
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete - changed framebuffer" << endl;
    }
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will set the texture colour buffer to the given texture colour buffer
 * 
 * @details This function will bind the texture colour buffer to current framebuffer.
 * 
 * @param textureColourBuffer [in] unsigned int The texture colour buffer to set
 * 
 * @return void
 */
void Framebuffer::setTextureColourBuffer(unsigned int textureColourBuffer){
    // Delete the old texture colour buffer
    glDeleteTextures(1, &this->textureColourBuffer);
    // Set the new texture colour buffer
    this->textureColourBuffer = textureColourBuffer;

    // Bind the framebuffer
    bindMultiSample();

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->textureColourBuffer);
    glTexImage2DMultisample(
        GL_TEXTURE_2D_MULTISAMPLE,
        multiSamples,
        GL_RGB,
        size.x,
        size.y,
        GL_TRUE
    );
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D_MULTISAMPLE,
        this->textureColourBuffer,
        0
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete - changed texture buffer" << endl;
    }
    // Unbind the framebuffer
    unbindMultiSample();
}

/**
 * @brief This function will set the depth stencil buffer to the given depth stencil buffer
 * 
 * @details This function will bind the depth stencil buffer to current framebuffer.
 * 
 * @param newDepthStencilBuffer [in] unsigned int The depth stencil buffer to set
 * 
 * @return void
 */
void Framebuffer::setDepthStencilBuffer(unsigned int newDepthStencilBuffer){
    // Delete the old depth buffer
    glDeleteRenderbuffers(1, &this->depthStencilBuffer);
    // Set the new depth buffer
    this->depthStencilBuffer = newDepthStencilBuffer;

    // Bind the framebuffer
    bindMultiSample();

    glBindRenderbuffer(GL_RENDERBUFFER, this->depthStencilBuffer);
    glRenderbufferStorageMultisample(
        GL_RENDERBUFFER,
        multiSamples,
        GL_DEPTH24_STENCIL8,
        size.x,
        size.y
    );
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER,
        GL_DEPTH_STENCIL_ATTACHMENT,
        GL_RENDERBUFFER,
        this->depthStencilBuffer
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete - changed renderOBj buffer" << endl;
    }
    // Unbind the framebuffer
    unbindMultiSample();
}

/**
 * @brief This function will set the screen buffer to the framebuffer
 * 
 * @param screenBuffer [in] unsigned int The screen buffer to set
 * 
 * @return void
 * 
 */
void Framebuffer::setScreenBuffer(unsigned int screenBuffer){
    // Delete the old screen buffer
    glDeleteFramebuffers(1, &this->screenBuffer);
    // Set the new screen buffer
    this->screenBuffer = screenBuffer;

    glBindFramebuffer(GL_FRAMEBUFFER, this->screenBuffer);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        screenTexture,
        0
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Screen framebuffer is not complete - changed screen buffer" << endl;
    }
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will set the screen texture to the framebuffer
 * 
 * @param screenTexture [in] unsigned int The screen texture to set
 * 
 * @return void
 * 
 */
void Framebuffer::setScreenTexture(unsigned int screenTexture){
    // Delete the old screen texture
    glDeleteTextures(1, &this->screenTexture);
    // Set the new screen texture
    this->screenTexture = screenTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, screenBuffer);
    glBindTexture(GL_TEXTURE_2D, this->screenTexture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        size.x,
        size.y,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER,
        GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D,
        this->screenTexture,
        0
    );
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        cerr << "ERROR::FRAMEBUFFER:: Screen framebuffer is not complete - changed screen texture" << endl;
    }
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will blit the multi-sample framebuffer to the screen framebuffer
 * 
 * @return void
 */
void Framebuffer::blitMultiToScreen(){
    glBindFramebuffer(GL_READ_FRAMEBUFFER, framebuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, screenBuffer);
    glBlitFramebuffer(
        0, 0, size.x, size.y,
        0, 0, size.x, size.y,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
    );
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
/**
 * @brief This callback function is called when the framebuffer size changes
 * 
 * @details This function will set the viewport to the new size. This is used to control the
 * target framebuffer.
 * 
 * @param window [in] GLFWwindow* The window object
 * @param width [in] int The width of the viewport
 * @param height [in] int The height of the viewport
 * 
 * @return void
 * 
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Framebuffer::framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
#pragma GCC diagnostic pop
