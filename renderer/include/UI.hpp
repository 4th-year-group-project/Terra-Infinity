/**
 * @file UI.hpp
 * @author King Attalus II
 * @brief This file contains the UI class, which is used to create and manage the user interface for the application.
 * @version 1.0
 * @date 2025
 *
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
 * @brief This class is used to create and manage the user interface for the application.
 *
 * @details The UI class uses the ImGui library to create a graphical user interface for the application.
 * It provides functions to render the main menu, loading screen, and homepage.
 *
 */
class UI {
private:
    std::vector<GLuint> textureHandles; // A list of texture handles of texture previews that will be used in the UI texture selector
    std::unordered_map<std::string, GLuint> previewMap; // A map of texture names to their preview IDs
    vector<string> textureFiles; // The names of the textures that will be used in the UI
    std::function<void (std::string)> setTextureCallback; // A callback function that will be used to change the texture selected
    Texture logoTexture; // The logo texture that will be used in the UI
    bool openTexturePopup = false; // A flag that will be used to open the texture selection popup

    // Helper function for rendering texture selecction section of the menu for a given texture group
    void drawTextureSelectionSection(
        const std::string& labelPrefix,
        const std::string& textureLow,
        const std::string& textureMidFlat,
        const std::string& textureMidSteep,
        const std::string& textureHigh,
        std::function<void(const std::string&)> setLowCallback,
        std::function<void(const std::string&)> setMidFlatCallback,
        std::function<void(const std::string&)> setMidSteepCallback,
        std::function<void(const std::string&)> setHighCallback
    );

public:
    UI(GLFWwindow *context, std::shared_ptr<Settings> settings); 

    ~UI();

    // Getters
    std::vector<GLuint> getTextureHandles() {return textureHandles;}
    std::vector<std::string> getTextureFiles() {return textureFiles;}

    // Get the texture preview ID for a given texture name
    GLuint getTexturePreviewID(const std::string& filename);

    // Functions to render a single frame of the UI for each screen
    void renderMain(std::shared_ptr<Settings> settings, float fps, glm::vec3 playerPos);
    void renderLoadingScreen(std::shared_ptr<Settings> settings);
    void renderHomepage(std::shared_ptr<Settings> settings);
};

#endif // UI_HPP
