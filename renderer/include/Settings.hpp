#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <memory>
#include "Parameters.hpp"

using namespace std;

/*
    This class will hold all of the settings for the project which will include the specific
    settings for the project along with settings for the renderer such as the window size and the
    graphics settings and the render distance.
*/

// define an enum for different pages of the UI either Home WorldMenuOpen or WorldMenuClosed
enum UIPage {Home, WorldMenuOpen, Loading, WorldMenuClosed};

class Settings
{
private:
    int windowWidth; // The width of the window
    int windowHeight; // The height of the window
    int UIWidth; // The width of the UI
    bool fullscreen; // Whether the window is fullscreen or not
    int renderDistance; // The render distance of the renderer
    int chunkSize; // The size of the chunks in the world
    int subChunkSize; // The size of the subchunks in the world
    float subChunkResolution; // The resolution of the subchunks in the world
    char filePathDelimitter; // The delimitter for the file paths
    float maximumHeight;
    float seaLevel;
    float requestDistance;
    UIPage currentPage; // The current page of the UI
    string currentWorld; // The current world that is being rendered
    shared_ptr<Parameters> parameters;
public:
    Settings(
        int inWindowWidth,
        int inWindowHeight,
        int inUIWidth,
        bool inFullscreen,
        int inRenderDistance,
        int inChunkSize,
        int inSubChunkSize,
        float inSubChunkResolution,
        char inFilePathDelimitter,
        float inMaximumHeight,
        float inSeaLevel,
        float inRequestDistance,
        UIPage inCurrentPage,
        string inCurrentWorld,
        shared_ptr<Parameters> inParameters
    ):
        windowWidth(inWindowWidth),
        windowHeight(inWindowHeight),
        UIWidth(inUIWidth),
        fullscreen(inFullscreen),
        renderDistance(inRenderDistance),
        chunkSize(inChunkSize),
        subChunkSize(inSubChunkSize),
        subChunkResolution(inSubChunkResolution),
        filePathDelimitter(inFilePathDelimitter),
        maximumHeight(inMaximumHeight),
        seaLevel(inSeaLevel),
        requestDistance(inRequestDistance),
        currentPage(inCurrentPage),
        currentWorld(inCurrentWorld),
        parameters(inParameters)
        {};
    Settings(): Settings(1920, 1080, 700, true, 16, 1024, 32, 1, '/', 192.0f, 0.2f, 1024.0f, UIPage::Home, "", make_shared<Parameters>(Parameters())) {};
    ~Settings() {parameters.reset();}

    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }
    int getUIWidth() { return UIWidth; }
    bool getFullscreen() { return fullscreen; }
    int getRenderDistance() { return renderDistance; }
    int getChunkSize() { return chunkSize; }
    int getSubChunkSize() { return subChunkSize; }
    float getSubChunkResolution() { return subChunkResolution; }
    char getFilePathDelimitter() { return filePathDelimitter; }
    float getMaximumHeight() { return maximumHeight; }
    float getSeaLevel() { return seaLevel; }
    float getRequestDistance() { return requestDistance; }
    UIPage getCurrentPage() { return currentPage; }
    string getCurrentWorld() { return currentWorld; }
    shared_ptr<Parameters> getParameters() { return parameters; }

    void setUIWidth(int inUIWidth) { UIWidth = inUIWidth; }
    void setCurrentPage(UIPage inCurrentPage) { currentPage = inCurrentPage; }
    void setParameters(shared_ptr<Parameters> inParameters) { parameters = inParameters; }
    void setCurrentWorld(string inCurrentWorld) { currentWorld = inCurrentWorld; }

    void updateSettings(
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
        shared_ptr<Parameters> inParameters
    );

    ostream& operator<< (ostream &os);
    string getProjectRoot() {return string(getenv("PROJECT_ROOT"));}
    string getRenderPath() {return string(getenv("PRISM_ROOT"));}
    string getShaderPath() {return string(getenv("SHADER_ROOT"));}
    string getTexturePath() {return string(getenv("TEXTURE_ROOT"));}
    string getDataPath() {return string(getenv("DATA_ROOT"));}
};

#endif
