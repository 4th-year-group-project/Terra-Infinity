#ifndef WATERFRAMEBUFFER_HPP
#define WATERFRAMEBUFFER_HPP

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


class WaterFrameBuffer {
private:
    glm::vec2 size; // The size of the framebuffer
    unsigned int framebuffer; // The ID of the framebuffer
    unsigned int colourTexture; // The color texture of the framebuffer
    unsigned int depthTexture; // The depth texture of the framebuffer

public:
    WaterFrameBuffer(glm::vec2 size);
    WaterFrameBuffer(){};

    ~WaterFrameBuffer();

    glm::vec2 getSize(){return size;}
    unsigned int getFramebuffer(){return framebuffer;}
    unsigned int getColourTexture(){return colourTexture;}
    unsigned int getDepthTexture(){return depthTexture;}

    void bind();
    void unbind();
    void clear();
};



#endif // WATERFRAMEBUFFER_HPP
