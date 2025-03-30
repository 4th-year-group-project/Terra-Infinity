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

    // Disable keyboard and gamepad navigation
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~(ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad);

    // Set the ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base style 
    ImGui::StyleColorsDark();

    colors[ImGuiCol_ChildBg]         = ImVec4(0.04f, 0.2f, 0.2f, 1.0f); // dark aqua


    // Text
    colors[ImGuiCol_Text]            = ImVec4(0.9f, 0.98f, 0.98f, 1.0f);  // light aqua
    colors[ImGuiCol_TextDisabled]    = ImVec4(0.45f, 0.55f, 0.55f, 1.0f);

    // Title bar 
    colors[ImGuiCol_TitleBg]         = ImVec4(0.1f, 0.45f, 0.45f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed]= ImVec4(0.05f, 0.25f, 0.25f, 0.7f);
    colors[ImGuiCol_TitleBgActive]   = ImVec4(0.1f, 0.45f, 0.45f, 1.0f);

    // Buttons
    colors[ImGuiCol_Button]          = ImVec4(0.10f, 0.45f, 0.45f, 1.0f);
    colors[ImGuiCol_ButtonHovered]   = ImVec4(0.15f, 0.6f, 0.6f, 1.0f);
    colors[ImGuiCol_ButtonActive]    = ImVec4(0.10f, 0.45f, 0.45f, 1.0f);

    // Headers / selectable
    colors[ImGuiCol_Header]          = ImVec4(0.10f, 0.35f, 0.35f, 1.0f);
    colors[ImGuiCol_HeaderHovered]   = ImVec4(0.18f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_HeaderActive]    = ImVec4(0.10f, 0.45f, 0.45f, 1.0f);

    // Frames (input, sliders, etc)
    colors[ImGuiCol_FrameBg]         = ImVec4(0.07f, 0.25f, 0.25f, 1.0f);
    colors[ImGuiCol_FrameBgHovered]  = ImVec4(0.12f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_FrameBgActive]   = ImVec4(0.10f, 0.35f, 0.35f, 1.0f);

    // Sliders, checkboxes, grabs
    colors[ImGuiCol_SliderGrab]      = ImVec4(0.25f, 0.7f, 0.7f, 1.0f);
    colors[ImGuiCol_SliderGrabActive]= ImVec4(0.35f, 0.9f, 0.9f, 1.0f);
    colors[ImGuiCol_CheckMark]       = ImVec4(0.35f, 0.85f, 0.85f, 1.0f);

    // Tabs
    colors[ImGuiCol_Tab]             = ImVec4(0.10f, 0.45f, 0.45f, 1.0f);
    colors[ImGuiCol_TabHovered]      = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabActive]       = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabUnfocused]    = colors[ImGuiCol_Tab];
    colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_Tab];

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg]     = ImVec4(0.02f, 0.10f, 0.10f, 1.0f);
    colors[ImGuiCol_ScrollbarGrab]   = ImVec4(0.12f, 0.4f, 0.4f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.5f, 0.5f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.10f, 0.35f, 0.35f, 1.0f);

    // Border
    colors[ImGuiCol_Border]          = ImVec4(0.06f, 0.15f, 0.15f, 0.5f);
    colors[ImGuiCol_BorderShadow]    = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    // Resize grip
    colors[ImGuiCol_ResizeGrip]      = ImVec4(0.2f, 0.5f, 0.5f, 0.2f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3f, 0.7f, 0.7f, 0.4f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4f, 0.8f, 0.8f, 0.6f);

    // Style tweaks
    style.FrameRounding = 4.0f;
    style.WindowRounding = 5.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarSize = 14.0f;
}

UI::~UI() {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
}

