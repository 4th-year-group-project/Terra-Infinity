/**
 * @file LinuxMain.cpp
 * @author King Attalus II
 * @brief This file contains the main function for the renderer application that is compiled on Linux.
 * @version 1.0
 * @date 2025
 * 
 */
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <omp.h>
#include <chrono>

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
#include "WaterFrameBuffer.hpp"
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
#include "Parameters.hpp"
#include "UI.hpp"

using namespace std::chrono;

/**
 * @brief A callback function for GLFW that will be called when an error occurs
 * 
 * @param error [in] int The error code
 * @param description [in] const char* The error description 
 * 
 * @return void
 * 
 */
void error_callback(int error, const char* description) {
    std::cerr << "Error " << error <<": " << description << std::endl;
}

unique_ptr<Renderer> renderer;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief The main entry point for the renderer application on Linux
 * 
 * @details This function will create all of the required objects for the renderer and the
 * scene to be rendered before entering the main rendering loop. The renderer settings 
 * 
 * @param argc [in] int The number of arguments
 * @param argv [in] char** The arguments passed as command line arguments
 * 
 * @return int The exit code of the application
 * 
 */
int main(int argc, char** argv){
    // Initialise GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Get the size of the display monitor 
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    // Set the number of threads to use for OpenMP, ensuring that the server will have at least 2 threads
    omp_set_num_threads(omp_get_num_procs() - 4); 

    // =================================================
    // Configurable renderer settings
    // =================================================
    int number_of_chunks = 20; // Set the render distance in chunks of the renderer
    bool use_1k_textures = true; // Set whether to use 1k textures or not
    // Create the Settings object
    Settings settings = Settings(
        mode->width, // The width of the window
        mode->height, // The height of the window
        700, // The width of the UI menu 
        true, // Whether the window is fullscreen or not
        number_of_chunks, // The render distance in chunks of the renderer
        1024, // The size of the chunks in the world
        32, // The size of the subchunks in the world
        10, // The largest resolution of a subchunk
        '/', // The delimitter for the file paths,
        256.0f, // The maximum height of the terrain
        0.2f, // The sea level of the terrain,
        1024.0f * 2.0, // The distance that the player can request chunks (multiplying by an arbitrary number to modify the request distance)
        UIPage::Home, // The current page/state of the UI
        "", // The current world that is being rendered (Initially empty to signal default world)
        make_shared<Parameters>(Parameters(use_1k_textures)), // The parameters for the terrain generation (Initially default parameters)
        // Fog settings
        (number_of_chunks - 3) * 32.0f, // The start distance of the fog
        (number_of_chunks) * 32.0f, // The end distance of the fog
        0.3f, // The density of the fog
        glm::vec3(1.0f, 1.0f, 1.0f), // The color of the fog
        use_1k_textures // Whether to use 1k textures or not, otherwise uses 2k textures
    );

    // Create the Window object
    Window window = Window(
        settings.getWindowWidth(),
        settings.getWindowHeight(),
        "TerraInfinity",
        true
    );

    // Set initial player position
    glm::vec3 playerPosition = glm::vec3(0.0f, 500.0f, 0.0f);

    // Create the Camera object
    Camera camera = Camera(
        playerPosition + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()),
        static_cast<float>((settings.getRenderDistance() -1.25) * settings.getSubChunkSize())
    );

    // Create the Cursor object
    Cursor cursor = Cursor(settings);

    // Create the Player object
    Player player = Player(
        make_shared<Camera>(camera),
        make_shared<Cursor>(cursor),
        playerPosition,
        glm::vec3(1.8f, 0.4f, 0.4f),
        0
    );

    // Create the Player shared pointer
    shared_ptr<Player> playerPtr = make_shared<Player>(player);

    // Create the Framebuffer object
    Framebuffer framebuffer = Framebuffer(
        glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()),
        4
    );

    // Create the reflection and refraction framebuffers
    WaterFrameBuffer reflectionBuffer = WaterFrameBuffer(glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()));
    WaterFrameBuffer refractionBuffer = WaterFrameBuffer(glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()));

    // Create the screen object
    Screen screen = Screen(framebuffer.getScreenTexture(), make_shared<Settings>(settings));

    // Create the UI object
    UI ui = UI(window.getWindow(), make_shared<Settings>(settings)); 

    // Create the main Renderer object
    renderer = make_unique<Renderer>(
        make_shared<Window>(window),
        make_shared<Settings>(settings),
        playerPtr,
        make_shared<Framebuffer>(framebuffer),
        make_shared<WaterFrameBuffer>(reflectionBuffer),
        make_shared<WaterFrameBuffer>(refractionBuffer),
        make_shared<UI>(ui),
        make_unique<Screen>(screen)
    );
    
    // Add a Sun object as a light source
    renderer->addLight(make_shared<Sun>(
        glm::vec3(0.0f, 500.0f, 0.0f), // pos
        glm::vec3(1.0f, 1.0f, 1.0f), // colour
        glm::vec3(0.2f, 0.2f, 0.2f), // ambient component
        glm::vec3(0.5f, 0.5f, 0.5f), // diffuse
        glm::vec3(1.0f, 1.0f, 1.0f), // specular
        5.0f, // radius
        settings
    ));

    // Create a World object
    renderer->addObject(make_unique<World>(
        make_shared<Settings>(settings),
        playerPtr,
        make_shared<WaterFrameBuffer>(reflectionBuffer),
        make_shared<WaterFrameBuffer>(refractionBuffer)
    ));

    // Run the renderer
    renderer->run();

    return 0;
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief This callback function is called when the framebuffer size changes
 * 
 * @param window [in] GLFWwindow* The window object
 * @param width [in] int The width of the framebuffer
 * @param height [in] int The height of the framebuffer
 * 
 * @return void
 * 
 */
