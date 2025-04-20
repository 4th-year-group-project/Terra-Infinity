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

/**
 * Constructor for the UI class. This function will set up the UI and load the texture previews that will be used in the UI.
 * @param context The GLFW window context that will be used to render the UI.
 */
UI::UI(GLFWwindow *context, shared_ptr<Settings> settings) {
    // Initialize ImGui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(context, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    ImGui::GetIO().IniFilename = nullptr; // Disable saving/loading of .ini file to improve performance

    // Get the main texture root directory from the environment variable
    string mainTextureRoot = getenv("MAIN_TEXTURE_ROOT");

    // Get the root directory for the texture previews
    string previewsRoot = getenv("PREVIEWS_ROOT");

    // Check if the preview directory exists, if not create it
    fs::path previewDir = fs::path(previewsRoot);
    if (!fs::exists(previewDir)){
        fs::create_directories(previewDir);
    }

    vector<string> diffTextureNames = {"_diff", "_Color","_color","_albedo"}; // A vector to hold common names for diffuse textures

    // Find all diffuse texture files in the main textures root directory that are of type JPG or PNG and are diffuse textures
    for (const auto& entry : fs::recursive_directory_iterator(mainTextureRoot)) {
        // Check if the file has a JPG or PNG extension
        if ((entry.path().extension() == ".jpg" || entry.path().extension() == ".png")) {
            // Check if the filename contains any of the specified types from the list of diffuse texture indicators
            for (const auto& t : diffTextureNames) {
                if (entry.path().filename().string().find(t) != std::string::npos) {
                    // Add the folder name (texture name) to the textureFiles vector
                    textureFiles.push_back(entry.path().parent_path().filename().string());

                    // Create a Texture object for the preview and add its ID to the textureHandles vector
                    Texture texture = Texture(entry.path().string(), "preview", entry.path().parent_path().filename().string());
                    textureHandles.push_back(texture.getId());

                    // Add a mapping from the folder name to the texture ID for the preview
                    previewMap[entry.path().parent_path().filename().string()] = texture.getId();
    
                    break; // Break out of the loop once a match is found
                }
            }
        }
    }

    string textureRoot = getenv("TEXTURE_ROOT");
    // Load the logo texture
    logoTexture = Texture((std::string(textureRoot) + settings->getFilePathDelimitter() + "logo.png").c_str(), "logo", "logo");

    // Disable keyboard and gamepad navigation
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags &= ~(ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_NavEnableGamepad);
    io.WantCaptureMouse = true;
    io.WantCaptureKeyboard = true;

    // Get the font root directory from the environment variable
    string fontRoot = getenv("FONT_ROOT");

    // Set the custom font to be used in the UI
    io.Fonts->AddFontFromFileTTF((std::string(fontRoot) + "FunnelSans-Regular.ttf").c_str(), 30.0f);

    // Load the FontAwesome font for icons
    ImFontConfig config;
    config.MergeMode = true;
    config.GlyphMinAdvanceX = 30.0f; 
    static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    io.Fonts->AddFontFromFileTTF((std::string(fontRoot) + "fontawesome-webfont.ttf").c_str(), 30.0f, &config, icon_ranges);

    // Set the ImGui style colours
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // Base style of dark theme
    ImGui::StyleColorsDark();

    // Background
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

    // More style tweaks
    style.FrameRounding = 4.0f;
    style.WindowRounding = 5.0f;
    style.GrabRounding = 3.0f;
    style.ScrollbarSize = 14.0f;
}



/**
 * Destructor for the UI class. This function will clean up the ImGui context and shutdown the ImGui backend.
 */
UI::~UI() {
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    };
}

/**
 * This function will render the UI for the texture selection submenu of a given texture group, including low ground, flat mid-ground, steep mid-ground and high ground textures.
 * @param labelPrefix The prefix for the labels of the textures.
 * @param textureLow The name of the low ground texture.
 * @param textureMidFlat The name of the flat mid-ground texture.
 * @param textureMidSteep The name of the steep mid-ground texture.
 * @param textureHigh The name of the high ground texture.
 * @param setLowCallback The callback function that will be called when the low ground texture is changed.
 * @param setMidFlatCallback The callback function that will be called when the flat mid-ground texture is changed.
 * @param setMidSteepCallback The callback function that will be called when the steep mid-ground texture is changed.
 * @param setHighCallback The callback function that will be called when the high ground texture is changed.
 */
void UI::drawTextureSelectionSection(
    const std::string& labelPrefix,
    const std::string& textureLow,
    const std::string& textureMidFlat,
    const std::string& textureMidSteep,
    const std::string& textureHigh,
    std::function<void(const std::string&)> setLowCallback,
    std::function<void(const std::string&)> setMidFlatCallback,
    std::function<void(const std::string&)> setMidSteepCallback,
    std::function<void(const std::string&)> setHighCallback
) {
    auto drawOne = [&](const std::string& label, const std::string& textureName, const std::string& buttonLabel, std::function<void(const std::string&)> callback) {
        ImGui::Text("%s:", label.c_str());
        ImGui::SameLine(230);
        ImGui::Image(previewMap[textureName], ImVec2(50, 50));
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("%s",textureName.c_str());
            ImGui::EndTooltip();
        }
        ImGui::SameLine();
        if (ImGui::Button(buttonLabel.c_str())) {
            setTextureCallback = callback;
            openTexturePopup = true;
        }
    };

    drawOne("Low Ground", textureLow, "Change Texture##" + labelPrefix + "Low", setLowCallback);
    drawOne("Flat Mid-ground", textureMidFlat, "Change Texture##" + labelPrefix + "MidFlat", setMidFlatCallback);
    drawOne("Steep Mid-ground", textureMidSteep, "Change Texture##" + labelPrefix + "MidSteep", setMidSteepCallback);
    drawOne("High Ground", textureHigh, "Change Texture##" + labelPrefix + "High", setHighCallback);
    
    ImGui::Spacing();
}


/**
 * This function will render the UI for the main screen.
 * @param settings The settings object that contains the current settings for the renderer.
 * @param fps The current frames per second of the renderer used for debugging.
 * @param playerPos The current position of the player in the world used for debugging.
 */
