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
#include "Window.hpp"
#include "Settings.hpp"
#include "Player.hpp"
#include "Framebuffer.hpp"
#include "Camera.hpp"
#include "Cursor.hpp"
#include "LinuxMain.hpp"

void error_callback(int error, const char* description) {
    std::cerr << "Error " << error <<": " << description << std::endl;
}

Renderer renderer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
int main(int argc, char** argv){
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    cout << "Monitor: " << monitor << endl;
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    cout << "Monitor width: " << mode->width << " Monitor height: " << mode->height << endl;
    // A simple controller program for the renderer that is compiled on Windows
    std::cout << "Hello, World!" << std::endl;
    try
    {
        // Create the Settings object
        Settings settings = Settings(
            1920, // The width of the window
            1080, // The height of the window
            true, // Whether the window is fullscreen or not
            16, // The render distance in chunks of the renderer
            1024, // The size of the chunks in the world
            32, // The size of the subchunks in the world
            2, // The resolution of the subchunks in the world
            '/' // The delimitter for the file paths
        );
        std::cout << "Settings created" << std::endl;
        // Create the Window object
        Window window = Window(
            settings.getWindowWidth(),
            settings.getWindowHeight(),
            "Prism",
            true
        );
        std::cout << "Window created" << std::endl;
        // Create the Player object
        glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        Player player = Player(
            Camera(
                playerPosition + glm::vec3(1.68f, 0.2f, 0.2f),
                glm::vec2(1920, 1080)
            ),
            Cursor(&window),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(1.8f, 0.4f, 0.4f),
            0
        );
        std::cout << "Player created" << std::endl;
        // Create the Framebuffer object
        Framebuffer framebuffer = Framebuffer(
            glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()),
            4
        );
        std::cout << "Framebuffer created" << std::endl;
        // Create the Renderer object
        renderer = Renderer(
            make_shared<Window>(window),
            make_shared<Settings>(settings),
            make_shared<Player>(player),
            make_shared<Framebuffer>(framebuffer)
        );

        printf("Renderer created\n");
        renderer.run();
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
#pragma GCC diagnostic ignored "-Wunused-parameter"
void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // This is a placeholder function
    glm::vec2 newMousePos = glm::vec2(xpos, ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glm::vec2 mouseOffset = renderer.getPlayer()->getCursor().processMouseMovement(newMousePos, window);
    renderer.getPlayer()->getCamera().processMouseMovement(newMousePos, mouseOffset, width, height);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    renderer.getPlayer()->getCamera().processMouseScroll(yoffset);
}
#pragma GCC diagnostic pop