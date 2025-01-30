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
#include "Framebuffer.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Cursor.hpp"
#ifdef WINDOWS_BUILD
    #include "WindowsMain.hpp"
#else
    #include "LinuxMain.hpp"
#endif

using namespace std;

Renderer::Renderer(){
    cout << "Creating the renderer setup" << endl;
//     glm::vec3 playerPosition = glm::vec3(0.0f, 0.0f, 0.0f);
//     int monitorWidth = 1920;
//     int monitorHeight = 1080;
//     bool fullscreen = true;
//     int renderDistance = 16;
//     int chunkSize = 1024;
//     int subChunkSize = 32;
//     int subChunkResolution = 2;
//     char filePathDelimitter = '/';
// #ifdef _WIN32
//     filePathDelimitter = '\\';
// #endif
//     // Initialise glfw incase 
//     if (!glfwInit()) {
//         cerr << "Failed to initialize GLFW" << endl;
//         exit(1);
//     }
//     Settings tempSettings = Settings(
//         monitorWidth,
//         monitorHeight,
//         fullscreen,
//         renderDistance,
//         chunkSize,
//         subChunkSize,
//         subChunkResolution,
//         filePathDelimitter
//     );
//     Window tempWindow = Window(
//         tempSettings.getWindowWidth(),
//         tempSettings.getWindowHeight(),
//         "Prism",
//         true
//     );
//     Player tempPlayer = Player(
//         Camera(
//             playerPosition + glm::vec3(1.68f, 0.2f, 0.2f),
//             glm::vec2(1920, 1080)
//         ),
//         Cursor(&tempWindow),
//         glm::vec3(0.0f, 0.0f, 0.0f),
//         glm::vec3(1.8f, 0.4f, 0.4f),
//         0
//     );
//     player = make_shared<Player>(tempPlayer);
//     settings = make_shared<Settings>(tempSettings);
//     window = make_shared<Window>(tempWindow);
//     cout << "Creating the framebuffer" << endl;
//     Framebuffer tempFramebuffer = Framebuffer(
//         glm::vec2(tempSettings.getWindowWidth(), tempSettings.getWindowHeight()),
//         4
//     );
//     framebuffer = make_shared<Framebuffer>(tempFramebuffer);
//     objects = list<shared_ptr<IRenderable>>();
//     setCallbackFunctions();
}

Renderer::~Renderer(){
    // Clear all of the shared pointers
    window.reset();
    settings.reset();
    player.reset();
    framebuffer.reset();
}

void Renderer::setCallbackFunctions(){
// We want an ifdef here to determine if we are on Windows or Linux
#ifdef _WIN32
    window->setFramebufferSizeCallback(windowsFramebufferSizeCallback);
    window->setCursorPosCallback(windowsMouseCallback);
    window->setScrollCallback(windowsScrollCallback);
#else
    window->setFramebufferSizeCallback(linuxFramebufferSizeCallback);
    window->setCursorPosCallback(linuxMouseCallback);
    window->setScrollCallback(linuxScrollCallback);
#endif
    return;
}


void Renderer::render(glm::mat4 view, glm::mat4 projection){
    // Bind the framebuffer
    framebuffer->bindMultiSample();
    // Clear the screen
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Process the time between frames
    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process the player's input
    player->processKeyBoardInput(window, deltaTime);

    // Update all of the objects in the scene
    for (shared_ptr<IRenderable> object : objects){
        object->updateData();
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    // Render all of the objects in the scene
    for (shared_ptr<IRenderable> object : objects){
        object->render(view, projection);
    }

    player->getCamera().checkCameraConstraints();

    // Swap the buffers using blit
    framebuffer->blitMultiToScreen();
    // Unbind the framebuffer
    framebuffer->unbindScreen();
    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
}

void Renderer::setupData(){
    // Loop through all of the objects and set up their data
    for (shared_ptr<IRenderable> object : objects){
        object->setupData();
    }
}

void Renderer::updateData(){
    // Loop through all of the objects and update their data
    for (shared_ptr<IRenderable> object : objects){
        object->updateData();
    }
}

void Renderer::addObject(shared_ptr<IRenderable> object){
    // Add an object to the list of objects
    objects.push_back(object);
}

int Renderer::run(){
    // This does nothing for now but it will be our main renderer loop
    setupData();
    while (!glfwWindowShouldClose(window->getWindow())){
        render(player->getCamera().getViewMatrix(), player->getCamera().getProjectionMatrix());
        updateData();
    }
    return 0;
}