void UI::renderMain(shared_ptr<Settings> settings, float, glm::vec3) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the UI window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getUIWidth(), settings->getWindowHeight()));  // Full height of the window

    // Set collapsed state based on whether the current page is WorldMenuOpen or not
    ImGui::SetNextWindowCollapsed(settings->getCurrentPage() != UIPage::WorldMenuOpen, ImGuiCond_Always);

    // Title of the menu window set to the current world name
    std::string title; 

    // If the world menu is closed, set the title to include "[Tab] Menu" 
    if (settings->getCurrentPage() == UIPage::WorldMenuClosed) {
        title = "[Tab] Menu | " + settings->getCurrentWorld();
    } else {
        title = settings->getCurrentWorld();
    }
    
    // // Add the FPS and player position to the title for debugging
    // if (settings->getCurrentPage() == UIPage::WorldMenuClosed) {
    //     title += " - FPS: ";
    //     title += to_string(static_cast<int>(std::round(fps)));
    //     title += " - Pos: (" + to_string(static_cast<int>(std::ceil(playerPos.x)));
    //     title += ", " + to_string(static_cast<int>(std::ceil(playerPos.y)));
    //     title += ", " + to_string(static_cast<int>(std::ceil(playerPos.z))) + ")";
    //     // Compute the chunk that the player is in
    //     // Add size /2 to the player position to account for the translation transformation
    //     int chunkX;
    //     int chunkZ;
    //     chunkX = static_cast<int>(floor((playerPos.x) / settings->getChunkSize()));
    //     chunkZ = static_cast<int>(floor((playerPos.z) / settings->getChunkSize()));
    //     title += " - Chunk: (" + to_string(chunkX) + ", " + to_string(chunkZ) + ")";
    // } 

    ImGui::Begin(title.c_str(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar);
    
    // Update window state if the user collapses or expands the window manually
    if (ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuOpen)
    {
        settings->setCurrentPage(UIPage::WorldMenuClosed);
    } else if (!ImGui::IsWindowCollapsed() && settings->getCurrentPage() == UIPage::WorldMenuClosed)
    {
        settings->setCurrentPage(UIPage::WorldMenuOpen);
    }
    
    // Set colour to purple tones for the top buttons
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.65f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.75f, 1.0f));
    
    // Regenerate button
    if (ImGui::Button("Regenerate", ImVec2(150, 0))) {
        settings->setCurrentPage(UIPage::Loading); // Set the current page to loading
    }
    // If the button is hovered, display a tooltip
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Regenerate the world using the current settings");
        ImGui::EndTooltip();
    }
    ImGui::SameLine();

    // Save button
    if (ImGui::Button("Save", ImVec2(150, 0)))
    {
        // Save the current parameter settings to a JSON file
        if (settings->getParameters()->saveToFile(settings->getCurrentWorld(), settings->getFilePathDelimitter())) {
            // If successful, open the save confirmation popup
            ImGui::OpenPopup("Save Confirmation");
        } else {
            // If failed, open the save failed popup
            ImGui::OpenPopup("Save Failed");
        }
    }
    // If the button is hovered, display a tooltip
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Save the current world settings");
        ImGui::EndTooltip();
    }
    ImGui::SameLine();

    // Shift the home button to the right of the menu
    ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 60);
    if (ImGui::Button(ICON_FA_HOME, ImVec2(50, 0))){
        // Open popup to confirm going home
        ImGui::OpenPopup("Return Home Confirmation");
    }
    // If the button is hovered, display a tooltip
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Return to homepage");
        ImGui::EndTooltip();
    }
    ImGui::PopStyleColor(2); // Pop the purple button colours
    ImGui::Spacing();

    ImGui::SetCursorPosX(0);
    // Save Confirmation Popup
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

    // Save Failed Popup
    if (ImGui::BeginPopupModal("Save Failed", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("There was a problem saving the changes!");
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 120) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    // Return Home Confirmation Popup
    if (ImGui::BeginPopupModal("Return Home Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Are you sure you want to return home? Any unsaved changes will be lost.");
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            // Update current page to home, set current world to empty string and clear ImGui state
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

    // If openTexturePopup is true, open the texture selection popup in a new window
    if (openTexturePopup) {
        ImGui::OpenPopup("Texture Selection");
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
    }

    // Adjust the background colours for the popup
    ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.02f, 0.05f, 0.05f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.01f, 0.03f, 0.03f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Texture Selection Popup
    if (ImGui::BeginPopupModal("Texture Selection", &openTexturePopup, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse)) {
        // Initialise the selected texture index to -1
        static int selectedTextureIndex = -1;

        // Texture previews table display
        float thumbnailSize = 120.0f;
        float panelWidth = ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ScrollbarSize - 2.0f;  // Leave space for scrollbar
        int columns = static_cast<int>(panelWidth / (thumbnailSize + 10.0f)); // Calculate the number of columns based on the available width
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
              
                // Show a preview image of the texture based on the texture ID
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

                // Call the texture callback function to set the selected texture appropriately
                setTextureCallback(textureFiles[selectedTextureIndex]);
                
                openTexturePopup = false; // Close the popup
                selectedTextureIndex = -1; // Reset the selected texture index
                ImGui::CloseCurrentPopup();
            }
        }

        ImGui::SameLine();
        
        // Cancel button
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            openTexturePopup = false; // Close the popup
            selectedTextureIndex = -1; // Reset the selected texture index
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
    ImGui::PopStyleColor(4); // Pop the styles for the popup
    
    ImGui::Spacing();
    
    

    // Make the Settings child window transparent
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

    // Create a child window for the settings so it is scrollable
    ImGui::BeginChild("Settings", ImVec2(0, ImGui::GetWindowHeight() - 100.0f), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
    ImGui::PushItemWidth(300); // Set the width of the items in the child window
    // Global parameters
    if (ImGui::CollapsingHeader("Global Parameters")) {
        ImGui::Indent(15.0f);   
        if (ImGui::CollapsingHeader("Terrain")) {
            ImGui::SliderInt("Maximum Height", &settings->getParameters()->getGlobalMaxHeight(), 0, 100);
            ImGui::SliderInt("Ocean Coverage", &settings->getParameters()->getOceanCoverage(), 0, 100);
            ImGui::SliderInt("Continent Size", &settings->getParameters()->getContinentSize(), 0, 100);
            ImGui::SliderInt("Ruggedness", &settings->getParameters()->getGlobalRuggedness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Biomes")) {
            ImGui::SliderInt("Biome Size", &settings->getParameters()->getBiomeSize(), 0, 100);
            ImGui::SliderInt("Warmth", &settings->getParameters()->getWarmth(), 0, 100);
            ImGui::SliderInt("Wetness", &settings->getParameters()->getWetness(), 0, 100);
            ImGui::SliderInt("Tree Density", &settings->getParameters()->getGlobalTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Rivers")) {
            ImGui::SliderInt("River Frequency", &settings->getParameters()->getRiverFrequency(), 0, 100);
            ImGui::SliderInt("River Width", &settings->getParameters()->getRiverWidth(), 0, 100);
            ImGui::SliderInt("River Depth", &settings->getParameters()->getRiverDepth(), 0, 100);
            ImGui::SliderInt("River Meandering", &settings->getParameters()->getRiverMeandering(), 0, 100);
        }
        ImGui::Unindent(15.0f);
    }

    // Boreal Forest parameters
    if (ImGui::CollapsingHeader("Boreal Forest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Boreal Forest", &settings->getParameters()->getBorealForestSelected());
    
        if (ImGui::CollapsingHeader("Plains ##Boreal")) {
            ImGui::SliderInt("Maximum Height##1", &settings->getParameters()->getBorealForestPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##1", &settings->getParameters()->getBorealForestPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##6", &settings->getParameters()->getBorealForestPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##1", &settings->getParameters()->getBorealForestPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills##Boreal")) {
            ImGui::SliderInt("Maximum Height##2", &settings->getParameters()->getBorealForestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##2", &settings->getParameters()->getBorealForestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##7", &settings->getParameters()->getBorealForestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##2", &settings->getParameters()->getBorealForestHillsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Mountains##Boreal")) {
            ImGui::SliderInt("Maximum Height##3", &settings->getParameters()->getBorealForestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##3", &settings->getParameters()->getBorealForestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##6", &settings->getParameters()->getBorealForestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##3", &settings->getParameters()->getBorealForestMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Boreal Forest Textures")) {
            // Draw the texture selection section for boreal forest textures
            drawTextureSelectionSection(
                "Boreal",
                settings->getParameters()->getBorealTextureLow(),
                settings->getParameters()->getBorealTextureMidFlat(),
                settings->getParameters()->getBorealTextureMidSteep(),
                settings->getParameters()->getBorealTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setBorealTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setBorealTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setBorealTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setBorealTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Grassland parameters
    if (ImGui::CollapsingHeader("Grassland Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Grassland", &settings->getParameters()->getGrasslandSelected());
    
        if (ImGui::CollapsingHeader("Plains (Grassy)##Grassland")) {
            ImGui::SliderInt("Maximum Height##4", &settings->getParameters()->getGrasslandPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##4", &settings->getParameters()->getGrasslandPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##1", &settings->getParameters()->getGrasslandPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##4", &settings->getParameters()->getGrasslandPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills (Grassy)##Grassland")) {
            ImGui::SliderInt("Maximum Height##5", &settings->getParameters()->getGrasslandHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##5", &settings->getParameters()->getGrasslandHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##1", &settings->getParameters()->getGrasslandHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##5", &settings->getParameters()->getGrasslandHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Terraced Fields (Grassy)")) {
            ImGui::SliderInt("Maximum Height##7", &settings->getParameters()->getGrasslandTerracedFieldsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##7", &settings->getParameters()->getGrasslandTerracedFieldsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size", &settings->getParameters()->getGrasslandTerracedFieldsSize(), 0, 100);
            ImGui::SliderInt("Tree Density##7", &settings->getParameters()->getGrasslandTerracedFieldsTreeDensity(), 0, 100);
            ImGui::SliderInt("Smoothness##3", &settings->getParameters()->getGrasslandTerracedFieldsSmoothness(), 0, 100);
            ImGui::SliderInt("Number of Terraces", &settings->getParameters()->getGrasslandTerracedFieldsNumberOfTerraces(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Rocky Fields")) {
            ImGui::SliderInt("Maximum Height##6", &settings->getParameters()->getGrasslandRockyFieldsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##6", &settings->getParameters()->getGrasslandRockyFieldsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Rock Density", &settings->getParameters()->getGrasslandRockyFieldsRockiness(), 0, 100);
            ImGui::SliderInt("Tree Density##6", &settings->getParameters()->getGrasslandRockyFieldsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Grassy Textures")) {
            // Draw the texture selection section for grassy textures
            drawTextureSelectionSection(
                "Grassy",
                settings->getParameters()->getGrassyTextureLow(),
                settings->getParameters()->getGrassyTextureMidFlat(),
                settings->getParameters()->getGrassyTextureMidSteep(),
                settings->getParameters()->getGrassyTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setGrassyTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Rocky Field Textures")) {
            // Draw the texture selection section for rocky field textures
            drawTextureSelectionSection(
                "RockyField",
                settings->getParameters()->getGrassyStoneTextureLow(),
                settings->getParameters()->getGrassyStoneTextureMidFlat(),
                settings->getParameters()->getGrassyStoneTextureMidSteep(),
                settings->getParameters()->getGrassyStoneTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setGrassyStoneTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyStoneTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyStoneTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setGrassyStoneTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Tundra parameters
    if (ImGui::CollapsingHeader("Tundra Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Tundra", &settings->getParameters()->getTundraSelected());

        if (ImGui::CollapsingHeader("Plains (Snowy)##Tundra")) {
            ImGui::SliderInt("Maximum Height##8", &settings->getParameters()->getTundraPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##8", &settings->getParameters()->getTundraPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##2", &settings->getParameters()->getTundraPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##8", &settings->getParameters()->getTundraPlainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Blunt Mountains (Snowy)")) {
            ImGui::SliderInt("Maximum Height##9", &settings->getParameters()->getTundraBluntMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##9", &settings->getParameters()->getTundraBluntMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##1", &settings->getParameters()->getTundraBluntMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##9", &settings->getParameters()->getTundraBluntMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Pointy Peaks (Icy)")) {
            ImGui::SliderInt("Maximum Height##10", &settings->getParameters()->getTundraPointyMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##10", &settings->getParameters()->getTundraPointyMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Steepness##1", &settings->getParameters()->getTundraPointyMountainsSteepness(), 0, 100);
            ImGui::SliderInt("Frequency", &settings->getParameters()->getTundraPointyMountainsFrequency(), 0, 100);
            ImGui::SliderInt("Tree Density##10", &settings->getParameters()->getTundraPointyMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Snowy Textures")) {
            // Draw the texture selection section for snowy textures
            drawTextureSelectionSection(
                "Snowy",
                settings->getParameters()->getSnowyTextureLow(),
                settings->getParameters()->getSnowyTextureMidFlat(),
                settings->getParameters()->getSnowyTextureMidSteep(),
                settings->getParameters()->getSnowyTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setSnowyTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setSnowyTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setSnowyTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setSnowyTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Icy Textures")) {
            // Draw the texture selection section for icy textures
            drawTextureSelectionSection(
                "Icy",
                settings->getParameters()->getIcyTextureLow(),
                settings->getParameters()->getIcyTextureMidFlat(),
                settings->getParameters()->getIcyTextureMidSteep(),
                settings->getParameters()->getIcyTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setIcyTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setIcyTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setIcyTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setIcyTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Savanna parameters
    if (ImGui::CollapsingHeader("Savanna Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Savanna", &settings->getParameters()->getSavannaSelected());

        if (ImGui::CollapsingHeader("Plains##Savanna")) {
            ImGui::SliderInt("Maximum Height##11", &settings->getParameters()->getSavannaPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##11", &settings->getParameters()->getSavannaPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##3", &settings->getParameters()->getSavannaPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##11", &settings->getParameters()->getSavannaPlainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains")) {
            ImGui::SliderInt("Maximum Height##12", &settings->getParameters()->getSavannaMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##12", &settings->getParameters()->getSavannaMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##2", &settings->getParameters()->getSavannaMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##12", &settings->getParameters()->getSavannaMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Savanna Textures")) {
            // Draw the texture selection section for savanna textures
            drawTextureSelectionSection(
                "Savanna",
                settings->getParameters()->getSavannaTextureLow(),
                settings->getParameters()->getSavannaTextureMidFlat(),
                settings->getParameters()->getSavannaTextureMidSteep(),
                settings->getParameters()->getSavannaTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setSavannaTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setSavannaTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setSavannaTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setSavannaTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Woodland parameters
    if (ImGui::CollapsingHeader("Woodland Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Woodland", &settings->getParameters()->getWoodlandSelected());
        if (ImGui::CollapsingHeader("Hills##Woodland")) {
            ImGui::SliderInt("Maximum Height##13", &settings->getParameters()->getWoodlandHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##13", &settings->getParameters()->getWoodlandHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##2", &settings->getParameters()->getWoodlandHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##13", &settings->getParameters()->getWoodlandHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Woodland Textures")) {
            // Draw the texture selection section for woodland textures
            drawTextureSelectionSection(
                "Woodland",
                settings->getParameters()->getWoodlandTextureLow(),
                settings->getParameters()->getWoodlandTextureMidFlat(),
                settings->getParameters()->getWoodlandTextureMidSteep(),
                settings->getParameters()->getWoodlandTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setWoodlandTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setWoodlandTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setWoodlandTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setWoodlandTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Tropical Rainforest parameters
    if (ImGui::CollapsingHeader("Tropical Rainforest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Tropical Rainforest", &settings->getParameters()->getTropicalRainforestSelected());
    
        if (ImGui::CollapsingHeader("Plains (Jungle)##TropicalRainforest")) {
            ImGui::SliderInt("Maximum Height##14", &settings->getParameters()->getTropicalRainforestPlainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##14", &settings->getParameters()->getTropicalRainforestPlainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Evenness##4", &settings->getParameters()->getTropicalRainforestPlainsEvenness(), 0, 100);
            ImGui::SliderInt("Tree Density##14", &settings->getParameters()->getTropicalRainforestPlainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Mountains (Jungle Mountains)##TropicalRainforest")) {
            ImGui::SliderInt("Maximum Height##15", &settings->getParameters()->getTropicalRainforestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##15", &settings->getParameters()->getTropicalRainforestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##3", &settings->getParameters()->getTropicalRainforestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##15", &settings->getParameters()->getTropicalRainforestMountainsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Hills (Jungle)##TropicalRainforest")) {
            ImGui::SliderInt("Maximum Height##16", &settings->getParameters()->getTropicalRainforestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##16", &settings->getParameters()->getTropicalRainforestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##3", &settings->getParameters()->getTropicalRainforestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##16", &settings->getParameters()->getTropicalRainforestHillsTreeDensity(), 0, 100);
        }
    
        if (ImGui::CollapsingHeader("Volcanoes")) {
            ImGui::SliderInt("Maximum Height##17",&settings->getParameters()->getTropicalRainforestVolcanoesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##17", &settings->getParameters()->getTropicalRainforestVolcanoesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size", &settings->getParameters()->getTropicalRainforestVolcanoesSize(), 0, 100);
            ImGui::SliderInt("Tree Density##17", &settings->getParameters()->getTropicalRainforestVolcanoesTreeDensity(), 0, 100);
            ImGui::SliderInt("Thickness##2", &settings->getParameters()->getTropicalRainforestVolcanoesThickness(), 0, 100);
            ImGui::SliderInt("Density", &settings->getParameters()->getTropicalRainforestVolcanoesDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Jungle Textures")) {
            // Draw the texture selection section for jungle textures
            drawTextureSelectionSection(
                "Jungle",
                settings->getParameters()->getJungleTextureLow(),
                settings->getParameters()->getJungleTextureMidFlat(),
                settings->getParameters()->getJungleTextureMidSteep(),
                settings->getParameters()->getJungleTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setJungleTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Jungle Mountains Textures")) {
            // Draw the texture selection section for jungle mountains textures
            drawTextureSelectionSection(
                "JungleMountains",
                settings->getParameters()->getJungleMountainsTextureLow(),
                settings->getParameters()->getJungleMountainsTextureMidFlat(),
                settings->getParameters()->getJungleMountainsTextureMidSteep(),
                settings->getParameters()->getJungleMountainsTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setJungleMountainsTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleMountainsTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleMountainsTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setJungleMountainsTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Volcanic Textures")) {
            // Draw the texture selection section for volcanic textures
            drawTextureSelectionSection(
                "Volcanic",
                settings->getParameters()->getVolcanicTextureLow(),
                settings->getParameters()->getVolcanicTextureMidFlat(),
                settings->getParameters()->getVolcanicTextureMidSteep(),
                settings->getParameters()->getVolcanicTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setVolcanicTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setVolcanicTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setVolcanicTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setVolcanicTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Temperate Rainforest parameters
    if (ImGui::CollapsingHeader("Temperate Rainforest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Temperate Rainforest", &settings->getParameters()->getTemperateRainforestSelected());
    
        if (ImGui::CollapsingHeader("Hills (Temperate Rainforest)##TemperateRainforest")) {
            ImGui::SliderInt("Maximum Height##18", &settings->getParameters()->getTemperateRainforestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##18", &settings->getParameters()->getTemperateRainforestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##4", &settings->getParameters()->getTemperateRainforestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##18", &settings->getParameters()->getTemperateRainforestHillsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains (Temperate Rainforest)##TemperateRainforest")) {
            ImGui::SliderInt("Maximum Height##19", &settings->getParameters()->getTemperateRainforestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##19", &settings->getParameters()->getTemperateRainforestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##4", &settings->getParameters()->getTemperateRainforestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##19", &settings->getParameters()->getTemperateRainforestMountainsTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Swamp")) {
            ImGui::SliderInt("Maximum Height##20", &settings->getParameters()->getTemperateRainforestSwampMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##20", &settings->getParameters()->getTemperateRainforestSwampOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Wetness", &settings->getParameters()->getTemperateRainforestSwampWetness(), 0, 100);
            ImGui::SliderInt("Tree Density##20", &settings->getParameters()->getTemperateRainforestSwampTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Temperate Rainforest Textures")) {
            // Draw the texture selection section for temperate rainforest textures
            drawTextureSelectionSection(
                "TemperateRainforest",
                settings->getParameters()->getTemperateTextureLow(),
                settings->getParameters()->getTemperateTextureMidFlat(),
                settings->getParameters()->getTemperateTextureMidSteep(),
                settings->getParameters()->getTemperateTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setTemperateTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setTemperateTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setTemperateTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setTemperateTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Swamp Textures")) {
            // Draw the texture selection section for swamp textures
            drawTextureSelectionSection(
                "Swamp",
                settings->getParameters()->getSwampTextureLow(),
                settings->getParameters()->getSwampTextureMidFlat(),
                settings->getParameters()->getSwampTextureMidSteep(),
                settings->getParameters()->getSwampTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setSwampTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setSwampTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setSwampTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setSwampTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Temperate Seasonal Forest parameters
    if (ImGui::CollapsingHeader("Temperate Seasonal Forest Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Temperate Seasonal Forest", &settings->getParameters()->getTemperateSeasonalForestSelected());
        if (ImGui::CollapsingHeader("Hills##SeasonalForest")) {
            ImGui::SliderInt("Maximum Height##21", &settings->getParameters()->getTemperateSeasonalForestHillsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##21", &settings->getParameters()->getTemperateSeasonalForestHillsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Bumpiness##5", &settings->getParameters()->getTemperateSeasonalForestHillsBumpiness(), 0, 100);
            ImGui::SliderInt("Tree Density##21", &settings->getParameters()->getTemperateSeasonalForestHillsTreeDensity(), 0, 100);
            ImGui::SliderInt("Autumnal Occurrence##1", &settings->getParameters()->getTemperateSeasonalForestHillsAutumnalOccurrence(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mountains##SeasonalForest")) {
            ImGui::SliderInt("Maximum Height##22", &settings->getParameters()->getTemperateSeasonalForestMountainsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##22", &settings->getParameters()->getTemperateSeasonalForestMountainsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Ruggedness##5", &settings->getParameters()->getTemperateSeasonalForestMountainsRuggedness(), 0, 100);
            ImGui::SliderInt("Tree Density##22", &settings->getParameters()->getTemperateSeasonalForestMountainsTreeDensity(), 0, 100);
            ImGui::SliderInt("Autumnal Occurrence##2", &settings->getParameters()->getTemperateSeasonalForestMountainsAutumnalOccurrence(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Seasonal Forest Textures")) {
            // Draw the texture selection section for seasonal forest textures
            drawTextureSelectionSection(
                "SeasonalForest",
                settings->getParameters()->getSeasonalForestTextureLow(),
                settings->getParameters()->getSeasonalForestTextureMidFlat(),
                settings->getParameters()->getSeasonalForestTextureMidSteep(),
                settings->getParameters()->getSeasonalForestTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setSeasonalForestTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setSeasonalForestTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setSeasonalForestTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setSeasonalForestTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Autumn Textures")) {
            // Draw the texture selection section for autumn textures
            drawTextureSelectionSection(
                "Autumn",
                settings->getParameters()->getAutumnTextureLow(),
                settings->getParameters()->getAutumnTextureMidFlat(),
                settings->getParameters()->getAutumnTextureMidSteep(),
                settings->getParameters()->getAutumnTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setAutumnTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setAutumnTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setAutumnTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setAutumnTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Subtropical Desert parameters
    if (ImGui::CollapsingHeader("Subtropical Desert Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Subtropical Desert", &settings->getParameters()->getSubtropicalDesertSelected());
        if (ImGui::CollapsingHeader("Dunes##SubtropicalDesert")) {
            ImGui::SliderInt("Maximum Height##23", &settings->getParameters()->getSubtropicalDesertDunesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##23", &settings->getParameters()->getSubtropicalDesertDunesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##1", &settings->getParameters()->getSubtropicalDesertDunesSize(), 0, 100);
            ImGui::SliderInt("Tree Density##23", &settings->getParameters()->getSubtropicalDesertDunesTreeDensity(), 0, 100);
            ImGui::SliderInt("Dune Frequency", &settings->getParameters()->getSubtropicalDesertDunesDuneFrequency(), 0, 100);
            ImGui::SliderInt("Dune Waviness", &settings->getParameters()->getSubtropicalDesertDunesDuneWaviness(), 0, 100);
            ImGui::SliderInt("Bumpiness##6", &settings->getParameters()->getSubtropicalDesertDunesBumpiness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Mesa")) {
            ImGui::SliderInt("Maximum Height##24", &settings->getParameters()->getSubtropicalDesertMesasMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##24", &settings->getParameters()->getSubtropicalDesertMesasOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##2", &settings->getParameters()->getSubtropicalDesertMesasSize(), 0, 100);
            ImGui::SliderInt("Tree Density##24", &settings->getParameters()->getSubtropicalDesertMesasTreeDensity(), 0, 100);
            ImGui::SliderInt("Number of Terraces", &settings->getParameters()->getSubtropicalDesertMesasNumberOfTerraces(), 0, 100);
            ImGui::SliderInt("Steepness##2", &settings->getParameters()->getSubtropicalDesertMesasSteepness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Ravines")) {
            ImGui::SliderInt("Maximum Height##25", &settings->getParameters()->getSubtropicalDesertRavinesMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##25", &settings->getParameters()->getSubtropicalDesertRavinesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Density", &settings->getParameters()->getSubtropicalDesertRavinesDensity(), 0, 100);
            ImGui::SliderInt("Tree Density##25", &settings->getParameters()->getSubtropicalDesertRavinesTreeDensity(), 0, 100);
            ImGui::SliderInt("Ravine Width", &settings->getParameters()->getSubtropicalDesertRavinesRavineWidth(), 0, 100);
            ImGui::SliderInt("Smoothness##1", &settings->getParameters()->getSubtropicalDesertRavinesSmoothness(), 0, 100);
            ImGui::SliderInt("Steepness##3", &settings->getParameters()->getSubtropicalDesertRavinesSteepness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Oasis")) {
            ImGui::SliderInt("Maximum Height##26", &settings->getParameters()->getSubtropicalDesertOasisMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##26", &settings->getParameters()->getSubtropicalDesertOasisOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##3", &settings->getParameters()->getSubtropicalDesertOasisSize(), 0, 100);
            ImGui::SliderInt("Flatness#1", &settings->getParameters()->getSubtropicalDesertOasisFlatness(), 0, 100);
            ImGui::SliderInt("Tree Density##26", &settings->getParameters()->getSubtropicalDesertOasisTreeDensity(), 0, 100);
            ImGui::SliderInt("Dune Frequency", &settings->getParameters()->getSubtropicalDesertOasisDuneFrequency(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Cracked")) {
            ImGui::SliderInt("Maximum Height##27", &settings->getParameters()->getSubtropicalDesertCrackedMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##27", &settings->getParameters()->getSubtropicalDesertCrackedOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##4", &settings->getParameters()->getSubtropicalDesertCrackedSize(), 0, 100);
            ImGui::SliderInt("Flatness#2", &settings->getParameters()->getSubtropicalDesertCrackedFlatness(), 0, 100);
            ImGui::SliderInt("Tree Density##27", &settings->getParameters()->getSubtropicalDesertCrackedTreeDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Dunes Textures")) {
            // Draw the texture selection section for dunes textures
            drawTextureSelectionSection(
                "Dunes",
                settings->getParameters()->getHotDesertTextureLow(),
                settings->getParameters()->getHotDesertTextureMidFlat(),
                settings->getParameters()->getHotDesertTextureMidSteep(),
                settings->getParameters()->getHotDesertTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setHotDesertTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setHotDesertTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setHotDesertTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setHotDesertTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Mesa Textures")) {
            // Draw the texture selection section for mesa textures
            drawTextureSelectionSection(
                "Mesa",
                settings->getParameters()->getMesaTextureLow(),
                settings->getParameters()->getMesaTextureMidFlat(),
                settings->getParameters()->getMesaTextureMidSteep(),
                settings->getParameters()->getMesaTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setMesaTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setMesaTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setMesaTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setMesaTextureHigh(texture); }
            );
        }

        if (ImGui::CollapsingHeader("Ravines Textures")) {
            // Draw the texture selection section for ravines textures
            drawTextureSelectionSection("Ravines",
                settings->getParameters()->getBadlandsTextureLow(),
                settings->getParameters()->getBadlandsTextureMidFlat(),
                settings->getParameters()->getBadlandsTextureMidSteep(),
                settings->getParameters()->getBadlandsTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setBadlandsTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setBadlandsTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setBadlandsTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setBadlandsTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Oasis Textures")) {
            // Draw the texture selection section for oasis textures
            drawTextureSelectionSection(
                "Oasis",
                settings->getParameters()->getOasisTextureLow(),
                settings->getParameters()->getOasisTextureMidFlat(),
                settings->getParameters()->getOasisTextureMidSteep(),
                settings->getParameters()->getOasisTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setOasisTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setOasisTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setOasisTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setOasisTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Cracked Textures")) {
            // Draw the texture selection section for cracked textures
            drawTextureSelectionSection("Cracked",
                settings->getParameters()->getDustyTextureLow(),
                settings->getParameters()->getDustyTextureMidFlat(),
                settings->getParameters()->getDustyTextureMidSteep(),
                settings->getParameters()->getDustyTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setDustyTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setDustyTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setDustyTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setDustyTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }

    // Ocean parameters
    if (ImGui::CollapsingHeader("Ocean Parameters")) {
        ImGui::Indent(15.0f);
        ImGui::Checkbox("Enable Ocean", &settings->getParameters()->getOceanSelected());
        if (ImGui::CollapsingHeader("Flat Seabed")) {
            ImGui::SliderInt("Maximum Height##28", &settings->getParameters()->getOceanFlatSeabedMaxHeight(), 0, 100);
            ImGui::SliderInt("Evenness##5", &settings->getParameters()->getOceanFlatSeabedEvenness(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##28", &settings->getParameters()->getOceanFlatSeabedOccurrenceProbability(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Volcanic Islands")) {
            ImGui::SliderInt("Maximum Height##29", &settings->getParameters()->getOceanVolcanicIslandsMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##29", &settings->getParameters()->getOceanVolcanicIslandsOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##6", &settings->getParameters()->getOceanVolcanicIslandsSize(), 0, 100);
            ImGui::SliderInt("Thickness##1", &settings->getParameters()->getOceanVolcanicIslandsThickness(), 0, 100);
            ImGui::SliderInt("Density##2", &settings->getParameters()->getOceanVolcanicIslandsDensity(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Trenches")) {
            ImGui::SliderInt("Maximum Height##30", &settings->getParameters()->getOceanTrenchesMaxHeight(), 0, 100);
            ImGui::SliderInt("Density##3", &settings->getParameters()->getOceanTrenchesDensity(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##31", &settings->getParameters()->getOceanTrenchesOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Trench Width", &settings->getParameters()->getOceanTrenchesTrenchWidth(), 0, 100);
            ImGui::SliderInt("Smoothness##2", &settings->getParameters()->getOceanTrenchesSmoothness(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Water Stacks")) {
            ImGui::SliderInt("Maximum Height##30", &settings->getParameters()->getOceanWaterStacksMaxHeight(), 0, 100);
            ImGui::SliderInt("Occurrence Probability##30", &settings->getParameters()->getOceanWaterStacksOccurrenceProbability(), 0, 100);
            ImGui::SliderInt("Size##7", &settings->getParameters()->getOceanWaterStacksSize(), 0, 100);
        }
        if (ImGui::CollapsingHeader("Ocean Textures")) {
            // Draw the texture selection section for ocean textures
            drawTextureSelectionSection(
                "Ocean",
                settings->getParameters()->getOceanTextureLow(),
                settings->getParameters()->getOceanTextureMidFlat(),
                settings->getParameters()->getOceanTextureMidSteep(),
                settings->getParameters()->getOceanTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setOceanTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setOceanTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setOceanTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setOceanTextureHigh(texture); }
            );
        }
        if (ImGui::CollapsingHeader("Water Stacks Textures")) {
            // Draw the texture selection section for water stacks textures
            drawTextureSelectionSection(
                "Cliffs",
                settings->getParameters()->getCliffsTextureLow(),
                settings->getParameters()->getCliffsTextureMidFlat(),
                settings->getParameters()->getCliffsTextureMidSteep(),
                settings->getParameters()->getCliffsTextureHigh(),
                [&](std::string texture) { settings->getParameters()->setCliffsTextureLow(texture); },
                [&](std::string texture) { settings->getParameters()->setCliffsTextureMidFlat(texture); },
                [&](std::string texture) { settings->getParameters()->setCliffsTextureMidSteep(texture); },
                [&](std::string texture) { settings->getParameters()->setCliffsTextureHigh(texture); }
            );
        }
        ImGui::Unindent(15.0f);
    }
    
    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopStyleColor(3);

    ImGui::End();

    // Render the frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



/** 
    * Function to render the UI for the homepage of the application
    * @param settings - shared pointer to the Settings object
*/
void UI::renderHomepage(shared_ptr<Settings> settings) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the ImGui window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("Welcome to TerraInfinity", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    ImGui::Image(logoTexture.getId(), ImVec2(600, 150)); // Display the logo in the top left corner

    // Add space between the logo and the text
    ImGui::Dummy(ImVec2(0, 20));

    // Introductory text to explain how to use the application
    ImGui::Text("Click 'New World' to generate a new default world, or select a saved one to open it...");

    // Add space between the text and the New World button
    ImGui::Dummy(ImVec2(0, 20));

    // Centre the New World button
    ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 300) / 2);
    if (ImGui::Button("New World", ImVec2(300, 0))) {
        // Open the new world name popup
        ImGui::OpenPopup("New World Name");
    }
    // Tooltip for the New World button
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::Text("Generate a new world with default settings");
        ImGui::EndTooltip();
    }

    // Add space between the New World button and the saved worlds list
    ImGui::Dummy(ImVec2(0, 20));

    // Display the scrollable list of saved worlds
    ImGui::Text("Your Saved Worlds:");
    ImGui::SetCursorPosX(0);
    ImGui::BeginChild("SavedWorlds", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight() - 500), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);

    // Retrieve the list of saved worlds from the saves directory
    string projectRoot = getenv("PROJECT_ROOT"); 
    string savedRoot = projectRoot + settings->getFilePathDelimitter() + "saves" + settings->getFilePathDelimitter();
    vector<string> savedFiles;

    // If saves directory does not exist, create it
    if (!fs::exists(savedRoot)) {
        fs::create_directories(savedRoot);
    }

    // Iterate through the saves directory recursively  and add the names of saved worlds to the vector
    for (const auto& entry : fs::recursive_directory_iterator(savedRoot)) {
        // Check if the entry is a file and has the .json extension
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            savedFiles.push_back(entry.path().stem().string());
        }
    }

    // Initialize variables for the delete and rename popups
    static string toDelete = "";
    static string toRename = "";

    // Display the saved worlds as buttons
    for (string savedFile : savedFiles) {
        if (ImGui::Button(savedFile.c_str(), ImVec2(1750, 0))) {
            // If a world is selected, load the parameters from the corresponding JSON file
            settings->getParameters()->loadFromFile(savedFile, settings->getFilePathDelimitter());
            settings->setCurrentWorld(savedFile); // Set the current world to the selected one
            settings->setCurrentPage(UIPage::Loading); // Set the current page to Loading
        }
        // Tooltip for the saved world button saying Open <world name>
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Open %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        ImGui::SameLine();

        // Change the button color to purple for rename button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.35f, 0.65f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.35f, 0.45f, 0.75f, 1.0f));

        // Add a rename button next to each saved world with the pencil icon
        if (ImGui::Button((ICON_FA_PENCIL + std::string("##Rename ") + savedFile).c_str(), ImVec2(50, 0))) {
            // Set the toRename variable to the selected file name
            toRename = savedFile;
        };
        // Tooltip for the rename button saying Rename <world name>
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Rename %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor(2);

        ImGui::SameLine();

        // Change the button color to red for delete button
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6, 0.2, 0.2, 1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.7f, 0.3f, 0.3f, 1.0f));

        // Add a delete button next to each saved world with the trash icon
        if (ImGui::Button((ICON_FA_TRASH + std::string("##Delete ") + savedFile).c_str(), ImVec2(50, 0))) {
            // Set the toDelete variable to the selected file name
            toDelete = savedFile;
        };
        // Tooltip for the delete button saying Delete <world name>
        if (ImGui::IsItemHovered()) {
            ImGui::BeginTooltip();
            ImGui::Text("Delete %s", savedFile.c_str());
            ImGui::EndTooltip();
        }
        // Reset the button color
        ImGui::PopStyleColor(2);
    }
    ImGui::EndChild();

    // If toDelete is not empty, open the delete confirmation popup
    if (toDelete != "") {
        ImGui::OpenPopup("Delete Confirmation");
    }

    // If toRename is not empty, open the rename world popup
    if (toRename != "") {
        ImGui::OpenPopup("Rename World");
    }

    // Delete confirmation popup
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Delete Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Spacing();
        ImGui::Text("Are you sure you want to delete '%s'?", toDelete.c_str());
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("Confirm", ImVec2(120, 0))) {
            // Delete the folder corresponding to the world name
            if (fs::remove_all(savedRoot + toDelete)) {
                toDelete = ""; // Reset the toDelete variable
            } else {
                cerr << "Failed to delete file" << endl;
            }
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        // If the Cancel button is clicked, close the popup without deleting
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            toDelete = ""; // Reset the toDelete variable
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::Spacing();
        ImGui::EndPopup();
    }

    // Initialise new world name to empty string
    static char newWorldName[128] = "";

    // Initialise exists and empty flags to false
    static bool exists = false;
    static bool empty = false;

    // Rename world popup
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Rename World", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Enter a new name for your world '%s':", toRename.c_str());
        ImGui::Spacing();
        // Input field for the new world name
        ImGui::InputText("##New Name", newWorldName, IM_ARRAYSIZE(newWorldName));
        ImGui::Spacing();

        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            empty = false; // Reset the empty flag
            exists = false; // Reset the exists flag

            // Check if the name entered is empty 
            if (newWorldName[0] == '\0') {
                empty = true; // Set the empty flag to true
            } 

            // Check if the name already exists
            for (const auto& savedFile : savedFiles) {
                if (savedFile == newWorldName) {
                    exists = true; // Set the exists flag to true
                    break;
                }
            }

            // If the name is not empty and a world does not already exist with that name, rename the world
            if (!exists && !empty) { 
                fs::rename(savedRoot + toRename, savedRoot + newWorldName); // Rename the directory
                // Rename the JSON file
                fs::rename(savedRoot + newWorldName + settings->getFilePathDelimitter() + toRename + ".json", savedRoot + newWorldName + settings->getFilePathDelimitter() + newWorldName + ".json");
                toRename = ""; // Reset the toRename variable
                newWorldName[0] = '\0'; // Reset the new world name
                ImGui::CloseCurrentPopup();
            } 
        }
        ImGui::SameLine();
        // If the Cancel button is clicked, close the popup without renaming and reset the variables
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            toRename = "";
            exists = false;
            empty = false;
            newWorldName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();

        // Display error messages if the name already exists or is empty
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
        ImGui::Spacing();

        // Input field for the new world name
        ImGui::InputText("##Name", newWorldName, IM_ARRAYSIZE(newWorldName));
        
        ImGui::Spacing();
        ImGui::SetCursorPosX((ImGui::GetWindowSize().x - 240) / 2);
        if (ImGui::Button("OK", ImVec2(120, 0))) {
            empty = false; // Reset the empty flag 
            exists = false; // Reset the exists flag 

            // Check if the name is empty
            if (newWorldName[0] == '\0') {
                empty = true; // Set the empty flag to true
            } 

            // Check if the name already exists
            for (const auto& savedFile : savedFiles) {
                if (savedFile == newWorldName) {
                    exists = true; // Set the exists flag to true
                    break; // Exit the loop if a match is found
                }
            }

            // If the name is not empty and does not exist, create the new world and load it
            if (!exists && !empty) { 
                settings->setCurrentWorld(newWorldName); // Set the current world to the new name
                settings->getParameters()->setDefaultValues(newWorldName); // Set parameters to default values
                settings->getParameters()->saveToFile(newWorldName, settings->getFilePathDelimitter()); // Save the parameters to a JSON file
                newWorldName[0] = '\0'; // Reset the new world name variable
                settings->setCurrentPage(UIPage::Loading); // Set the current page to Loading
                ImGui::CloseCurrentPopup(); // Close the popup
            } 
        }
        ImGui::SameLine();
        // If the Cancel button is clicked, close the popup without creating a new world and reset the variables
        if (ImGui::Button("Cancel", ImVec2(120, 0))) {
            exists = false;
            empty = false;
            newWorldName[0] = '\0';
            ImGui::CloseCurrentPopup();
        }
        ImGui::Spacing();
        // Display error messages if the name already exists or is empty
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

    ImGui::End(); // End the ImGui window

    // Render the current ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/**
    * Function to render the loading screen while the world is being generated
    * @param settings - shared pointer to the Settings object
*/
void UI::renderLoadingScreen(shared_ptr<Settings> settings) {
    // Start the ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Create the ImGui window
    ImGui::SetNextWindowPos(ImVec2(0, 0));  // Position at the top-left
    ImGui::SetNextWindowSize(ImVec2(settings->getWindowWidth(), settings->getWindowHeight()));  // Full height of the window

    ImGui::Begin("Loading World", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

   
    // Animated Loading Dots
    static float elapsedTime = 0.0f;
    elapsedTime += ImGui::GetIO().DeltaTime;
    int dotCount = static_cast<int>(elapsedTime * 2.0f) % 4;  // Cycle through 0-3 dots based on elapsed time

    // Set the loading text with the correct number of dots
    std::string loadingText = "Generating World '" + settings->getCurrentWorld() + "'";
    for (int i = 0; i < dotCount; ++i) loadingText += ".";

    // Centre the loading text
    ImGui::SetCursorPosX((settings->getWindowWidth() - ImGui::CalcTextSize(loadingText.c_str()).x) / 2);
    ImGui::SetCursorPosY((settings->getWindowHeight() - ImGui::CalcTextSize(loadingText.c_str()).y) / 2);
    ImGui::Text("%s", loadingText.c_str());

    ImGui::End(); // End the ImGui window

    // Render the current ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}