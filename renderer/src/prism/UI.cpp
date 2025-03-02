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
#include "Texture.hpp"

using namespace std;
  
UI::UI(GLFWwindow *context, shared_ptr<Settings> settings) {
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

    string textureRoot = getenv("TEXTURE_ROOT");
        
    textureHandles.clear();  // Clear the member variable

    textureFiles = {"grass.jpg", "sand.jpg", "rock.jpg"};
    for (string textureFile : textureFiles) {
        cout << "Texture file: " << textureFile << endl;
        Texture texture = Texture(textureRoot + settings->getFilePathDelimitter() + textureFile, "texture", "grass", 100, 100, 3);
        textureHandles.push_back(texture.getId());
        cout << "Texture ID: " << texture.getId() << endl;
    }
    cout << "Inside constructor, textureFiles size: " << textureFiles.size() << endl;
    for (const auto& file : textureFiles) {
        cout << "  " << file << endl;
    }
    cout << "Inside constructor, textureHandles size: " << textureFiles.size() << endl;
    for (const auto& handle : textureHandles) {
        cout << "  " << handle << endl;
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
    io.FontGlobalScale = 2.0f; //
    //printf("Rendering the UI\n");
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

    ImGui::Button("Regenerate Terrain");
    ImGui::Button("Save Terrain");
    ImGui::Button("Home");

    // Add texture selection button
    static bool openTexturePopup = false;
    if (ImGui::Button("Select Texture")) {
        openTexturePopup = true;
    }

    // Texture Selection Popup
    if (openTexturePopup) {
        ImGui::OpenPopup("Texture Selection");
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    }

    // Handle Texture Selection Popup
    if (ImGui::BeginPopupModal("Texture Selection", &openTexturePopup, ImGuiWindowFlags_NoResize)) {
        static int selectedTextureIndex = -1;
        
        ImGui::Text("Select a texture:");
        ImGui::Separator();

        // Texture grid display
        float thumbnailSize = 100.0f;
        float panelWidth = ImGui::GetContentRegionAvail().x;
        int columns = static_cast<int>(panelWidth / (thumbnailSize + 10.0f));
        if (columns < 1) columns = 1;

        if (ImGui::BeginTable("TextureTable", columns)) {
            for (size_t i = 0; i < textureFiles.size(); i++) {
                ImGui::TableNextColumn();
                
                // Create selectable texture preview
                ImGui::PushID(static_cast<int>(i));
                bool isSelected = (selectedTextureIndex == static_cast<int>(i));
                
                // Draw texture preview (replace with your texture drawing logic)
                ImGui::BeginGroup();
                
                // Create a colored rectangle as placeholder for texture preview
                // Replace this with actual texture rendering using OpenGL textures
                ImVec2 pos = ImGui::GetCursorScreenPos();
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // isSelected ? ImVec4(1.0f, 0.8f, 0.8f, 1.0f) : 
                ImVec4 border_col = isSelected ? ImVec4(1.0f, 0.0f, 0.0f, 0.5f) : ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
                
                // Show a preview image of the texture
                ImGui::Image(
                    textureHandles[i], 
                    ImVec2(thumbnailSize, thumbnailSize), 
                    ImVec2(0, 0), 
                    ImVec2(1, 1), 
                    tint_col, 
                    border_col
                );


                // Texture name under the preview
                float textWidth = ImGui::CalcTextSize(textureFiles[i].c_str()).x;
                float textPosX = pos.x + (thumbnailSize - textWidth) * 0.5f;
                ImGui::SetCursorScreenPos(ImVec2(textPosX, pos.y + thumbnailSize + 5.0f));
                ImGui::Text("%s", textureFiles[i].c_str());
                
                ImGui::EndGroup();
                
                // Handle selection
                if (ImGui::IsItemClicked()) {
                    selectedTextureIndex = static_cast<int>(i);
                }
                
                ImGui::PopID();
            }
            ImGui::EndTable();
        }

        ImGui::Separator();
        
        // // Upload new texture button and functionality
        // if (ImGui::Button("Upload New Texture", ImVec2(180, 0))) {
        //     // This would typically open a file dialog
        //     // For now, simulate adding a new texture
        //     textureFiles.push_back("new_texture_" + std::to_string(textureFiles.size() + 1) + ".png");
        //     textureHandles.push_back(0); // Replace with actual texture loading code
            
        //     // In a real implementation, you would:
        //     // 1. Open a file dialog to select an image
        //     // 2. Load the image and create an OpenGL texture
        //     // 3. Add it to your texture list
        //     // 4. Save the path for future reference
            
        //     // Example pseudocode:
        //     // const char* filePath = openFileDialog("*.png,*.jpg;*.jpeg;*.tga;*.bmp");
        //     // if (filePath) {
        //     //     GLuint textureID = loadTexture(filePath);
        //     //     textureFiles.push_back(getFileName(filePath));
        //     //     textureHandles.push_back(textureID);
        //     //     saveTextureList(textureFiles); // Save updated list to config
        //     // }
        // }
        
        ImGui::SameLine();
        
        // Apply button (to use the selected texture)
        if (ImGui::Button("Apply Selected Texture", ImVec2(180, 0))) {
            if (selectedTextureIndex >= 0 && selectedTextureIndex < static_cast<int>(textureFiles.size())) {
                // Apply the selected texture
                // settings->getParameters()->setSelectedTexture(textureFiles[selectedTextureIndex]);
                // settings->getParameters()->setSelectedTextureID(textureHandles[selectedTextureIndex]);
                
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
    if (ImGui::CollapsingHeader("Water settings")) {
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
    }
    if (ImGui::CollapsingHeader("Terrain settings")) {
        ImGui::SliderInt("Maximum Height", &settings->getParameters()->getMaximumHeight(), 0, 100);
        ImGui::SliderInt("Height Variance", &settings->getParameters()->getHeightVariance(), 0, 100);
        ImGui::SliderInt("Mountain Frequency", &settings->getParameters()->getMountainDensity(), 0, 100);
        ImGui::SliderInt("Roughness", &settings->getParameters()->getRoughness(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Biome settings")) {
        ImGui::SliderInt("Biome Size", &settings->getParameters()->getBiomeSize(), 0, 100);
        ImGui::SliderInt("Biome Terrain Variety", &settings->getParameters()->getBiomeTerrainVariety(), 0, 100);
        ImGui::SliderInt("Biome Border Roughness", &settings->getParameters()->getBiomeBorderRoughness(), 0, 100);
        ImGui::SliderInt("Coastline Roughness", &settings->getParameters()->getCoastlineRoughness(), 0, 100);
        ImGui::SliderInt("Warm/Cold", &settings->getParameters()->getWarmCold(), 0, 100);
        ImGui::SliderInt("Dry/Wet", &settings->getParameters()->getDryWet(), 0, 100);
    }
    if (ImGui::CollapsingHeader("Vegetation settings")) {
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
    if (ImGui::CollapsingHeader("Advanced settings")) {
        if (ImGui::CollapsingHeader("Subtropical Desert")) {
            ImGui::SliderInt("Chance of occurring##1", &settings->getParameters()->getDesertProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Temperate Forest")) {
            ImGui::SliderInt("Chance of occurring##2", &settings->getParameters()->getTemperateForestProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Tropical Rainforest")) {
            ImGui::SliderInt("Chance of occurring##3", &settings->getParameters()->getTropicalRainforestProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Savanna")) {
            ImGui::SliderInt("Chance of occurring##4", &settings->getParameters()->getSavannaProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Temperate Rainforest")) {
            ImGui::SliderInt("Chance of occurring##5", &settings->getParameters()->getTemperateRainforestProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Boreal Forest")) {
            ImGui::SliderInt("Chance of occurring##6", &settings->getParameters()->getBorealForestProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Grassland")) {
            ImGui::SliderInt("Chance of occurring##7", &settings->getParameters()->getGrasslandProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Woodland")) {
            ImGui::SliderInt("Chance of occurring##8", &settings->getParameters()->getWoodlandProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Tundra")) {
            ImGui::SliderInt("Chance of occurring##9", &settings->getParameters()->getTundraProbability(), 0, 100);
        }
    }
    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}