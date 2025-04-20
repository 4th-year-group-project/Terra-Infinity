/*
    This class contains the main renderer for the application. It is responsible for managing all
    of the resources that will be required from the framebuffer to the camera, along with the
    list of objects that will need to be rendered in the scene.
*/
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <omp.h>
#include <thread>
#include <atomic>

#ifdef DEPARTMENT_BUILD
#include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
#include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#include "/dcs/large/efogahlewem/.local/include/stb/stb_image_write.h"
#else
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include <stb/stb_image_write.h>
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
namespace fs = std::filesystem;


Renderer::~Renderer()
{
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

void Renderer::setCallbackFunctions()
{
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

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void Renderer::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane)
{
    // There are three different passes that the renderer will now need to complete
    // 1. The water reflection pass
    // 2. The water refraction pass
    // 3. The final render pass

    // Clearing the default framebuffer just in case
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Process player input at the very start and update the view matrix
    player->processKeyBoardInput(window, deltaTime);
    view = player->getCamera()->getViewMatrix();

    // Bind the reflection framebuffer
    reflectionBuffer->bind();
    // Clear the buffer
    reflectionBuffer->clear();


    // enable the depth testing along with culling
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CLIP_DISTANCE0);

    // invert the camera
    player->getCamera()->setInverted(settings);

    glm::mat4 reflectionView = player->getCamera()->getViewMatrix();
    // The project matrix is unchanged

    float waterHeight = settings->getSeaLevel() * settings->getMaximumHeight();
    plane = glm::vec4(0.0f, 1.0f, 0.0f, -waterHeight); // Set the clipping plane for reflection

    // Render the water reflection pass
    for (shared_ptr<Light> light : this->lights)
    {
        light->render(reflectionView, projection, lights, viewPos, true, false, plane);
    }
    for (const unique_ptr<IRenderable> &object : objects)
    {
        object->render(reflectionView, projection, lights, viewPos, true, false, plane);
    }

    player->getCamera()->setNormal(settings);

    // Set the new clipping plane for the refraction pass
    plane = glm::vec4(0.0f, -1.0f, 0.0f, waterHeight);

    // Bind the refraction framebuffer
    reflectionBuffer->unbind();
    refractionBuffer->bind();
    // Clear the buffer
    refractionBuffer->clear();

    glm::mat4 refractionView = view;

    // The project matrix is unchanged
    // Render the water refraction pass
    for (shared_ptr<Light> light : this->lights)
    {
        light->render(refractionView, projection, lights, viewPos, true, false, plane);
    }
    for (const unique_ptr<IRenderable> &object : objects)
    {
        object->render(refractionView, projection, lights, viewPos, true, false, plane);
    }
    
    // The final render pass
    glDisable(GL_CLIP_DISTANCE0);
    glEnable(GL_CULL_FACE);

    // Enable multisampling
    glEnable(GL_MULTISAMPLE);    

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // If the q key is pressed we are going to render in wireframe mode
    if (glfwGetKey(window->getWindow(), GLFW_KEY_Q) == GLFW_PRESS)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    bool photoMode = false;
    if (glfwGetKey(window->getWindow(), GLFW_KEY_F12) == GLFW_PRESS)
    {
        photoMode = true;
    }

    plane = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);  // Set the clipping plane for the final render pass
    // Render the lights
    for (shared_ptr<Light> light : this->lights)
    {
        light->render(view, projection, lights, viewPos, false, false, plane);
    }
    // Render all of the objects in the scene
    for (const unique_ptr<IRenderable> &object : objects)
    {
        object->render(view, projection, lights, viewPos, false, false, plane);
    }

    // If we are using photo mode then we want to take a screenshot of the screen and save it to a file
    if (photoMode)
    {
        // Get the width and height of the window
        int width, height;
        glfwGetWindowSize(window->getWindow(), &width, &height);
        // Create a buffer to store the pixels
        vector<unsigned char> pixels(width * height * 3);
        // Read the pixels from the framebuffer
        glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
        // Flip the image vertically
        // Save the image to a file
        string filename = settings->getCurrentWorld() + "_screenshot_" + to_string(static_cast<int>(currentFrame)) + ".png";
        // Create the directory if it does not exist
        string directory = std::string(getenv("PROJECT_ROOT")) + settings->getFilePathDelimitter() + "saves" + settings->getFilePathDelimitter() + settings->getCurrentWorld() + settings->getFilePathDelimitter() + "screenshots";
        if (!fs::exists(directory)) {
            fs::create_directories(directory);
        }
        string path = directory + settings->getFilePathDelimitter() + filename; 
        cout << "Saving screenshot to: " << path << endl;
        stbi_flip_vertically_on_write(true);
        stbi_write_png(path.c_str(), width, height, 3, pixels.data(), width * 3);
    }


    // Render the UI for the main screen
    ui->renderMain(settings, 1.0f / deltaTime, player->getPosition());

    // If the UI menu is open then disable edge scrolling if it was active
    if (settings->getCurrentPage() == UIPage::WorldMenuOpen)
    {
        player->getCamera()->setOnTopEdge(false);
        player->getCamera()->setOnBottomEdge(false);
        player->getCamera()->setOnLeftEdge(false);
        player->getCamera()->setOnRightEdge(false);
    }

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
#pragma GCC diagnostic pop

