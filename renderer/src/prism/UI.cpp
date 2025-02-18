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
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UI::render(shared_ptr<Settings> settings) {
    printf("Rendering the UI\n");
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height

    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove );
    
    if (ImGui::IsWindowCollapsed())
    {
        glViewport(0, 0, settings->getWindowWidth(), settings->getWindowHeight());
    } else {
        glViewport(settings->getUIWidth(), 0, settings->getWindowWidth() - settings->getUIWidth(), settings->getWindowHeight());
    }
    
    
    ImGui::SliderFloat("Ocean Coverage", &oceanCoverage, 0.0f, 1.0f);
    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}