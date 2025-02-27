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
}

UI::~UI() {
    printf("Shutting down the UI\n");
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
}

void UI::render(shared_ptr<Settings> settings) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable gamepad navigation 
    io.WantCaptureMouse = true;
    io.FontGlobalScale = 2.0f; //
    //printf("Rendering the UI\n");
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height

    cout << "Show UI: " << settings->getShowUI() << endl;

    // if (!settings->getShowUI()) {
    //     ImGui::SetNextWindowCollapsed(true, ImGuiCond_Once); 
    // } else {
    //     ImGui::SetNextWindowCollapsed(false, ImGuiCond_Once);
    // }
    
    ImGui::SetNextWindowCollapsed(!settings->getShowUI(), ImGuiCond_Always);
    ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    if (ImGui::IsWindowCollapsed())
    {
        //cout << "Window is collapsed" << endl;
        settings->setShowUI(false);
        settings->setUIWidth(0);
    } else {
        //cout << "Window is not collapsed" << endl;
        settings->setShowUI(true);
        settings->setUIWidth(700);
    }
    cout << "Ocean Coverage: " << settings->getParameters()->getOceanCoverage() << endl;
    cout << "Sea Level: " << settings->getParameters()->getSeaLevel() << endl;
    cout << "Maximum Height: " << settings->getParameters()->getMaximumHeight() << endl;
        
    glViewport(settings->getUIWidth(), 0, settings->getWindowWidth() - settings->getUIWidth(), settings->getWindowHeight());

    ImGui::Button("Regenerate Terrain");
    ImGui::Button("Save Terrain");
    ImGui::Button("Home");

    ImGui::PushItemWidth(300);
    if (ImGui::CollapsingHeader("Global Parameters")) {
        ImGui::Text("Water");
        // cout << "Ocean Coverage: " << settings->getParameters()->getOceanCoverage() << endl;
        // cout << "Sea Level: " << settings->getParameters()->getSeaLevel() << endl;
        // cout << "Maximum Height: " << settings->getParameters()->getMaximumHeight() << endl;
        ImGui::SliderInt("Ocean Coverage (%)", &settings->getParameters()->getOceanCoverage(), 0, 100);
        ImGui::SliderInt("Sea Level (%)",&settings->getParameters()->getSeaLevel(), 0, 100);
        ImGui::SliderInt("Land fragmentation (%)", &settings->getParameters()->getLandFragmentation(), 0, 100);
        ImGui::SliderInt("Continent size (%)", &settings->getParameters()->getContinentSize(), 0, 100);
        ImGui::SliderInt("River width", &settings->getParameters()->getRiverWidth(), 0, 100);
        ImGui::SliderInt("River depth", &settings->getParameters()->getRiverDepth(), 0, 100);
        ImGui::SliderInt("River frequency", &settings->getParameters()->getRiverFrequency(), 0, 100);
        ImGui::SliderInt("River length", &settings->getParameters()->getRiverLength(), 0, 100);
        ImGui::SliderInt("River variety", &settings->getParameters()->getRiverVariety(), 0, 100);
        ImGui::SliderInt("Lake frequency", &settings->getParameters()->getLakeFrequency(), 0, 100);
        ImGui::Text("Terrain");
        ImGui::SliderInt("Maximum Height", &settings->getParameters()->getMaximumHeight(), 0, 100);
        ImGui::SliderInt("Height Variance", &settings->getParameters()->getHeightVariance(), 0, 100);
        ImGui::SliderInt("Mountain Frequency", &settings->getParameters()->getMountainDensity(), 0, 100);
        ImGui::SliderInt("Roughness", &settings->getParameters()->getRoughness(), 0, 100);
        ImGui::Text("Biomes");
        ImGui::SliderInt("Biome Size", &settings->getParameters()->getBiomeSize(), 0, 100);
        ImGui::SliderInt("Biome Terrain Variety", &settings->getParameters()->getBiomeTerrainVariety(), 0, 100);
        ImGui::SliderInt("Biome Border Roughness", &settings->getParameters()->getBiomeBorderRoughness(), 0, 100);
        ImGui::SliderInt("Coastline Roughness", &settings->getParameters()->getCoastlineRoughness(), 0, 100);
        ImGui::SliderInt("Warm/Cold", &settings->getParameters()->getWarmCold(), 0, 100);
        ImGui::SliderInt("Dry/Wet", &settings->getParameters()->getDryWet(), 0, 100);
        ImGui::Text("Vegetation");
        ImGui::SliderInt("Tree Density", &settings->getParameters()->getTreesDensity(), 0, 100);
        ImGui::SliderInt("Tree Variety", &settings->getParameters()->getTreeVariety(), 0, 100);
        ImGui::SliderInt("Tree Height", &settings->getParameters()->getTreeHeight(), 0, 100);
        ImGui::SliderInt("Tree Width", &settings->getParameters()->getTreeWidth(), 0, 100);
        ImGui::SliderInt("Tree Biome Exclusivity", &settings->getParameters()->getTreeBiomeExclusivity(), 0, 100);
        ImGui::SliderInt("Grass Density", &settings->getParameters()->getGrassDensity(), 0, 100);
        ImGui::SliderInt("Grass Variety", &settings->getParameters()->getGrassVariety(), 0, 100);
        ImGui::SliderInt("Bush Density", &settings->getParameters()->getBushDensity(), 0, 100);
        ImGui::SliderInt("Bush Variety", &settings->getParameters()->getBushVariety(), 0, 100);
        ImGui::SliderInt("Bush Frequency", &settings->getParameters()->getBushFrequency(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Subtropical Desert")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getDesertProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Temperate Forest")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getTemperateForestProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Tropical Rainforest")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getTropicalRainforestProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Savanna")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getSavannaProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Temperate Rainforest")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getTemperateRainforestProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Boreal Forest")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getBorealForestProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Grassland")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getGrasslandProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Woodland")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getWoodlandProbability(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Tundra")) {
        ImGui::SliderInt("Chance of occurring", &settings->getParameters()->getTundraProbability(), 0, 100);
    }

    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}