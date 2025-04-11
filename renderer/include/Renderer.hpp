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
#include <list>

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
#include "Screen.hpp"
#include "Light.hpp"
#include "UI.hpp"

using namespace std;

class Renderer : public IRenderable{
private:
    shared_ptr<Window> window; // The window that the renderer will use
    shared_ptr<Settings> settings; // The settings that the renderer will use
    shared_ptr<Player> player; // The player that the renderer will use
    shared_ptr<Framebuffer> framebuffer; // The framebuffer that the renderer will use
    vector<unique_ptr<IRenderable>> objects; // The objects that the renderer will render
    vector<shared_ptr<Light>> lights;
    shared_ptr<UI> ui; // The UI object that will be used to control the renderer and customise the terrain
    unique_ptr<Screen> screen; // The screen object that will be used to render the framebuffer to the screen
    // vector<unique_ptr<IRenderable>> objects; // The objects that the renderer will render
    float lastFrame = 0.0f; // The time of the last frame
    float deltaTime = 0.0f; // The time between the current frame and the last frame
    float currentFrame = 0.0f; // The time of the current frame

public:
    //Renderer();
    Renderer(
        shared_ptr<Window> inWindow,
        shared_ptr<Settings> inSettings,
        shared_ptr<Player> inPlayer,
        shared_ptr<Framebuffer> inFramebuffer,
        shared_ptr<UI> inUI,
        unique_ptr<Screen> inScreen
    ):
        window(inWindow),
        settings(inSettings),
        player(inPlayer),
        framebuffer(inFramebuffer),
        ui(inUI),
        screen(move(inScreen))
    {
        objects = vector<unique_ptr<IRenderable>>();
        lights = vector<shared_ptr<Light>>();
        setCallbackFunctions();
    };
    ~Renderer();

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;

    void renderHomepage();
    void renderLoading();

    // // This is the main run function for the renderer
    int run();
    void addObject(unique_ptr<IRenderable> object);
    void addLight(shared_ptr<Light> light);


    // // Getters and setters
    shared_ptr<Window> getWindow(){return window;}
    shared_ptr<Settings> getSettings(){return settings;}
    shared_ptr<Player> getPlayer(){return player;}
    shared_ptr<Framebuffer> getFramebuffer(){return framebuffer;}
    const vector<unique_ptr<IRenderable>>& getObjects() const {return objects;}
    vector<shared_ptr<Light>> getLights(){return lights;}
    float getLastFrame(){return lastFrame;}
    float getDeltaTime(){return deltaTime;}
    float getCurrentFrame(){return currentFrame;}
    void setWindow(shared_ptr<Window> window){this->window = window;}
    void setSettings(shared_ptr<Settings> settings){this->settings = settings;}
    void setPlayer(shared_ptr<Player> player){this->player = player;}
    void setFramebuffer(shared_ptr<Framebuffer> framebuffer){this->framebuffer = framebuffer;}
    void setObjects(vector<unique_ptr<IRenderable>> objects){this->objects = move(objects);}
    void setLights(vector<shared_ptr<Light>> lights){this->lights = lights;}
    void setLastFrame(float lastFrame){this->lastFrame = lastFrame;}
    void setDeltaTime(float deltaTime){this->deltaTime = deltaTime;}
    void setCurrentFrame(float currentFrame){this->currentFrame = currentFrame;}

    void setCallbackFunctions();
};

#endif  // RENDERER_HPP