void linuxFramebufferSizeCallback(GLFWwindow* window, int width, int height)
{ 
    // Set the viewport to the new size
    glViewport(0, 0, width, height);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief This callback function is called when the mouse is moved
 * 
 * @param window [in] GLFWwindow* The window object
 * @param xpos [in] double The x position of the mouse
 * @param ypos [in] double The y position of the mouse
 * 
 * @return void
 * 
 */
void linuxMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // Get the current mouse position
    glm::vec2 newMousePos = glm::vec2(xpos, ypos);

    // Only process mouse movement from within the renderer if the world menu is closed
    if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuClosed) {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glm::vec2 mouseOffset = renderer->getPlayer()->getCursor()->processMouseMovement(newMousePos, window);
        renderer->getPlayer()->getCamera()->processMouseMovement(newMousePos, mouseOffset, width, height);
    }

    // Forward the mouse event to ImGui
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief This callback function is called when the mouse scroll wheel is moved
 * 
 * @param window [in] GLFWwindow* The window object
 * @param xoffset [in] double The x offset of the scroll wheel
 * @param yoffset [in] double The y offset of the scroll wheel
 * 
 * @return void
 * 
 */
void linuxScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    // Only process mouse scroll from the renderer if the world menu is closed
    if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuClosed) {
        renderer->getPlayer()->getCamera()->processMouseScroll(yoffset);
    }

    // Forward the scroll event to ImGui
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}
#pragma GCC diagnostic pop

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief This callback function is called when a key is pressed
 * 
 * @param window [in] GLFWwindow* The window object
 * @param key [in] int The key that was pressed
 * @param scancode [in] int The scancode of the key
 * @param action [in] int The action that was performed
 * @param mods [in] int The modifiers that were pressed
 * 
 * @return void
 * 
 */
void linuxKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {  // Detects only first press, ignores repeats
        // If Tab is pressed, toggle the world menu open or closed
        if (key == GLFW_KEY_TAB) {
            if (renderer->getSettings()->getCurrentPage() == UIPage::WorldMenuOpen) {
                renderer->getSettings()->setCurrentPage(UIPage::WorldMenuClosed);
            } else {
                renderer->getSettings()->setCurrentPage(UIPage::WorldMenuOpen);
        }
    }
    }

    // Forward the key event to ImGui
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
}
#pragma GCC diagnostic pop