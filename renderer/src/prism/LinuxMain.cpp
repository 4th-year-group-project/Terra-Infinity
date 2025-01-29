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

#include "Renderer.hpp"
#include "LinuxMain.hpp"



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char** argv){
    // A simple controller program for the renderer that is compiled on Windows
    std::cout << "Hello, World!" << std::endl;
    try
    {
        //crashing here
        Renderer renderer = Renderer();
        printf("Renderer created\n");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // Initialize the settings for the renderer
    // renderer = Renderer();
    // renderer.run();
    return 0;
}
#pragma GCC diagnostic pop

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height)
// {
//     glViewport(0, 0, width, height);
// }
// #pragma GCC diagnostic pop

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos)
// {
//     // This is a placeholder function
//     glm::vec2 newMousePos = glm::vec2(xpos, ypos);
//     int width, height;
//     glfwGetWindowSize(window, &width, &height);
//     glm::vec2 mouseOffset = renderer.getPlayer().getCursor().processMouseMovement(newMousePos, window);
//     renderer.getPlayer().getCamera().processMouseMovement(newMousePos, mouseOffset, width, height);
// }
// #pragma GCC diagnostic pop

// #pragma GCC diagnostic push
// #pragma GCC diagnostic ignored "-Wunused-parameter"
// void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
// {
//     renderer.getPlayer().getCamera().processMouseScroll(yoffset);
// }
// #pragma GCC diagnostic pop