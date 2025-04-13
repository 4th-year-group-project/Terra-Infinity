#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <omp.h>

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
#include "Cube.hpp"
#include "Triangle.hpp"
#include "Utility.hpp"
#include "Terrain.hpp"
#include "World.hpp"
#include "Axes.hpp"
#include "Sun.hpp"
#include "UI.hpp"

void error_callback(int error, const char* description) {
    std::cerr << "Error " << error <<": " << description << std::endl;
}

unique_ptr<Renderer> renderer;

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

    // Set the number of threads to use for OpenMP
    omp_set_num_threads(omp_get_num_procs());

    try
    {
        // Create the Settings object
        Settings settings = Settings(
            // Full HD
            1920, // The width of the window
            1080, // The height of the window
            // Department machines
            // 2560, // The width of the window
            // 1440, // The height of the window
            700, // The width of the UI menu 
            true, // Whether the window is fullscreen or not
            8, // The render distance in chunks of the renderer
            1024, // The size of the chunks in the world
            32, // The size of the subchunks in the world
            8, // The largest resolution of a subchunk
            '/', // The delimitter for the file paths,
            256.0f, // The maximum height of the terrain
            0.2f, // The sea level of the terrain,
            1536.0f, // The distance that the player can request chunks
            UIPage::Home, // The current page of the UI
            "", // The current world that is being rendered (Initially empty to signal default world)
            make_shared<Parameters>(Parameters()) // The parameters for the terrain generation (Initially default parameters)
        );
        std::cout << "Settings created" << std::endl;
        // Create the Window object
        Window window = Window(
            settings.getWindowWidth(),
            settings.getWindowHeight(),
            "Prism",
            true
        );
        // Create the Player object
        glm::vec3 playerPosition = glm::vec3(0.0f, 80.0f, 0.0f);
        Camera camera = Camera(
            playerPosition + glm::vec3(1.68f, 0.2f, 0.2f),
            glm::vec2(settings.getWindowWidth()-settings.getUIWidth(), settings.getWindowHeight())
        );
        Cursor cursor = Cursor(settings);
        Player player = Player(
            make_shared<Camera>(camera),
            make_shared<Cursor>(cursor),
            playerPosition,
            glm::vec3(1.8f, 0.4f, 0.4f),
            0
        );
        // Set the cursor position
        // print the cursor position
        double xpos, ypos;
        // glfwGetCursorPos(window.getWindow(), &xpos, &ypos);
        // cout << "Before Setting Cursor position: " << xpos << ", " << ypos << endl;
        // player.getCursor()->setStartPosition(&window);
        glfwGetCursorPos(window.getWindow(), &xpos, &ypos);
        // cout << "After Setting Cursor position: " << xpos << ", " << ypos << endl;
        // std::cout << "Player created" << std::endl;
        shared_ptr<Player> playerPtr = make_shared<Player>(player);
        // Create the Framebuffer object
        Framebuffer framebuffer = Framebuffer(
            glm::vec2(settings.getWindowWidth() - settings.getUIWidth(), settings.getWindowHeight()),
            4
        );
        
        std::cout << "Framebuffer created" << std::endl;

        // Create the screen object
        Screen screen = Screen(framebuffer.getScreenTexture(), make_shared<Settings>(settings));
        cout << "Screen shader id: " << screen.getShader()->getId() << endl;

        UI ui = UI(window.getWindow()); // Create the UI object
        std::cout << "UI created" << std::endl;

        // Create the Renderer object
        renderer = make_unique<Renderer>(
            make_shared<Window>(window),
            make_shared<Settings>(settings),
            playerPtr,
            make_shared<Framebuffer>(framebuffer),
            make_shared<UI>(ui),
            make_unique<Screen>(screen)
        );

        // // We are creating a triangle
        // Triangle triangle = Triangle(make_shared<Settings>(settings));
        // cout << "Triangle created" << endl;
        // renderer->addObject(make_shared<Triangle>(triangle));

        // Cube cube = Cube(make_shared<Settings>(settings));
        // cout << "Cube created" << endl;
        // renderer->addObject(make_shared<Cube>(cube));

        Sun sun = Sun(
            glm::vec3(0.0f, 500.0f, 0.0f), // pos
            glm::vec3(1.0f, 1.0f, 1.0f), // colour
            glm::vec3(0.2f, 0.2f, 0.2f), // ambient component 
            glm::vec3(0.5f, 0.5f, 0.5f), // diffuse 
            glm::vec3(1.0f, 1.0f, 1.0f), // specular
            5.0f, // radius
            settings
        );
        renderer->addLight(make_shared<Sun>(sun));


        Axes axes = Axes(settings);
        cout << "Axes created" << endl;
        renderer->addObject(make_shared<Axes>(axes));

        // // We are going to create a world object
        World world = World(settings, playerPtr);
        cout << "World created" << endl;
        renderer->addObject(make_shared<World>(world));

        printf("Renderer created\n");
        renderer->run();
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
void windowsFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void windowsMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    glm::vec2 newMousePos = glm::vec2(xpos, ypos);

    if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuClosed) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glm::vec2 mouseOffset = renderer->getPlayer()->getCursor()->processMouseMovement(newMousePos, window);
        renderer->getPlayer()->getCamera()->processMouseMovement(newMousePos, mouseOffset, width, height);
    }
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void windowsScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuClosed) {
        renderer->getPlayer()->getCamera()->processMouseScroll(yoffset);
    }
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}


void windowsKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {  // Detects only first press, ignores repeats
        if (key == GLFW_KEY_TAB) {
            if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuOpen) {
                renderer->getSettings()->setCurrentPage(UIPage::WorldMenuClosed);
            } else {
                renderer->getSettings()->setCurrentPage(UIPage::WorldMenuOpen);
            }
        }
    }
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}

#pragma GCC diagnostic pop