void UI::render(shared_ptr<Settings> settings) {
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;
    io.FontGlobalScale = 1.8f; 

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::SetNextWindowCollapsed(settings->getCurrentPage() != UIPage::WorldMenuOpen, ImGuiCond_Always);

    ImGui::Begin(settings->getCurrentWorld().c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    if (ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuOpen)
    {
        settings->setCurrentPage(UIPage::WorldMenuClosed);
    } else if (!ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuClosed)
    {
        settings->setCurrentPage(UIPage::WorldMenuOpen);
    }
            
    // Add buttons to the UI
    ImGui::Button("Regenerate", ImVec2(150, 0));
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Regenerate the world using the current settings");
        ImGui::EndTooltip();
    }
    ImGui::SameLine();
    if (ImGui::Button("Save", ImVec2(150, 0)))
    {
        // Save the current parameter settings to a file
        if (settings->getParameters()->saveToFile(settings->getCurrentWorld(), settings->getFilePathDelimitter())) {
            // If successful, display a message to the user
            ImGui::OpenPopup("Save Confirmation");
        } else {
            ImGui::OpenPopup("Save Failed");
        }
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Save the current world settings");
        ImGui::EndTooltip();
    }
    ImGui::SameLine();

    // Shift the home button to the right
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 160);
    if (ImGui::Button("Home", ImVec2(150, 0))){
        settings->setCurrentPage(UIPage::Home);
        settings->setCurrentWorld("");
        ImGui::GetStateStorage()->Clear();
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Return to homepage");
        ImGui::EndTooltip();
    }
    ImGui::Spacing();

    ImGui::SetCursorPosX(0);
    if (ImGui::BeginPopupModal("Save Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Changes saved successfully!");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 30))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("There was a problem saving the changes!");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 30))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
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
        ImGui::BeginChild("TextureTableScroll", ImVec2(0, 500), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
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
    
    ImGui::Spacing();

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
        ImGui::SliderInt("Shrub Density", &settings->getParameters()->getBushDensity(), 0, 100);
        ImGui::SliderInt("Shrub Variety", &settings->getParameters()->getBushVariety(), 0, 100);
        ImGui::SliderInt("Shrub Frequency", &settings->getParameters()->getBushFrequency(), 0, 100);
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
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;
    io.FontGlobalScale = 2.0f;

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("TerraToolbox", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Press 'New World' to generate a new world or select a saved world to open it.");

    ImGui::Dummy(ImVec2(0, 20));
    // Centre the button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) / 2);
    if (ImGui::Button("New World", ImVec2(300, 0))) {
        // Ask for the name of the new world
        // Centre the popup
        ImGui::OpenPopup("New World Name");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Generate a new world");
        ImGui::EndTooltip();
    }

    ImGui::Dummy(ImVec2(0, 20));

    // Display the scrollable list of saved worlds
    ImGui::Text("Saved worlds:");
    ImGui::SetCursorPosX(0);
    ImGui::BeginChild("SavedWorlds", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 300), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Retrieve the list of saved worlds from the saved directory
    string dataRoot = getenv("DATA_ROOT");
    string savedRoot = dataRoot + settings->getFilePathDelimitter() + "saved" + settings->getFilePathDelimitter();
    vector<string> savedFiles;
    for (const auto& entry : fs::directory_iterator(savedRoot)) {
        savedFiles.push_back(entry.path().filename().string());
    }

    static string toDelete = "";
    static string toRename = "";

    // Display the saved worlds as buttons
    for (string savedFile : savedFiles) {
        if (ImGui::Button(savedFile.c_str(), ImVec2(1600, 0))) {
            settings->getParameters()->loadFromFile(savedFile, settings->getFilePathDelimitter());
            settings->setCurrentWorld(savedFile);
            settings->setCurrentPage(UIPage::WorldMenuClosed);
        }
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Open %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        ImGui::SameLine();

        // Add a blue rename button next to each saved world
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.65f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.75f, 1.0f));
        if (ImGui::Button((std::string("Rename##") + savedFile).c_str(), ImVec2(100, 0))) {
            // Set the toRename variable to the selected file name
            toRename = savedFile;
        };
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Rename %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();

        ImGui::SameLine();
        // Add a red delete button next to each saved world
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6, 0.2, 0.2, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));
        if (ImGui::Button((std::string("Delete##") + savedFile).c_str(), ImVec2(100, 0))) {
            // Set the toDelete variable to the selected file name
            toDelete = savedFile;
        };
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Delete %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();

    if (toDelete != "") {
        ImGui::OpenPopup("Delete Confirmation");
    }

    if (toRename != "") {
        ImGui::OpenPopup("Rename World");
    }

    // Delete confirmation popup
    if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Are you sure you want to delete this world?");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("Confirm", ImVec2(120, 30))) {
            // Delete the file
            if (fs::remove(savedRoot + toDelete)) {
                toDelete = "";
            } else {
                cout << "Failed to delete file" << endl;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 30))) {
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    static char newWorldName[128] = "";
    static bool exists = false;
    static bool empty = false;

    // Rename world popup
    if (ImGui::BeginPopupModal("Rename World", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a new name for %s:", toRename.c_str());
        ImGui::Spacing();
        // Input field for the new world name
        ImGui::InputText("##New Name", newWorldName, IM_ARRAYSIZE(newWorldName));
        ImGui::Spacing();
        // Centre the buttons
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            empty = false;
            exists = false;

            // Check if the name is empty
            if (newWorldName[0] == '\0') {
                empty = true;
            } 

            // Check if the name already exists
            for (const auto& savedFile : savedFiles) {
                if (savedFile == newWorldName) {
                    exists = true;
                    break;
                }
            }

            // If the name is not empty and does not exist, create the new world
            if (!exists && !empty) { 
                fs::rename(savedRoot + toRename, savedRoot + newWorldName);
                toRename = "";
                newWorldName[0] = '\0';
                ImGui::CloseCurrentPopup();
            } 
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            toRename = "";
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        if (exists) {
            ImGui::Spacing();
            ImGui::Text("This world name already exists!");
        }
        if (empty) {
            ImGui::Spacing();
            ImGui::Text("Empty name is not allowed!");
        }
        ImGui::EndPopup();
    }

    // New world name popup
    ImGui::SetCursorPosX(0);
    if (ImGui::BeginPopupModal("New World Name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a name for your new world:");

        ImGui::InputText("##Name", newWorldName, IM_ARRAYSIZE(newWorldName));
        
        ImGui::Spacing();
        // Centre the buttons
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            empty = false;
            exists = false;

            // Check if the name is empty
            if (newWorldName[0] == '\0') {
                empty = true;
            } 

            // Check if the name already exists
            for (const auto& savedFile : savedFiles) {
                if (savedFile == newWorldName) {
                    exists = true;
                    break;
                }
            }

            // If the name is not empty and does not exist, create the new world
            if (!exists && !empty) { 
                settings->setCurrentPage(UIPage::WorldMenuClosed);
                settings->setCurrentWorld(newWorldName);
                settings->getParameters()->setDefaultValues();
                settings->getParameters()->saveToFile(newWorldName, settings->getFilePathDelimitter());
                newWorldName[0] = '\0';
                ImGui::CloseCurrentPopup();
            } 
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            newWorldName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        if (exists) {
            ImGui::Spacing();
            ImGui::Text("This world name already exists!");
        }
        if (empty) {
            ImGui::Spacing();
            ImGui::Text("Empty name is not allowed!");
        }
        ImGui::EndPopup();
    }

    // Existing name popup
    if (ImGui::BeginPopupModal("##Existing Name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("This world name already exists!");
        ImGui::Spacing();
        // Centre the button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
            ImGui::OpenPopup("##New World Name");
        }
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    // Empty name popup
    if (ImGui::BeginPopupModal("##Empty Name", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Empty name is not allowed!");
        ImGui::Spacing();
        // Centre the button
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void UI::renderLoadingScreen(shared_ptr<Settings> settings) {
    ImGuiIO& io = ImGui::GetIO();
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;
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