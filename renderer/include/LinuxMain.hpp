/*
    This is the header file for the linux main file. It contains any global callback
    functions that are required for the renderer to run on Linux. This is a hacky soluation to
    get around the problem of not being able to pass member functions to GLFW callbacks.
*/
#ifndef LINUXMAIN_HPP
#define LINUXMAIN_HPP


#include <iostream>

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

// Function prototypes
void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height);
void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos);
void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset);


#endif // LINUXMAIN_HPP