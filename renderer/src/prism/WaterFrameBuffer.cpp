/**
 * @file WaterFrameBuffer.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the WaterFrameBuffer class.
 * @version 1.0
 * @date 2025
 * 
 */
#include <vector>
#include <iostream>
#include <string>
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

#include "WaterFrameBuffer.hpp"

/**
 * @brief Construct a new WaterFrameBuffer object with the given size
 * 
 * @details This constructor will create a framebuffer object with the given size. It will create
 * a colour texture and a depth texture. The colour texture will be used to store the colour of the
 * water and the depth texture will be used to store the depth of the water. The framebuffer
 * will be used to render the water to a texture that can be used in the shader. 
 * 
 * @param size [in] glm::vec2 The size of the framebuffer
 * 
 */
WaterFrameBuffer::WaterFrameBuffer(glm::vec2 size):
    size(size)
{
    // Generate the framebuffer
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // Create the colour texture
    glGenTextures(1, &colourTexture);
    glBindTexture(GL_TEXTURE_2D, colourTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colourTexture, 0);

    // Create the depth texture
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);


    // Check if the framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "ERROR::WATERFRAMEBUFFER:: WATER Framebuffer is not complete!" << std::endl;
    }
    
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    std::cout << "Water Framebuffer created" << std::endl;
}

/**
 * @brief This function will bind the framebuffer to the current active framebuffer
 * 
 * @return void
 */
void WaterFrameBuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

/**
 * @brief This function will unbind the framebuffer from the current active framebuffer
 * 
 * @return void
 */
void WaterFrameBuffer::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/**
 * @brief This function will clear the framebuffer and its attachments of their contents
 * 
 * @return void
 */
void WaterFrameBuffer::clear(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
