/**
 * This file contains a class for the UI object that will be used to control the renderer and customise the terrain. 
 */

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
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "Object.hpp"
#include "Settings.hpp"
#include "UI.hpp"
#include "Window.hpp"

using namespace std;

UI::UI(GLFWwindow *context) {
    printf("Initialising the UI\n");
    // Initialize ImGui
    ImGui::CreateContext();
    // if (!ImGui::GetCurrentContext()) {
    //     std::cerr << "Failed to initialize ImGui context!" << std::endl;
    // } else {
    //     std::cout << "ImGui context initialized successfully!" << std::endl;
    // }
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    oceanCoverage = 0.2f;
}

UI::~UI() {
    printf("Shutting down the UI\n");
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
}

void UI::render(shared_ptr<Settings> settings, float fps, glm::vec3 playerPos) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable gamepad navigation 
    io.WantCaptureMouse = true;

    //printf("Rendering the UI\n");
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height
    // round the values to 2 decimal places
    std::string title = "Menu - FPS: ";
    title += to_string(static_cast<int>(std::round(fps)));
    title += " - Player Position: (" + to_string(static_cast<int>(std::ceil(playerPos.x)));
    title += ", " + to_string(static_cast<int>(std::ceil(playerPos.y)));
    title += ", " + to_string(static_cast<int>(std::ceil(playerPos.z))) + ")";
    // Compute the chunk that the player is in
    // Add size /2 to the player position to account for the translation transformation
    int chunkX;
    int chunkZ;
    chunkX = static_cast<int>(floor((playerPos.x + settings->getChunkSize() / 2) / settings->getChunkSize()));
    chunkZ = static_cast<int>(floor((playerPos.z + settings->getChunkSize() / 2) / settings->getChunkSize()));
    title += " - Chunk: (" + to_string(chunkX) + ", " + to_string(chunkZ) + ")";

    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );

    if (ImGui::IsWindowCollapsed())
    {
        //cout << "Window is collapsed" << endl;
        settings->setUIWidth(0);
    } else {
        //cout << "Window is not collapsed" << endl;
        settings->setUIWidth(500);
    }

    glViewport(settings->getUIWidth(), 0, settings->getWindowWidth() - settings->getUIWidth(), settings->getWindowHeight());


    ImGui::SliderFloat("Ocean Coverage", &oceanCoverage, 0.0f, 1.0f);
    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}