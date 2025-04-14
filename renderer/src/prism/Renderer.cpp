/*
    This class contains the main renderer for the application. It is responsible for managing all
    of the resources that will be required from the framebuffer to the camera, along with the
    list of objects that will need to be rendered in the scene.
*/
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <omp.h>

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
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

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

Renderer::~Renderer(){
    printf("Shutting down the renderer\n");
    // Clear all of the shared pointers
    window.reset();
    settings.reset();
    player.reset();
    framebuffer.reset();
    objects.clear();
    ui.reset();
    screen.reset();
}

void Renderer::setCallbackFunctions(){
// We want an ifdef here to determine if we are on Windows or Linux
#ifdef _WIN32
    cout << "Setting the windows callback functions" << endl;
    window->setFramebufferSizeCallback(windowsFramebufferSizeCallback);
    window->setCursorPosCallback(windowsMouseCallback);
    window->setScrollCallback(windowsScrollCallback);
    window->setKeyCallback(windowsKeyCallback);
#else
    cout << "Setting the linux callback functions: Framebuffer" << endl;
    window->setFramebufferSizeCallback(linuxFramebufferSizeCallback);
    cout << "Setting the linux callback functions: Mouse" << endl;
    window->setCursorPosCallback(linuxMouseCallback);
    cout << "Setting the linux callback functions: Scroll" << endl;
    window->setScrollCallback(linuxScrollCallback);
    cout << "Setting the linux callback functions: Key" << endl;
    window->setKeyCallback(linuxKeyCallback);
#endif
    return;
}


void Renderer::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
){
    // cout << "================================================================" << endl;

    // We are going to print the current bound mouse callback function using glfw

    // If the q key is pressed we are going to render in wireframe mode
    if (glfwGetKey(window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS){
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // cout << "FPS: " << 1.0f / deltaTime << endl;

    player->processKeyBoardInput(window, deltaTime);

    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);

    // Renderer the lights
    for (shared_ptr<Light> light : this->lights){
        light->render(view, projection, lights, viewPos);
    }
    for (const unique_ptr<IRenderable>& object : objects){
        object->render(view, projection, lights, viewPos);
    }

    // Render the UI side panel
    ui->render(settings, 1.0f / deltaTime, player->getPosition());

    // If the UI is shown then disable edge scrolling if it is active 
    if (settings->getCurrentPage() == UIPage::WorldMenuOpen){
        player->getCamera()->setOnTopEdge(false);
        player->getCamera()->setOnBottomEdge(false);
        player->getCamera()->setOnLeftEdge(false);
        player->getCamera()->setOnRightEdge(false);
    }

    // Save the framebuffer to an image
    // cv::Mat image = cv::Mat(1080, 1920, CV_8UC3);
    // glReadPixels(0, 0, 1920, 1080, GL_BGR, GL_UNSIGNED_BYTE, image.data);
    // cv::imwrite("screenshot.png", image);

    player->getCamera()->setScreenDimensions(glm::vec2(settings->getWindowWidth(), settings->getWindowHeight()));

    player->getCamera()->checkCameraConstraints();

    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
    // return



    // Bind the framebuffer
    // framebuffer->bindMultiSample();

    // // Clear the screen
    // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // // Process the time between frames
    // currentFrame = static_cast<float>(glfwGetTime());
    // deltaTime = currentFrame - lastFrame;
    // lastFrame = currentFrame;

    // cout << "FPS: " << 1.0f / deltaTime << endl;

    // // Process the player's input
    // player->processKeyBoardInput(window, deltaTime);

    // // Update all of the objects in the scene
    // for (shared_ptr<IRenderable> object : objects){
    //     object->updateData();
    // }

    // glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glDepthFunc(GL_LEQUAL);

    // // Render the lights
    // for (shared_ptr<Light> light : lights){
    //     light->render(view, projection, lights, viewPos);
    // }

    // // Render all of the objects in the scene
    // for (shared_ptr<IRenderable> object : objects){
    //     object->render(view, projection, lights, viewPos);
    // }

    // player->getCamera()->checkCameraConstraints();

    // // Swap the buffers using blit
    // framebuffer->blitMultiToScreen();


    // // Unbind the framebuffer (Sets the framebuffer back to the default framebuffer)
    // framebuffer->unbindScreen();

    // // Always set the render mode to fill
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // // We are now using the screen texture to render to the screen
    // screen->render(view, projection, lights, viewPos);
    // glfwSwapBuffers(window->getWindow());
    // glfwPollEvents();
}


void Renderer::renderHomepage(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // cout << "FPS: " << 1.0f / deltaTime << endl;

    player->processKeyBoardInput(window, deltaTime);

    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ui->renderHomepage(settings);

    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
}

void Renderer::renderLoading(){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ui->renderLoadingScreen(settings);
    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
}


void Renderer::setupData(){
    for (shared_ptr<Light> light : lights){
        light->setupData();
    }
    // Loop through all of the objects and set up their data
    for (const unique_ptr<IRenderable>& object : objects){
        object->setupData();
    }
}

void Renderer::updateData(){
    // double start = omp_get_wtime();
    for (shared_ptr<Light> light : lights){
        light->updateData();
    }
    // Loop through all of the objects and update their data
    for (const unique_ptr<IRenderable>& object : objects){
        object->updateData();
    }
}

void Renderer::addObject(unique_ptr<IRenderable> object){
    // Add an object to the list of objects
    objects.push_back(move(object));
}

void Renderer::addLight(shared_ptr<Light> light){
    // Add a light to the list of lights
    lights.push_back(light);
}

int Renderer::run(){
    // This does nothing for now but it will be our main renderer loop
    setupData();
    while (!glfwWindowShouldClose(window->getWindow())){
        if (settings->getCurrentPage() == UIPage::Loading){
            renderLoading();
        } else if (settings->getCurrentPage() == UIPage::Home){
            renderHomepage();
        } else {
            updateData();
            render(
                player->getCamera()->getViewMatrix(),
                player->getCamera()->getProjectionMatrix(),
                this->lights,
                player->getCamera()->getPosition()
            );
        }
    }
    return 0;
}