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

WaterFrameBuffer::~WaterFrameBuffer(){
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &colourTexture);
    glDeleteTextures(1, &depthTexture);
}

void WaterFrameBuffer::bind(){
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
}

void WaterFrameBuffer::unbind(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void WaterFrameBuffer::clear(){
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
