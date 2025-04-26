/**
 * @file LinuxMain.hpp
 * @author King Attalus II
 * @brief This file contains the function prototypes for the Linux main functions.
 * @version 1.0
 * @date 2025
 *
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
void linuxKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif // LINUXMAIN_HPP
