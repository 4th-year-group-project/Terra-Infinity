#include <iostream>
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

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>

// Mock implementations of the Linux callback functions
void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    // Test mock implementation
    std::cout << "Mock framebuffer size callback called" << std::endl;
}

void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    // Test mock implementation
    std::cout << "Mock mouse callback called" << std::endl;
}

void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    // Test mock implementation
    std::cout << "Mock scroll callback called" << std::endl;
}

void linuxKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Test mock implementation
    std::cout << "Mock key callback called" << std::endl;
}