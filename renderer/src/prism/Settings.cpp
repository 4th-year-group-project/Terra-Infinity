/*
    This class will hold all of the settings for the project which will include the specific
    settings for the project along with settings for the renderer such as the window size and the
    graphics settings and the render distance.
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
    glm::vec3 inFogColor
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
}

/*
    Overloading the << operator to print the settings to the console or a file
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
