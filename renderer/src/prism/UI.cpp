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
    string mainTextureRoot = getenv("MAIN_TEXTURE_ROOT");

    // Get the root directory for the previews
    string previewsRoot = getenv("PREVIEWS_ROOT");

    // Check if the preview directory exists, if not create it
    fs::path previewDir = fs::path(previewsRoot);
    if (!fs::exists(previewDir)){
        fs::create_directories(previewDir);
    }

    // Find all files in the main textures root directory that are jpg or png and contain "_diff"
    for (const auto& entry : fs::recursive_directory_iterator(mainTextureRoot)) {
        // If the file is jpg or png, add it to the list of texture files and load the preview as a texture object
        if ((entry.path().extension() == ".jpg" || entry.path().extension() == ".png") && (entry.path().string().find("_diff") != std::string::npos)) {
            textureFiles.push_back(entry.path().parent_path().filename().string());
            Texture texture = Texture(entry.path().string(), "preview", entry.path().parent_path().filename().string());
            textureHandles.push_back(texture.getId());
            previewMap[entry.path().parent_path().filename().string()] = texture.getId();
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
    
    // Regenerate button
    if (ImGui::Button("Regenerate", ImVec2(150, 0))) {
        settings->setCurrentPage(UIPage::Loading); // Set the current page to loading
    }
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
                selectedTextureIndex = -1; // Reset the selected texture index
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
    if (ImGui::CollapsingHeader("Global Parameters")) {
        ImGui::Indent(15.0f);   
        if (ImGui::CollapsingHeader("Terrain")) {
            ImGui::SliderInt("Maximum Height", &settings->getParameters()->getMaxHeight(), 0, 100);
            ImGui::SliderInt("Ocean Coverage", &settings->getParameters()->getOceanCoverage(), 0, 100);
            ImGui::SliderInt("Continent Size", &settings->getParameters()->getContinentSize(), 0, 100);
            ImGui::SliderInt("Roughness", &settings->getParameters()->getRoughness(), 0, 100);
            ImGui::SliderInt("Mountainousness", &settings->getParameters()->getMountainousness(), 0, 100);
            ImGui::SliderInt("Coastline Roughness", &settings->getParameters()->getCoastlineRoughness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Biomes")) {
            ImGui::SliderInt("Biome Size", &settings->getParameters()->getBiomeSize(), 0, 100);
            ImGui::SliderInt("Warmth", &settings->getParameters()->getWarmth(), 0, 100);
            ImGui::SliderInt("Wetness", &settings->getParameters()->getWetness(), 0, 100);
            ImGui::SliderInt("Trees Density", &settings->getParameters()->getTreesDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Rivers")) {
            ImGui::SliderInt("River Frequency", &settings->getParameters()->getRiverFrequency(), 0, 100);
            ImGui::SliderInt("River Width", &settings->getParameters()->getRiverWidth(), 0, 100);
            ImGui::SliderInt("River Depth", &settings->getParameters()->getRiverDepth(), 0, 100);
            ImGui::SliderInt("River Meandering", &settings->getParameters()->getRiverMeandering(), 0, 100);
            ImGui::SliderInt("River Smoothness", &settings->getParameters()->getRiverSmoothness(), 0, 100);
        }
        ImGui::Unindent(15.0f);
    }

    if (ImGui::CollapsingHeader("Boreal Forest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Boreal Forest", &settings->getParameters()->getBorealForestSelected());
    
        if (ImGui::CollapsingHeader("Plains ##Boreal")) {
            ImGui::SliderInt("Max Height##1", &settings->getParameters()->getBorealForestPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##1", &settings->getParameters()->getBorealForestPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##6", &settings->getParameters()->getBorealForestPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##1", &settings->getParameters()->getBorealForestPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills##Boreal")) {
            ImGui::SliderInt("Max Height##2", &settings->getParameters()->getBorealForestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##2", &settings->getParameters()->getBorealForestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##7", &settings->getParameters()->getBorealForestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##2", &settings->getParameters()->getBorealForestHillsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Mountains##Boreal")) {
            ImGui::SliderInt("Max Height##3", &settings->getParameters()->getBorealForestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##3", &settings->getParameters()->getBorealForestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##6", &settings->getParameters()->getBorealForestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##3", &settings->getParameters()->getBorealForestMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Boreal Forest Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBorealTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBorealTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBorealTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBorealTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BorealHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBorealTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }

    if (ImGui::CollapsingHeader("Grassland Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Grassland", &settings->getParameters()->getGrasslandSelected());
    
        if (ImGui::CollapsingHeader("Plains (Grassy)##Grassland")) {
            ImGui::SliderInt("Max Height##4", &settings->getParameters()->getGrasslandPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##4", &settings->getParameters()->getGrasslandPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##1", &settings->getParameters()->getGrasslandPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##4", &settings->getParameters()->getGrasslandPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills (Grassy)##Grassland")) {
            ImGui::SliderInt("Max Height##5", &settings->getParameters()->getGrasslandHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##5", &settings->getParameters()->getGrasslandHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##1", &settings->getParameters()->getGrasslandHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##5", &settings->getParameters()->getGrasslandHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Terraced Fields (Grassy)")) {
            ImGui::SliderInt("Max Height##7", &settings->getParameters()->getGrasslandTerracedFieldsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##7", &settings->getParameters()->getGrasslandTerracedFieldsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size", &settings->getParameters()->getGrasslandTerracedFieldsSize(), 0, 100);
            ImGui::SliderInt("Tree Density##7", &settings->getParameters()->getGrasslandTerracedFieldsTreeDensity(), 0, 100);
            ImGui::SliderInt("Smoothness##3", &settings->getParameters()->getGrasslandTerracedFieldsSmoothness(), 0, 100);
            ImGui::SliderInt("Number of Terraces", &settings->getParameters()->getGrasslandTerracedFieldsNumberOfTerraces(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Rocky Fields")) {
            ImGui::SliderInt("Max Height##6", &settings->getParameters()->getGrasslandRockyFieldsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##6", &settings->getParameters()->getGrasslandRockyFieldsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Rock Density", &settings->getParameters()->getGrasslandRockyFieldsRockiness(), 0, 100);
            ImGui::SliderInt("Tree Density##6", &settings->getParameters()->getGrasslandRockyFieldsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Grassy Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrassyLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrassyMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrassyMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrassyHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyTextureHigh(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Rocky Field Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyStoneTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrasseStoneLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyStoneTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyStoneTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrassyStoneMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyStoneTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyStoneTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrasseStoneMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyStoneTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getGrassyStoneTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##GrasseStoneHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setGrassyStoneTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }

    if (ImGui::CollapsingHeader("Tundra Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Tundra", &settings->getParameters()->getTundraSelected());

        if (ImGui::CollapsingHeader("Plains (Snowy)##Tundra")) {
            ImGui::SliderInt("Max Height##8", &settings->getParameters()->getTundraPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##8", &settings->getParameters()->getTundraPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##2", &settings->getParameters()->getTundraPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##8", &settings->getParameters()->getTundraPlainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Blunt Mountains (Snowy)")) {
            ImGui::SliderInt("Max Height##9", &settings->getParameters()->getTundraBluntMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##9", &settings->getParameters()->getTundraBluntMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##1", &settings->getParameters()->getTundraBluntMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##9", &settings->getParameters()->getTundraBluntMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Pointy Peaks (Icy)")) {
            ImGui::SliderInt("Max Height##10", &settings->getParameters()->getTundraPointyMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##10", &settings->getParameters()->getTundraPointyMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Steepness##1", &settings->getParameters()->getTundraPointyMountainsSteepness(), 0, 100);
            ImGui::SliderInt("Frequency", &settings->getParameters()->getTundraPointyMountainsFrequency(), 0, 100);
            ImGui::SliderInt("Tree Density##10", &settings->getParameters()->getTundraPointyMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Snowy Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSnowyTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SnowyLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSnowyTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSnowyTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SnowyMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSnowyTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSnowyTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SnowyMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSnowyTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSnowyTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SnowyHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSnowyTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Icy Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getIcyTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##IcyLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setIcyTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getIcyTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##IcyMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setIcyTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getIcyTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##IcyMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setIcyTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getIcyTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##IcyHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setIcyTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Savanna Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Savanna", &settings->getParameters()->getSavannaSelected());

        if (ImGui::CollapsingHeader("Plains##Savanna")) {
            ImGui::SliderInt("Max Height##11", &settings->getParameters()->getSavannaPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##11", &settings->getParameters()->getSavannaPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##3", &settings->getParameters()->getSavannaPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##11", &settings->getParameters()->getSavannaPlainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains")) {
            ImGui::SliderInt("Max Height##12", &settings->getParameters()->getSavannaMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##12", &settings->getParameters()->getSavannaMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##2", &settings->getParameters()->getSavannaMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##12", &settings->getParameters()->getSavannaMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Savanna Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SavannaLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SavannaMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SavannaMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSavannaTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SavannaHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSavannaTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Woodland Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Woodland", &settings->getParameters()->getWoodlandSelected());
        if (ImGui::CollapsingHeader("Hills##Woodland")) {
            ImGui::SliderInt("Max Height##13", &settings->getParameters()->getWoodlandHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##13", &settings->getParameters()->getWoodlandHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##2", &settings->getParameters()->getWoodlandHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##13", &settings->getParameters()->getWoodlandHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Woodland Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##WoodlandLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##WoodlandMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##WoodlandMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getWoodlandTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##WoodlandHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setWoodlandTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Tropical Rainforest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Tropical Rainforest", &settings->getParameters()->getTropicalRainforestSelected());
    
        if (ImGui::CollapsingHeader("Plains (Jungle)##TropicalRainforest")) {
            ImGui::SliderInt("Max Height##14", &settings->getParameters()->getTropicalRainforestPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##14", &settings->getParameters()->getTropicalRainforestPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##4", &settings->getParameters()->getTropicalRainforestPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##14", &settings->getParameters()->getTropicalRainforestPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Mountains (Jungle Mountains)##TropicalRainforest")) {
            ImGui::SliderInt("Max Height##15", &settings->getParameters()->getTropicalRainforestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##15", &settings->getParameters()->getTropicalRainforestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##3", &settings->getParameters()->getTropicalRainforestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##15", &settings->getParameters()->getTropicalRainforestMountainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills (Jungle)##TropicalRainforest")) {
            ImGui::SliderInt("Max Height##16", &settings->getParameters()->getTropicalRainforestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##16", &settings->getParameters()->getTropicalRainforestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##3", &settings->getParameters()->getTropicalRainforestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##16", &settings->getParameters()->getTropicalRainforestHillsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Volcanoes")) {
            ImGui::SliderInt("Max Height##17",&settings->getParameters()->getTropicalRainforestVolcanoesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##17", &settings->getParameters()->getTropicalRainforestVolcanoesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size", &settings->getParameters()->getTropicalRainforestVolcanoesSize(), 0, 100);
            ImGui::SliderInt("Tree Density##17", &settings->getParameters()->getTropicalRainforestVolcanoesTreeDensity(), 0, 100);
            ImGui::SliderInt("Thickness##2", &settings->getParameters()->getTropicalRainforestVolcanoesThickness(), 0, 100);
            ImGui::SliderInt("Density", &settings->getParameters()->getTropicalRainforestVolcanoesDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Jungle Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Jungle Mountains Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleMountainsTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMountainsLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleMountainsTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleMountainsTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMountainsMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleMountainsTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleMountainsTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMountainsMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleMountainsTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getJungleMountainsTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##JungleMountainsHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setJungleMountainsTextureHigh(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Volcanic Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getVolcanicTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##VolcanoLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setVolcanicTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getVolcanicTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##VolcanoMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setVolcanicTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getVolcanicTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##VolcanoMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setVolcanicTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getVolcanicTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##VolcanoHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setVolcanicTextureHigh(texture);
                };
                openTexturePopup = true;
            }
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Temperate Rainforest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Temperate Rainforest", &settings->getParameters()->getTemperateRainforestSelected());
    
        if (ImGui::CollapsingHeader("Hills (Temperate Rainforest)##TemperateRainforest")) {
            ImGui::SliderInt("Max Height##18", &settings->getParameters()->getTemperateRainforestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##18", &settings->getParameters()->getTemperateRainforestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##4", &settings->getParameters()->getTemperateRainforestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##18", &settings->getParameters()->getTemperateRainforestHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains (Temperate Rainforest)##TemperateRainforest")) {
            ImGui::SliderInt("Max Height##19", &settings->getParameters()->getTemperateRainforestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##19", &settings->getParameters()->getTemperateRainforestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##4", &settings->getParameters()->getTemperateRainforestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##19", &settings->getParameters()->getTemperateRainforestMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Swamp")) {
            ImGui::SliderInt("Max Height##20", &settings->getParameters()->getTemperateRainforestSwampMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##20", &settings->getParameters()->getTemperateRainforestSwampOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Wetness", &settings->getParameters()->getTemperateRainforestSwampWetness(), 0, 100);
            ImGui::SliderInt("Tree Density##20", &settings->getParameters()->getTemperateRainforestSwampTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Temperate Rainforest Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getTemperateTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getTemperateTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getTemperateTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getTemperateTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##TemperateHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setTemperateTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Swamp Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSwampTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SwampLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSwampTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSwampTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SwampMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSwampTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSwampTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SwampMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSwampTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSwampTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SwampHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSwampTextureHigh(texture);
                };
                openTexturePopup = true;
            }
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Temperate Seasonal Forest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Temperate Seasonal Forest", &settings->getParameters()->getTemperateSeasonalForestSelected());
        if (ImGui::CollapsingHeader("Hills##SeasonalForest")) {
            ImGui::SliderInt("Max Height##21", &settings->getParameters()->getTemperateSeasonalForestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##21", &settings->getParameters()->getTemperateSeasonalForestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##5", &settings->getParameters()->getTemperateSeasonalForestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##21", &settings->getParameters()->getTemperateSeasonalForestHillsTreeDensity(), 0, 100);
            ImGui::SliderInt("Autumnal Occurrence##1", &settings->getParameters()->getTemperateSeasonalForestHillsAutumnalOccurrence(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains##SeasonalForest")) {
            ImGui::SliderInt("Max Height##22", &settings->getParameters()->getTemperateSeasonalForestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##22", &settings->getParameters()->getTemperateSeasonalForestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##5", &settings->getParameters()->getTemperateSeasonalForestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##22", &settings->getParameters()->getTemperateSeasonalForestMountainsTreeDensity(), 0, 100);
            ImGui::SliderInt("Autumnal Occurrence##2", &settings->getParameters()->getTemperateSeasonalForestMountainsAutumnalOccurrence(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Seasonal Forest Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSeasonalForestTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SeasonalForestLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSeasonalForestTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSeasonalForestTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SeasonalForestMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSeasonalForestTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSeasonalForestTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SeasonalForestMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSeasonalForestTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getSeasonalForestTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##SeasonalForestHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setSeasonalForestTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Autumn Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getAutumnTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##AutumnLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setAutumnTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getAutumnTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##AutumnMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setAutumnTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getAutumnTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##AutumnMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setAutumnTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getAutumnTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##AutumnHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setAutumnTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }


    if (ImGui::CollapsingHeader("Subtropical Desert Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Subtropical Desert", &settings->getParameters()->getSubtropicalDesertSelected());
        if (ImGui::CollapsingHeader("Dunes##SubtropicalDesert")) {
            ImGui::SliderInt("Max Height##23", &settings->getParameters()->getSubtropicalDesertDunesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##23", &settings->getParameters()->getSubtropicalDesertDunesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##1", &settings->getParameters()->getSubtropicalDesertDunesSize(), 0, 100);
            ImGui::SliderInt("Tree Density##23", &settings->getParameters()->getSubtropicalDesertDunesTreeDensity(), 0, 100);
            ImGui::SliderInt("Dune Frequency", &settings->getParameters()->getSubtropicalDesertDunesDuneFrequency(), 0, 100);
            ImGui::SliderInt("Dune Waviness", &settings->getParameters()->getSubtropicalDesertDunesDuneWaviness(), 0, 100);
            ImGui::SliderInt("Bumpiness##6", &settings->getParameters()->getSubtropicalDesertDunesBumpiness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mesa")) {
            ImGui::SliderInt("Max Height##24", &settings->getParameters()->getSubtropicalDesertMesasMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##24", &settings->getParameters()->getSubtropicalDesertMesasOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##2", &settings->getParameters()->getSubtropicalDesertMesasSize(), 0, 100);
            ImGui::SliderInt("Tree Density##24", &settings->getParameters()->getSubtropicalDesertMesasTreeDensity(), 0, 100);
            ImGui::SliderInt("Number of Terraces", &settings->getParameters()->getSubtropicalDesertMesasNumberOfTerraces(), 0, 100);
            ImGui::SliderInt("Steepness##2", &settings->getParameters()->getSubtropicalDesertMesasSteepness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Ravines")) {
            ImGui::SliderInt("Max Height##25", &settings->getParameters()->getSubtropicalDesertRavinesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##25", &settings->getParameters()->getSubtropicalDesertRavinesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Density", &settings->getParameters()->getSubtropicalDesertRavinesDensity(), 0, 100);
            ImGui::SliderInt("Tree Density##25", &settings->getParameters()->getSubtropicalDesertRavinesTreeDensity(), 0, 100);
            ImGui::SliderInt("Ravine Width", &settings->getParameters()->getSubtropicalDesertRavinesRavineWidth(), 0, 100);
            ImGui::SliderInt("Smoothness##1", &settings->getParameters()->getSubtropicalDesertRavinesSmoothness(), 0, 100);
            ImGui::SliderInt("Steepness##3", &settings->getParameters()->getSubtropicalDesertRavinesSteepness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Oasis")) {
            ImGui::SliderInt("Max Height##26", &settings->getParameters()->getSubtropicalDesertOasisMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##26", &settings->getParameters()->getSubtropicalDesertOasisOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##3", &settings->getParameters()->getSubtropicalDesertOasisSize(), 0, 100);
            ImGui::SliderInt("Flatness#1", &settings->getParameters()->getSubtropicalDesertOasisFlatness(), 0, 100);
            ImGui::SliderInt("Tree Density##26", &settings->getParameters()->getSubtropicalDesertOasisTreeDensity(), 0, 100);
            ImGui::SliderInt("Dune Frequency", &settings->getParameters()->getSubtropicalDesertOasisDuneFrequency(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Cracked")) {
            ImGui::SliderInt("Max Height##27", &settings->getParameters()->getSubtropicalDesertCrackedMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##27", &settings->getParameters()->getSubtropicalDesertCrackedOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##4", &settings->getParameters()->getSubtropicalDesertCrackedSize(), 0, 100);
            ImGui::SliderInt("Flatness#2", &settings->getParameters()->getSubtropicalDesertCrackedFlatness(), 0, 100);
            ImGui::SliderInt("Tree Density##27", &settings->getParameters()->getSubtropicalDesertCrackedTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Dunes Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getHotDesertTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##HotDesertLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setHotDesertTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getHotDesertTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##HotDesertMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setHotDesertTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getHotDesertTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##HotDesertMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setHotDesertTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getHotDesertTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##HotDesertHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setHotDesertTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Mesa Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getMesaTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##MesaLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setMesaTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getMesaTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##MesaMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setMesaTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getMesaTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##MesaMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setMesaTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getMesaTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##MesaHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setMesaTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Ravines Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBadlandsTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BadlandsLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBadlandsTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBadlandsTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BadlandsMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBadlandsTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBadlandsTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BadlandsMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBadlandsTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getBadlandsTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##BadlandsHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setBadlandsTextureHigh(texture);
                };
                openTexturePopup = true;
            }
        }
        if (ImGui::CollapsingHeader("Oasis Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOasisTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OasisLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOasisTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOasisTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OasisMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOasisTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOasisTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OasisMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOasisTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOasisTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OasisHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOasisTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Cracked Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getDustyTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##DustyLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDustyTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getDustyTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##DustyMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDustyTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getDustyTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##DustyMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDustyTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getDustyTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##DustyHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setDustyTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        ImGui::Unindent(15.0f);
    }
    if (ImGui::CollapsingHeader("Ocean Parameters")) {
        ImGui::Indent(15.0f);
        if (ImGui::CollapsingHeader("Flat Seabed")) {
            ImGui::SliderInt("Max Height##28", &settings->getParameters()->getOceanFlatSeabedMaxHeight(), 0, 100);
            ImGui::SliderInt("Evenness##5", &settings->getParameters()->getOceanFlatSeabedEvenness(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##28", &settings->getParameters()->getOceanFlatSeabedOccurrenceProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Volcanic Islands")) {
            ImGui::SliderInt("Max Height##29", &settings->getParameters()->getOceanVolcanicIslandsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##29", &settings->getParameters()->getOceanVolcanicIslandsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##6", &settings->getParameters()->getOceanVolcanicIslandsSize(), 0, 100);
            ImGui::SliderInt("Thickness##1", &settings->getParameters()->getOceanVolcanicIslandsThickness(), 0, 100);
            ImGui::SliderInt("Density##2", &settings->getParameters()->getOceanVolcanicIslandsDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Trenches")) {
            ImGui::SliderInt("Density##3", &settings->getParameters()->getOceanTrenchesDensity(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##31", &settings->getParameters()->getOceanTrenchesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Trench Width", &settings->getParameters()->getOceanTrenchesTrenchWidth(), 0, 100);
            ImGui::SliderInt("Smoothness##2", &settings->getParameters()->getOceanTrenchesSmoothness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Water Stacks")) {
            ImGui::SliderInt("Max Height##30", &settings->getParameters()->getOceanWaterStacksMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##30", &settings->getParameters()->getOceanWaterStacksOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##7", &settings->getParameters()->getOceanWaterStacksSize(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Ocean Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOceanTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OceanLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOceanTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOceanTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OceanMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOceanTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOceanTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OceanMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOceanTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getOceanTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##OceanHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setOceanTextureHigh(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Spacing();
        }
        if (ImGui::CollapsingHeader("Water Stacks Textures")) {
            ImGui::Text("Low Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getCliffsTextureLow()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##CliffsLow")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setCliffsTextureLow(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Flat Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getCliffsTextureMidFlat()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##CliffsMidFlat")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setCliffsTextureMidFlat(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("Steep Mid-ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getCliffsTextureMidSteep()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##CliffsMidSteep")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setCliffsTextureMidSteep(texture);
                };
                openTexturePopup = true;
            }
            ImGui::Text("High Ground:");
            ImGui::SameLine(230);
            ImGui::Image(previewMap[settings->getParameters()->getCliffsTextureHigh()], ImVec2(50, 50));
            ImGui::SameLine();
            if (ImGui::Button("Change Texture##CliffsHigh")) {
                setTextureCallback = [&settings](std::string texture) {
                    settings->getParameters()->setCliffsTextureHigh(texture);
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




void UI::renderHomepage(shared_ptr<Settings> settings) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("TerraInfinity", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

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
    string projectRoot = getenv("PROJECT_ROOT");
    string savedRoot = projectRoot + settings->getFilePathDelimitter() + "saves" + settings->getFilePathDelimitter();
    vector<string> savedFiles;

    // If saves directory does not exist, create it
    if (!fs::exists(savedRoot)) {
        fs::create_directories(savedRoot);
    }

    for (const auto& entry : fs::recursive_directory_iterator(savedRoot)) {
        // Check if the entry is a file and has the .json extension
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            savedFiles.push_back(entry.path().stem().string());
        }
    }
    static string toDelete = "";
    static string toRename = "";

    // Display the saved worlds as buttons
    for (string savedFile : savedFiles) {
        if (ImGui::Button(savedFile.c_str(), ImVec2(1750, 0))) {
            settings->getParameters()->loadFromFile(savedFile, settings->getFilePathDelimitter());
            settings->setCurrentWorld(savedFile);
            settings->setCurrentPage(UIPage::Loading);
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
            if (fs::remove_all(savedRoot + toDelete)) {
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

            // If the name is not empty and a world does not already exist with that name, rename the world
            if (!exists && !empty) { 
                fs::rename(savedRoot + toRename, savedRoot + newWorldName); // Rename the directory
                // Rename the JSON file
                fs::rename(savedRoot + newWorldName + settings->getFilePathDelimitter() + toRename + ".json", savedRoot + newWorldName + settings->getFilePathDelimitter() + newWorldName + ".json");
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
                settings->setCurrentWorld(newWorldName);
                settings->getParameters()->setDefaultValues(newWorldName);
                settings->getParameters()->saveToFile(newWorldName, settings->getFilePathDelimitter());
                newWorldName[0] = '\0';
                settings->setCurrentPage(UIPage::Loading);
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

    std::string loadingText = "Generating world '" + settings->getCurrentWorld() + "'";
    for (int i = 0; i < dotCount; ++i) loadingText += ".";

    
    ImGui::SetCursorPosX((settings->getWindowWidth() - ImGui::CalcTextSize("Generating World").x) / 2);
    ImGui::SetCursorPosY((settings->getWindowHeight() - ImGui::CalcTextSize("Generating World").y) / 2);
    ImGui::Text("%s", loadingText.c_str());

    ImGui::End();

    // Render the UI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}