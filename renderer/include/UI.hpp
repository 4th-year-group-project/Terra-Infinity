/**
 * This file contains a class for the UI object that will be used to control the renderer and customise the terrain. 
 */

#ifndef UI_HPP
#define UI_HPP

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
 
#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

#include <unordered_map>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Object.hpp"
#include "Settings.hpp"
#include "Window.hpp"

/**
 * This class represents the user interface object that will be used render ImGui user interface that the user can interact with.
 */
class UI {
private:
    std::vector<GLuint> textureHandles; // A list of texture handles of texture previews that will be used in the UI texture selector
    std::unordered_map<std::string, GLuint> previewMap; // A map of texture names to their preview IDs
    vector<string> textureFiles; // The names of the textures that will be used in the UI
    std::function<void (std::string)> setTextureCallback; // A callback function that will be used to change the texture selected
public:
    UI(GLFWwindow *context); 

    ~UI();

    // Getters
    std::vector<GLuint> getTextureHandles() {return textureHandles;}
    std::vector<std::string> getTextureFiles() {return textureFiles;}

    // Get the texture preview ID for a given texture name
    GLuint getTexturePreviewID(const std::string& filename);

    // Functions to render a single frame of the UI for each screen
    void renderMain(shared_ptr<Settings> settings, float fps, glm::vec3 playerPos);
    void renderLoadingScreen(shared_ptr<Settings> settings);
    void renderHomepage(shared_ptr<Settings> settings);
};

#endif // UI_HPP