/*
    This class will hold all of the settings for the project which will include the specific
    settings for the project along with settings for the renderer such as the window size and the
    graphics settings and the render distance.
*/
#include <string>
#include <ostream>

#include <Settings.hpp>

using namespace std;


void Settings::updateSettings(
    int inWindowWidth,
    int inWindowHeight,
    bool inFullscreen,
    int inRenderDistance,
    int inChunkSize,
    int inSubChunkSize,
    int inSubChunkResolution,
    char inFilePathDelimitter,
    float inMaxHeight,
    float inSeaLevel
){
    windowWidth = inWindowWidth;
    windowHeight = inWindowHeight;
    fullscreen = inFullscreen;
    renderDistance = inRenderDistance;
    chunkSize = inChunkSize;
    subChunkSize = inSubChunkSize;
    subChunkResolution = inSubChunkResolution;
    filePathDelimitter = inFilePathDelimitter;
    maximumHeight = inMaxHeight;
    seaLevel = inSeaLevel;
}

/*
    Overloading the << operator to print the settings to the console or a file
*/
ostream& Settings::operator<< (ostream &os){
    os << "Window Width: " << windowWidth << endl;
    os << "Window Height: " << windowHeight << endl;
    os << "Fullscreen: " << fullscreen << endl;
    os << "Render Distance: " << renderDistance << endl;
    os << "Chunk Size: " << chunkSize << endl;
    os << "SubChunk Size: " << subChunkSize << endl;
    os << "SubChunk Resolution: " << subChunkResolution << endl;
    os << "File Path Delimitter: " << filePathDelimitter << endl;
    os << "Max Height: " << maximumHeight << endl;
    os << "Sea Level: " << seaLevel << endl;
    return os;
}
