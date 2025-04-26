/**
 * @file WindowsMain.hpp
 * @author King Attalus II
 * @brief This file contains the main function for the Windows version of the application. It initializes the
 * GLFW window, sets up the OpenGL context, and handles the main rendering loop.
 * @version 1.0
 * @date 2025
 * @date 2025-04-26
 *
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
void windowsKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main(int argc, char** argv);

#endif // WINDOWSMAIN_HPP
