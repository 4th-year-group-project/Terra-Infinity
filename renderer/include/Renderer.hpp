/*
    This class contains the main renderer for the application. It is responsible for managing all
    of the resources that will be required from the framebuffer to the camera, along with the
    list of objects that will need to be rendered in the scene.
*/
#ifndef RENDERER_HPP
#define RENDERER_HPP


#include <iostream>
#include <string>
#include <vector>
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

#include "IRenderable.hpp"
#include "Window.hpp"
#include "Settings.hpp"
#include "Player.hpp"
#include "Framebuffer.hpp"

using namespace std;

class Renderer : public IRenderable{
private:
    shared_ptr<Window> window; // The window that the renderer will use
    shared_ptr<Settings> settings; // The settings that the renderer will use
    shared_ptr<Player> player; // The player that the renderer will use
    shared_ptr<Framebuffer> framebuffer; // The framebuffer that the renderer will use
    // vector<unique_ptr<IRenderable>> objects; // The objects that the renderer will render
    float lastFrame = 0.0f; // The time of the last frame
    float deltaTime = 0.0f; // The time between the current frame and the last frame
    float currentFrame = 0.0f; // The time of the current frame

public:
    Renderer();
    Renderer(
        shared_ptr<Window> window,
        shared_ptr<Settings> settings,
        shared_ptr<Player> player,
        shared_ptr<Framebuffer> framebuffer
    ):
        window(window),
        settings(settings),
        player(player),
        framebuffer(framebuffer)
    {cout << "Creating the renderer" << endl;}
    ~Renderer();

    void render(glm::mat4 view, glm::mat4 projection) override;
    void setupData() override;
    void updateData() override;

    // // This is the main run function for the renderer
    // int run();

    // // Getters and setters
    // Window getWindow(){return window;}
    // Settings getSettings(){return settings;}
    // Player getPlayer(){return player;}
    // Framebuffer getFramebuffer(){return framebuffer;}
    // void setWindow(Window window){this->window = window;}
    // void setSettings(Settings settings){this->settings = settings;}
    // void setPlayer(Player player){this->player = player;}
    // void setFramebuffer(Framebuffer framebuffer){this->framebuffer = framebuffer;}
    // // void addObject(unique_ptr<IRenderable> object){objects.push_back(move(object));}
    // // Callback functions
    // void setCallbackFunctions();
};

#endif  // RENDERER_HPP