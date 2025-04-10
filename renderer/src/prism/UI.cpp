/**
 * This file contains a class for the UI object that will be used to control the renderer and customise the terrain. 
 */

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>

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
#include <fontawesome/IconsFontAwesome4.h>

#include "Object.hpp"
#include "Settings.hpp"
#include "UI.hpp"
#include "Window.hpp"
#include "Texture.hpp"

using namespace std;
namespace fs = std::filesystem;
  
UI::UI(GLFWwindow *context) {
    printf("Initialising the UI\n");
    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    ImGui::GetIO().IniFilename = nullptr; // Disable saving/loading of .ini file

    // Get the root directory for the diffuse textures
    string diffuseTextureRoot = getenv("DIFFUSE_TEXTURE_ROOT");

    // Get the root directory for the previews
    string previewsRoot = getenv("PREVIEWS_ROOT");

    // Check if the preview directory exists, if not create it
    fs::path previewDir = fs::path(previewsRoot);
    if (!fs::exists(previewDir)){
        fs::create_directories(previewDir);
    }

    // Find all files in the diffuse textures root directory
    for (const auto& entry : fs::recursive_directory_iterator(diffuseTextureRoot)) {
        // If the file is jpg or png, add it to the list of texture files and load the preview as a texture object
        if (entry.path().extension() == ".jpg" || entry.path().extension() == ".png") {
            textureFiles.push_back(entry.path().filename().string());
            Texture texture = Texture(entry.path().string(), "preview", entry.path().filename().string());
            textureHandles.push_back(texture.getId());
            previewMap[entry.path().filename().string()] = texture.getId();
        }
    }

    // Disable keyboard and gamepad navigation
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~(ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad);
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;


    string fontRoot = getenv("FONT_ROOT");

    // Set the ImGui font
    io.Fonts->AddFontFromFileTTF((std::string(fontRoot) + "FunnelSans-Regular.ttf").c_str(), 30.0f);

    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 30.0f; // Use if you want to make the icon monospaced
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF((std::string(fontRoot) + "fontawesome-webfont.ttf").c_str(), 30.0f, &config, icon_ranges);

    // Set the ImGui style
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base style 
    ImGui::StyleColorsDark();

    colors[ImGuiCol_WindowBg]       = ImVec4(0.02f, 0.05f, 0.05f, 0.95f);
    colors[ImGuiCol_ChildBg]         = ImVec4(0.01f, 0.03f, 0.03f, 0.7f); 

    // Text
    colors[ImGuiCol_Text]            = ImVec4(0.9f, 0.98f, 0.98f, 1.0f);  
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
    colors[ImGuiCol_Border]          = ImVec4(0.06f, 0.15f, 0.15f, 0.6f);
    colors[ImGuiCol_BorderShadow]    = ImVec4(0.0f, 0.0f, 0.0f, 0.1f);

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

void UI::render(shared_ptr<Settings> settings, float fps, glm::vec3 playerPos) {

    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::SetNextWindowCollapsed(settings->getCurrentPage() != UIPage::WorldMenuOpen, ImGuiCond_Always);

    // Title of the menu window set to the current world name
    std::string title = settings->getCurrentWorld();
    
    // Add the FPS and player position to the title for debugging
    if (settings->getCurrentPage() == UIPage::WorldMenuClosed) {
        title += " - FPS: ";
        title += to_string(static_cast<int>(std::round(fps)));
        title += " - Pos: (" + to_string(static_cast<int>(std::ceil(playerPos.x)));
        title += ", " + to_string(static_cast<int>(std::ceil(playerPos.y)));
        title += ", " + to_string(static_cast<int>(std::ceil(playerPos.z))) + ")";
        // Compute the chunk that the player is in
        // Add size /2 to the player position to account for the translation transformation
        int chunkX;
        int chunkZ;
        chunkX = static_cast<int>(floor((playerPos.x) / settings->getChunkSize()));
        chunkZ = static_cast<int>(floor((playerPos.z) / settings->getChunkSize()));
        title += " - Chunk: (" + to_string(chunkX) + ", " + to_string(chunkZ) + ")";
    } 

    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    
    if (ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuOpen)
    {
        settings->setCurrentPage(UIPage::WorldMenuClosed);
    } else if (!ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuClosed)
    {
        settings->setCurrentPage(UIPage::WorldMenuOpen);
    }
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.65f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.75f, 1.0f));
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
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 60);
    if (ImGui::Button(ICON_FA_HOME, ImVec2(50, 0))){
        // Open popup to confirm going home
        ImGui::OpenPopup("Return Home Confirmation");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Return to homepage");
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2);
    ImGui::Spacing();

    ImGui::SetCursorPosX(0);
    if (ImGui::BeginPopupModal("Save Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Changes saved successfully!");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
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
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopupModal("Return Home Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Are you sure you want to return home? Any unsaved changes will be lost.");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            settings->setCurrentPage(UIPage::Home);
            settings->setCurrentWorld("");
            ImGui::GetStateStorage()->Clear();
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
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

    // Change the background colours for the popup
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.02f, 0.05f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.01f, 0.03f, 0.03f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Handle Texture Selection Popup
    if (ImGui::BeginPopupModal("Texture Selection", &openTexturePopup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
        // Get the index of the texture in textureFiles matching a string
        static int selectedTextureIndex = -1;

        // Texture grid display
        float thumbnailSize = 120.0f;
        float panelWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize - 2.0f; 
        int columns = static_cast<int>(panelWidth / (thumbnailSize + 10.0f));
        if (columns < 1) columns = 1;

        // Make this a child of the popup so that the table is scrollable
        ImGui::BeginChild("TextureTableScroll", ImVec2(0, 500), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
        float scrollbarSize = ImGui::GetStyle().ScrollbarSize;
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - scrollbarSize - 2.0f); 
        if (ImGui::BeginTable("TextureTable", columns)) {
            for (size_t i = 0; i < textureFiles.size(); i++) {
                ImGui::TableNextColumn();
                
                // Create selectable texture preview
                ImGui::PushID(static_cast<int>(i));
                bool isSelected = (selectedTextureIndex == static_cast<int>(i));
                
                // Set the tint and border color based on selection
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
        ImGui::PopItemWidth();
        ImGui::EndChild();

        ImGui::Spacing();

        // Centre the buttons
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 240) / 2);
  
        // Confirm texture change button 
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            if (selectedTextureIndex >= 0 && selectedTextureIndex < static_cast<int>(textureFiles.size())) {

                // Call the callback function to set the selected texture
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
    ImGui::PopStyleColor(4); // Pop the styles for the popup
    
    ImGui::Spacing();
    
    // Create a child window for the settings so it is scrollable

    // Make the child window transparent
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    ImGui::BeginChild("Settings", ImVec2(0, ImGui::GetWindowHeight() - 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PushItemWidth(300);
    if (ImGui::CollapsingHeader("Water Settings")) {
        ImGui::SliderInt("Ocean Coverage", &settings->getParameters()->getOceanCoverage(), 0, 100);
        ImGui::SliderInt("Sea Level",&settings->getParameters()->getSeaLevel(), 0, 100);
        ImGui::SliderInt("Continent Size", &settings->getParameters()->getContinentSize(), 0, 100);
        ImGui::SliderInt("River Width", &settings->getParameters()->getRiverWidth(), 0, 100);
        ImGui::SliderInt("River Depth", &settings->getParameters()->getRiverDepth(), 0, 100);
        ImGui::SliderInt("River Frequency", &settings->getParameters()->getRiverFrequency(), 0, 100);
        ImGui::SliderInt("River Length", &settings->getParameters()->getRiverLength(), 0, 100);
        ImGui::SliderInt("River Variety", &settings->getParameters()->getRiverVariety(), 0, 100);
        ImGui::SliderInt("Lake Frequency", &settings->getParameters()->getLakeFrequency(), 0, 100);
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
        ImGui::Indent(15.0f);
        if (ImGui::CollapsingHeader("Subtropical Desert")) {
            ImGui::SliderInt("Chance of occurring##1", &settings->getParameters()->getDesertProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getDesertTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SubtropicalDesert1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDesertTexture1(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getDesertTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SubtropicalDesert2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDesertTexture2(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getDesertTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SubtropicalDesert3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDesertTexture3(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getDesertTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SubtropicalDesert4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDesertTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Temperate Forest")) {
            ImGui::SliderInt("Chance of occurring##2", &settings->getParameters()->getTemperateForestProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateForestTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateForest1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateForestTexture1(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateForestTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateForest2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateForestTexture2(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateForestTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateForest3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateForestTexture3(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateForestTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateForest4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateForestTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Tropical Rainforest")) {
            ImGui::SliderInt("Chance of occurring##3", &settings->getParameters()->getTropicalRainforestProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTropicalRainforestTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TropicalRainforest1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTropicalRainforestTexture1(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTropicalRainforestTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TropicalRainforest2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTropicalRainforestTexture2(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTropicalRainforestTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TropicalRainforest3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTropicalRainforestTexture3(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture    
            ImGui::Image(previewMap[settings->getParameters()->getTropicalRainforestTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TropicalRainforest4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTropicalRainforestTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Savanna")) {
            ImGui::SliderInt("Chance of occurring##4", &settings->getParameters()->getSavannaProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Savanna1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTexture1(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Savanna2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTexture2(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Savanna3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTexture3(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Savanna4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Temperate Rainforest")) {
            ImGui::SliderInt("Chance of occurring##5", &settings->getParameters()->getTemperateRainforestProbability(), 0, 100);    
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateRainforestTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateRainforest1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateRainforestTexture1(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateRainforestTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateRainforest2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateRainforestTexture2(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateRainforestTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateRainforest3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateRainforestTexture3(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTemperateRainforestTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateRainforest4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateRainforestTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Boreal Forest")) {
            ImGui::SliderInt("Chance of occurring##6", &settings->getParameters()->getBorealForestProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getBorealForestTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealForest1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealForestTexture1(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getBorealForestTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealForest2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealForestTexture2(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getBorealForestTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealForest3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealForestTexture3(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getBorealForestTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealForest4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealForestTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Grassland")) {
            ImGui::SliderInt("Chance of occurring##7", &settings->getParameters()->getGrasslandProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getGrasslandTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Grassland1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrasslandTexture1(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getGrasslandTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Grassland2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrasslandTexture2(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getGrasslandTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Grassland3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrasslandTexture3(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getGrasslandTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Grassland4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrasslandTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }        
        if (ImGui::CollapsingHeader("Woodland")) {
            ImGui::SliderInt("Chance of occurring##8", &settings->getParameters()->getWoodlandProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Woodland1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTexture1(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Woodland2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTexture2(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Woodland3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTexture3(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("High ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Woodland4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }        
        if (ImGui::CollapsingHeader("Tundra")) {
            ImGui::SliderInt("Chance of occurring##9", &settings->getParameters()->getTundraProbability(), 0, 100);
            ImGui::Spacing();
            ImGui::Text("Low ground:");
            ImGui::SameLine(230);
            // Show a preview of the texture    
            ImGui::Image(previewMap[settings->getParameters()->getTundraTexture1()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Tundra1")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTundraTexture1(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, flat:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTundraTexture2()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Tundra2")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTundraTexture2(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("Mid ground, steep:");
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTundraTexture3()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Tundra3")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTundraTexture3(texture);
                };
                openTexturePopup = true;
            }
        
            ImGui::Text("High ground:"); 
            ImGui::SameLine(230);
            // Show a preview of the texture
            ImGui::Image(previewMap[settings->getParameters()->getTundraTexture4()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##Tundra4")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTundraTexture4(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }        
        ImGui::Unindent(15.0f);
    }
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor(3);

    ImGui::End();

    //Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


long UI::generateRandomSeed(){
    // Get the current time without using time function and initialise srand
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    srand(millis);
    int msbRandom = rand();
    int lsbRandom = rand();
    uint64_t u_seed = (static_cast<uint64_t>(msbRandom) << 32) | static_cast<uint64_t>(lsbRandom);
    long seed = static_cast<long>(u_seed);
    return seed;
}

void UI::renderHomepage(shared_ptr<Settings> settings) {
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
        ImGui::OpenPopup("New World Name");
    }
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Generate a new world");
        ImGui::EndTooltip();
    }

    ImGui::Dummy(ImVec2(0, 20));

    // Display the scrollable list of saved worlds
    ImGui::Text("Your saved worlds:");
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
        if (ImGui::Button(savedFile.c_str(), ImVec2(1750, 0))) {
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

        // Add a rename button next to each saved world
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.65f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.75f, 1.0f));
        if (ImGui::Button((ICON_FA_PENCIL + std::string("##Rename ") + savedFile).c_str(), ImVec2(50, 0))) {
            // Set the toRename variable to the selected file name
            toRename = savedFile;
        };
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Rename %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor(2);

        ImGui::SameLine();
        // Add a red delete button next to each saved world
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6, 0.2, 0.2, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));

        if (ImGui::Button((ICON_FA_TRASH + std::string("##Delete ") + savedFile).c_str(), ImVec2(50, 0))) {
            // Set the toDelete variable to the selected file name
            toDelete = savedFile;
        };
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Delete %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor(2);
    }
    ImGui::EndChild();

    if (toDelete != "") {
        ImGui::OpenPopup("Delete Confirmation");
    }

    if (toRename != "") {
        ImGui::OpenPopup("Rename World");
    }

    // Delete confirmation popup
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Are you sure you want to delete this world?");
        // Centre the button
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            // Delete the file
            if (fs::remove(savedRoot + toDelete)) {
                toDelete = "";
            } else {
                cout << "Failed to delete file" << endl;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            toDelete = "";
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    static char newWorldName[128] = "";
    static bool exists = false;
    static bool empty = false;

    // Rename world popup
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
                settings->getParameters()->setSeed(generateRandomSeed()); //Generates the random seed for the new world
                std::cout << "New world seed: " << settings->getParameters()->getSeed() << std::endl;
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
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
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