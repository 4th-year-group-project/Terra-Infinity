/**
 * @file Globals.hpp
 * @author King Attalus II
 * @brief This file contains the global variables and functions for the renderer.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef GLOBALS_HPP
#define GLOBALS_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glfw/glfw3.h"
#else
    #include <glad/glad.h>
    #include <GLFW/glfw3.h>
#endif



extern void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos);
extern void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
extern void linuxKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height);

extern void windowsMouseCallback(GLFWwindow* window, double xpos, double ypos);
extern void windowsScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
extern void windowsKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
extern void windowsFramebufferSizeCallback(GLFWwindow* window, int width, int height);

#endif // GLOBALS_HPP