/*
 * This function will render the homepage for the application. It will be displayed when the application is first opened.
*/
void Renderer::renderHomepage()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFrame = static_cast<float>(glfwGetTime());
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    // cout << "FPS: " << 1.0f / deltaTime << endl;

    player->processKeyBoardInput(window, deltaTime);

    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the UI for the homepage
    ui->renderHomepage(settings);

    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
}

/*
 * This function will render the loading screen for the application. It will be displayed while a world is being loaded.
*/
void Renderer::renderLoading()
{
    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // Clear the screen
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the UI for the loading screen
    ui->renderLoadingScreen(settings);

    glfwSwapBuffers(window->getWindow());
    glfwPollEvents();
}

void Renderer::setupData()
{
    for (shared_ptr<Light> light : lights)
    {
        light->setupData();
    }
    // Loop through all of the objects and set up their data
    for (const unique_ptr<IRenderable> &object : objects)
    {
        object->setupData();
    }
}

void Renderer::updateData(bool regenerate){
    // double start = omp_get_wtime();
    for (shared_ptr<Light> light : lights){
        light->updateData(regenerate);
    }
    // Loop through all of the objects and update their data
    for (const unique_ptr<IRenderable>& object : objects){
        object->updateData(regenerate);
    }
}

void Renderer::addObject(unique_ptr<IRenderable> object)
{
    // Add an object to the list of objects
    objects.push_back(move(object));
}

void Renderer::addLight(shared_ptr<Light> light)
{
    // Add a light to the list of lights
    lights.push_back(light);
}

int Renderer::run()
{
    // This does nothing for now but it will be our main renderer loop
    setupData();
    static std::atomic<bool> loadingStarted = false;
    while (!glfwWindowShouldClose(window->getWindow())){
        if (settings->getCurrentPage() == UIPage::Loading){
            if (!loadingStarted) {
                loadingStarted = true;
                std::thread([this]() {
                    updateData(true);
                    settings->setCurrentPage(UIPage::WorldMenuClosed);
                    loadingStarted = false;
                }).detach();
            }
            renderLoading();
        }
        else if (settings->getCurrentPage() == UIPage::Home)
        {
            renderHomepage();
        } else {
            updateData(false);
            render(
                player->getCamera()->getViewMatrix(),
                player->getCamera()->getProjectionMatrix(),
                this->lights,
                player->getCamera()->getPosition(),
                false, // The water pass and shadow pass are set by the renderer's render function
                false,
                // The clipping plane is set by the renderer's render function
                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)
            );
        }
    }
    return 0;
}