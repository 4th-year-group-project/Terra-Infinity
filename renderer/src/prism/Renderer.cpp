/*
    This class contains the main renderer for the application. It is responsible for managing all
    of the resources that will be required from the framebuffer to the camera, along with the
    list of objects that will need to be rendered in the scene.
*/
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <GLFW/glfw3.h>
#endif

#include "Window.hpp"
#include "Settings.hpp"
#include "Player.hpp"
// #include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Cursor.hpp"
// #ifdef WINDOWS_BUILD
//     #include "WindowsMain.hpp"
// #else
//     #include "LinuxMain.hpp"
// #endif

using namespace std;

Renderer::Renderer(){
    cout << "Creating the renderer" << endl;
    try {
        cout << "Creating the player" << endl;
        player = Player();
    } catch (exception &e){
        cout << "ERROR::RENDERER::FAILED_TO_CREATE_PLAYER: " << e.what() << endl;
    }
//     // Create the settings class
//     // We will get the monitor width and height
//     GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//     const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//     int monitorWidth = mode->width;
//     int monitorHeight = mode->height;
//     char filePathDelimitter = '/';
// #ifdef _WIN32
//     filePathDelimitter = '\\';
// #endif
//     Settings settings(
//         monitorWidth, // The width of the window
//         monitorHeight, // The height of the window
//         true, // Whether the window is fullscreen or not
//         16, // The render distance in chunks of the renderer
//         1024, // The size of the chunks in the world
//         32, // The size of the subchunks in the world
//         2, // The resolution of the subchunks in the world
//         filePathDelimitter // The delimitter for the file paths
//     );
//     this->settings = settings;

//     // Create the window class
//     Window window = Window(
//         settings.getWindowWidth(),
//         settings.getWindowHeight(),
//         "Prism",
//         true
//     );
//     this->window = window;

//     // Create a Camera object
//     glm::vec3 playerPosition(0.0f, 0.0f, 0.0f);
//     // Create a Player object
//     Player player = Player(
//         Camera(
//             // We need to add the offset to the camera position from the player position
//             playerPosition + glm::vec3(1.68f, 0.2f, 0.2f),
//             glm::vec2(settings.getWindowWidth(), settings.getWindowHeight())
//         ), // The camera that the player will use
//         Cursor(&window), // The cursor that the player will use
//         glm::vec3(0.0f, 0.0f, 0.0f), // The origin of the player (bottom left corner)
//         glm::vec3(1.8f, 0.4f, 0.4f), // The size of the player
//         0 // The mode of the player
//     );
//     this->player = player;

//     // Create a Framebuffer object
//     Framebuffer framebuffer = Framebuffer(
//         glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()),
//         4
//     );
//     this->framebuffer = framebuffer;

//     // Attach callback functions to the window
//     // setCallbackFunctions();
}

Renderer::~Renderer(){
    // Nothing to do here
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::render(glm::mat4 view, glm::mat4 projection){
}

void Renderer::setupData(){
}

void Renderer::updateData(){
}
#pragma GCC diagnostic pop

// void Renderer::setCallbackFunctions(){
// // // We want an ifdef here to determine if we are on Windows or Linux
// // #ifdef _WIN32
// //     window.setFramebufferSizeCallback(windowsFramebufferSizeCallback);
// //     window.setCursorPosCallback(windowsMouseCallback);
// //     window.setScrollCallback(windowsScrollCallback);
// // #else
// //     window.setFramebufferSizeCallback(linuxFramebufferSizeCallback);
// //     window.setCursorPosCallback(linuxMouseCallback);
// //     window.setScrollCallback(linuxScrollCallback);
// // #endif
//     return;
// }



// Renderer::Renderer(Window window, Settings settings, Player player, Framebuffer framebuffer)
//     : window(window), settings(settings){
//     this->player = player;
//     this->framebuffer = framebuffer;
// }

// Renderer::~Renderer(){
//     // Nothing to do here
// }

// void Renderer::render(glm::mat4 view, glm::mat4 projection){
//     // Bind the framebuffer
//     framebuffer.bindMultiSample();
//     // Clear the screen
//     glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//     // Process the time between frames
//     currentFrame = static_cast<float>(glfwGetTime());
//     deltaTime = currentFrame - lastFrame;
//     lastFrame = currentFrame;

//     // Process the player's input
//     player.processKeyBoardInput(&window, deltaTime);

//     // Update all of the objects in the scene
//     for (IRenderable *object : objects){
//         object->updateData();
//     }

//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_CULL_FACE);
//     glDepthFunc(GL_LEQUAL);

//     // Render all of the objects in the scene
//     for (IRenderable *object : objects){
//         object->render(view, projection);
//     }

//     player.getCamera().checkCameraConstraints();

//     // Swap the buffers using blit
//     framebuffer.blitMultiToScreen();
//     // Unbind the framebuffer
//     framebuffer.unbindScreen();
//     glfwSwapBuffers(window.getWindow());
//     glfwPollEvents();
// }

// void Renderer::setupData(){
//     // Loop through all of the objects and set up their data
//     for (IRenderable *object : objects){
//         object->setupData();
//     }
// }

// void Renderer::updateData(){
//     // Loop through all of the objects and update their data
//     for (IRenderable *object : objects){
//         object->updateData();
//     }
// }

// void Renderer::addObject(IRenderable *object){
//     objects.push_back(object);
// }

// int Renderer::run(){
//     // This does nothing for now but it will be our main renderer loop
//     setupData();
//     while (!glfwWindowShouldClose(window.getWindow())){
//         render(player.getCamera().getViewMatrix(), player.getCamera().getProjectionMatrix());
//         updateData();
//     }
//     return 0;
// }