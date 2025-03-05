/**
 * This file contains a class for the UI object that will be used to control the renderer and customise the terrain. 
 */

#include <filesystem>
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
#include "Texture.hpp"

using namespace std;
namespace fs = std::filesystem;
  
UI::UI(GLFWwindow *context, shared_ptr<Settings> settings) {
    printf("Initialising the UI\n");
    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    string textureRoot = getenv("TEXTURE_ROOT");
        
    textureHandles.clear();  // Clear the member variable
    textureFiles.clear();    // Clear the member variable

    // Find all files in the texture root directory
    for (const auto& entry : fs::directory_iterator(textureRoot)) {
        // If the file is jpg or png, add it to the list of texture files
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png")
            textureFiles.push_back(entry.path().filename().string());
    }

    for (string textureFile : textureFiles) {
        Texture texture = Texture(textureRoot + settings->getFilePathDelimitter() + textureFile, "preview", textureFile);
        textureHandles.push_back(texture.getId());
    }
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
    io.FontGlobalScale = 1.8f; 


    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height of the window

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
        
    glViewport(settings->getUIWidth(), 0, settings->getWindowWidth() - settings->getUIWidth(), settings->getWindowHeight());
            
    // Add buttons to the UI
    ImGui::Button("Regenerate", ImVec2(150, 0));
    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2(150, 0)))
    {
        // Save the current parameter settings to a file
        if (settings->getParameters()->saveToFile("test1", settings->getFilePathDelimitter())) {
            // If successful, display a message to the user
            ImGui::OpenPopup("Save Confirmation");
        } else {
            ImGui::OpenPopup("Save Failed");
        }
    }
    ImGui::SameLine();

    // Shift the home button to the right
    ImGui::SetCursorPosX(settings->getUIWidth() - 160);
    if (ImGui::Button("Home", ImVec2(150, 0))){
        settings->setCurrentWorld("");
    }

    ImGui::Spacing();

    if (ImGui::BeginPopupModal("Save Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Changes saved successfully!");
        // Centre the button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 30))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("There was a problem saving the changes!");
        // Centre the button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 30))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    static bool openTexturePopup = false;
    
    // Texture Selection Popup
    if (openTexturePopup) {
        ImGui::OpenPopup("Texture Selection");
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    }

    // Handle Texture Selection Popup
    if (ImGui::BeginPopupModal("Texture Selection", &openTexturePopup, ImGuiWindowFlags_NoResize)) {
        // Get the index of the texture in textureFiles matching a string
        static int selectedTextureIndex = -1;

        // Texture grid display
        float thumbnailSize = 100.0f;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = static_cast<int>(panelWidth / (thumbnailSize + 10.0f));
        if (columns < 1) columns = 1;

        // Make this a child of the popup so that the table is scrollable
        ImGui::BeginChild("TextureTableScroll", ImVec2(0, 500), true);
        if (ImGui::BeginTable("TextureTable", columns)) {
            for (size_t i = 0; i < textureFiles.size(); i++) {
                ImGui::TableNextColumn();
                
                // Create selectable texture preview
                ImGui::PushID(static_cast<int>(i));
                bool isSelected = (selectedTextureIndex == static_cast<int>(i));
                
                ImVec4 tint_col = isSelected ? ImVec4(1.0f, 1.0, 1.0f, 0.7f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f); 
                ImVec4 border_col = isSelected ? ImVec4(1.0f, 0.0f, 0.0f, 1.0f) : ImVec4(0.0f, 0.0f, 0.0f, 0.7f);
              
                // Show a preview image of the texture
                ImGui::Image(
                    textureHandles[i], 
                    ImVec2(thumbnailSize, thumbnailSize), 
                    ImVec2(0, 0), 
                    ImVec2(1, 1), 
                    tint_col, 
                    border_col
                );

                // Texture name shows on hover
                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", textureFiles[i].c_str());
                    ImGui::EndTooltip();
                }
                
                // Handle selection
                if (ImGui::IsItemClicked()) {
                    selectedTextureIndex = static_cast<int>(i);
                }

                ImGui::PopID();
            }
            ImGui::EndTable();
        }
        ImGui::EndChild();

        // Centre the buttons
        ImGui::SetCursorPosX((panelWidth - 240) / 2);
        
        // Apply button 
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            if (selectedTextureIndex >= 0 && selectedTextureIndex < static_cast<int>(textureFiles.size())) {

                // Call the callback function to set the texture
                setTextureCallback(textureFiles[selectedTextureIndex]);
                
                // Close the popup
                openTexturePopup = false;
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();
        
        // Cancel button
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            openTexturePopup = false;
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::PushItemWidth(300);
    if (ImGui::CollapsingHeader("Water Settings")) {
        ImGui::SliderInt("Ocean Coverage", &settings->getParameters()->getOceanCoverage(), 0, 100);
        ImGui::SliderInt("Sea Level",&settings->getParameters()->getSeaLevel(), 0, 100);
        ImGui::SliderInt("Land fragmentation", &settings->getParameters()->getLandFragmentation(), 0, 100);
        ImGui::SliderInt("Continent size", &settings->getParameters()->getContinentSize(), 0, 100);
        ImGui::SliderInt("River width", &settings->getParameters()->getRiverWidth(), 0, 100);
        ImGui::SliderInt("River depth", &settings->getParameters()->getRiverDepth(), 0, 100);
        ImGui::SliderInt("River frequency", &settings->getParameters()->getRiverFrequency(), 0, 100);
        ImGui::SliderInt("River length", &settings->getParameters()->getRiverLength(), 0, 100);
        ImGui::SliderInt("River variety", &settings->getParameters()->getRiverVariety(), 0, 100);
        ImGui::SliderInt("Lake frequency", &settings->getParameters()->getLakeFrequency(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Terrain Settings")) {
        ImGui::SliderInt("Maximum Height", &settings->getParameters()->getMaximumHeight(), 0, 100);
        ImGui::SliderInt("Height Variance", &settings->getParameters()->getHeightVariance(), 0, 100);
        ImGui::SliderInt("Mountain Frequency", &settings->getParameters()->getMountainDensity(), 0, 100);
        ImGui::SliderInt("Coastline Roughness", &settings->getParameters()->getCoastlineRoughness(), 0, 100);
        ImGui::SliderInt("Land Roughness", &settings->getParameters()->getRoughness(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Biome Settings")) {
        ImGui::SliderInt("Biome Size", &settings->getParameters()->getBiomeSize(), 0, 100);
        ImGui::SliderInt("Biome Terrain Variety", &settings->getParameters()->getBiomeTerrainVariety(), 0, 100);
        ImGui::SliderInt("Biome Border Roughness", &settings->getParameters()->getBiomeBorderRoughness(), 0, 100);
        ImGui::SliderInt("Warm/Cold", &settings->getParameters()->getWarmCold(), 0, 100);
        ImGui::SliderInt("Dry/Wet", &settings->getParameters()->getDryWet(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Vegetation Settings")) {
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
    if (ImGui::CollapsingHeader("Advanced Settings")) {
        if (ImGui::CollapsingHeader("Subtropical Desert")) {
            ImGui::SliderInt("Chance of occurring##1", &settings->getParameters()->getDesertProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getDesertTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDesertTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Temperate Forest")) {
            ImGui::SliderInt("Chance of occurring##2", &settings->getParameters()->getTemperateForestProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getTemperateForestTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateForestTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Tropical Rainforest")) {
            ImGui::SliderInt("Chance of occurring##3", &settings->getParameters()->getTropicalRainforestProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getTropicalRainforestTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTropicalRainforestTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Savanna")) {
            ImGui::SliderInt("Chance of occurring##4", &settings->getParameters()->getSavannaProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getSavannaTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Temperate Rainforest")) {
            ImGui::SliderInt("Chance of occurring##5", &settings->getParameters()->getTemperateRainforestProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getTemperateRainforestTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##5")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateRainforestTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Boreal Forest")) {
            ImGui::SliderInt("Chance of occurring##6", &settings->getParameters()->getBorealForestProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getBorealForestTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##6")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealForestTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Grassland")) {
            ImGui::SliderInt("Chance of occurring##7", &settings->getParameters()->getGrasslandProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getGrasslandTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##7")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrasslandTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Woodland")) {
            ImGui::SliderInt("Chance of occurring##8", &settings->getParameters()->getWoodlandProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getWoodlandTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##8")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTexture(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Tundra")) {
            ImGui::SliderInt("Chance of occurring##9", &settings->getParameters()->getTundraProbability(), 0, 100);
            ImGui::Text("Current Texture: %s", settings->getParameters()->getTundraTexture().c_str());
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##9")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTundraTexture(texture);
                };
                openTexturePopup = true;
            }
        }
    }
    ImGui::PopItemWidth();
    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



void UI::renderHomepage(shared_ptr<Settings> settings) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable gamepad navigation 
    io.WantCaptureMouse = true;


    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("Homepage", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    // Add buttons to the UI
    if (ImGui::Button("New World", ImVec2(150, 0))) {

        // Ask for the name of the new world
        ImGui::OpenPopup("New World Name");
    }

    if (ImGui::BeginPopupModal("New World Name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter the name of the new world:");

        static char newWorldName[128] = "";
        ImGui::InputText("Name", newWorldName, IM_ARRAYSIZE(newWorldName));
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            // Create a new world
            settings->setCurrentWorld(newWorldName);
            newWorldName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void UI::renderLoadingScreen(shared_ptr<Settings> settings) {
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable keyboard navigation
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable gamepad navigation
    io.WantCaptureMouse = true;
    io.FontGlobalScale = 2.0f;

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("Loading", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

   
    // Animated Loading Dots
    static float elapsedTime = 0.0f;
    elapsedTime += ImGui::GetIO().DeltaTime;
    int dotCount = static_cast<int>(elapsedTime * 2.0f) % 4;  // Cycle through 0-3 dots

    std::string loadingText = "Generating World";
    for (int i = 0; i < dotCount; ++i) loadingText += ".";

    
    ImGui::SetCursorPosX((settings->getWindowWidth() - ImGui::CalcTextSize("Generating World").x) / 2);
    ImGui::SetCursorPosY((settings->getWindowHeight() - ImGui::CalcTextSize("Generating World").y) / 2);
    ImGui::Text("%s", loadingText.c_str());

    ImGui::End();

    // Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}