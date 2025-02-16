/*
    This is the header file for the windows main file. It contains any global callback
    functions that are required for the renderer to run on Windows. This is a hacky soluation to
    get around the problem of not being able to pass member functions to GLFW callbacks.
*/
#ifndef WINDOWSMAIN_HPP
#define WINDOWSMAIN_HPP


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
void windowsFramebufferSizeCallback(GLFWwindow* window, int width, int height);
void windowsMouseCallback(GLFWwindow* window, double xpos, double ypos);
void windowsScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

int main(int argc, char** argv);

#endif // WINDOWSMAIN_HPP