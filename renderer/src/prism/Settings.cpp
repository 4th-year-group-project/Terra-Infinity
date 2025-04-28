/**
 * @file Settings.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Settings class.
 * @details This class will hold all of the settings for the project which will include the specific
 * settings for the project along with settings for the renderer such as the window size and the
 * graphics settings and the render distance.
 * @version 1.0
 * @date 2025
 * 
 */
#include <string>
#include <ostream>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Parameters.hpp"
#include "Settings.hpp"

using namespace std;

/**
 * @brief This function will update the settings for the project
 * 
 * @details This function will update the settings for the project. It will take in all of the
 * settings and update the settings for the project. There is no method to only update a few
 * settings values at a time. This is to ensure that all of the settings are updated at once.
 * 
 * @param inWindowWidth [in] int The width of the window
 * @param inWindowHeight [in] int The height of the window
 * @param inUIWidth [in] int The width of the UI
 * @param inFullscreen [in] bool Whether the window is fullscreen or not
 * @param inRenderDistance [in] int The render distance of the renderer
 * @param inChunkSize [in] int The size of the chunks
 * @param inSubChunkSize [in] int The size of the subchunks
 * @param inSubChunkResolution [in] float The resolution of the subchunks
 * @param inFilePathDelimitter [in] char The delimitter for the file paths
 * @param inMaxHeight [in] float The maximum height of the terrain
 * @param inSeaLevel [in] float The sea level of the terrain
 * @param inRequestDistance [in] float The request distance of the terrain
 * @param inCurrentPage [in] UIPage The current page of the UI
 * @param inCurrentWorld [in] string The current world being rendered
 * @param inParameters [in] shared_ptr<Parameters> The parameters for the terrain generation
 * @param inFogStart [in] float The start distance of the fog
 * @param inFogEnd [in] float The end distance of the fog
 * @param inFogDensity [in] float The density of the fog
 * @param inFogColor [in] glm::vec3 The color of the fog
 * @param inUse1kTextures [in] bool Whether to use 1k textures or not
 * 
 * @return void
 * 
 */
void Settings::updateSettings(
    int inWindowWidth,
    int inWindowHeight,
    int inUIWidth,
    bool inFullscreen,
    int inRenderDistance,
    int inChunkSize,
    int inSubChunkSize,
    float inSubChunkResolution,
    char inFilePathDelimitter,
    float inMaxHeight,
    float inSeaLevel,
    float inRequestDistance,
    UIPage inCurrentPage,
    string inCurrentWorld,
    shared_ptr<Parameters> inParameters,
    // Fog settings
    float inFogStart,
    float inFogEnd,
    float inFogDensity,
    glm::vec3 inFogColor,
    bool inUse1kTextures
){
    windowWidth = inWindowWidth;
    windowHeight = inWindowHeight;
    UIWidth = inUIWidth;
    fullscreen = inFullscreen;
    renderDistance = inRenderDistance;
    chunkSize = inChunkSize;
    subChunkSize = inSubChunkSize;
    subChunkResolution = inSubChunkResolution;
    filePathDelimitter = inFilePathDelimitter;
    maximumHeight = inMaxHeight;
    seaLevel = inSeaLevel;
    requestDistance = inRequestDistance;
    currentPage = inCurrentPage;
    currentWorld = inCurrentWorld;
    parameters = inParameters;
    fogStart = inFogStart;
    fogEnd = inFogEnd;
    fogDensity = inFogDensity;
    fogColor = inFogColor;
    use1kTextures = inUse1kTextures; // Default to 1k textures
}

/**
 * @brief Overloading the << operator to print the settings to the console or a file
 * 
 * @details This function will overload the << operator to print the settings to the console or a
 * file. This is used for debugging purposes.
 * 
 * @param os [in] ostream& The output stream to print to
 * 
 * @return ostream& The output stream
 * 
 */
ostream& Settings::operator<< (ostream &os){
    os << "Window Width: " << windowWidth << endl;
    os << "Window Height: " << windowHeight << endl;
    os << "UI Width: " << UIWidth << endl;
    os << "Fullscreen: " << fullscreen << endl;
    os << "Render Distance: " << renderDistance << endl;
    os << "Chunk Size: " << chunkSize << endl;
    os << "SubChunk Size: " << subChunkSize << endl;
    os << "SubChunk Resolution: " << subChunkResolution << endl;
    os << "File Path Delimitter: " << filePathDelimitter << endl;
    os << "Max Height: " << maximumHeight << endl;
    os << "Sea Level: " << seaLevel << endl;
    os << "Request Distance: " << requestDistance << endl;
    os << "Current Page: " << currentPage << endl;
    os << "Current World: " << currentWorld << endl;
    os << "Fog Start: " << fogStart << endl;
    os << "Fog End: " << fogEnd << endl;
    os << "Fog Density: " << fogDensity << endl;
    os << "Fog Color: " << fogColor.x << ", " << fogColor.y << ", " << fogColor.z << endl;
    return os;
}
