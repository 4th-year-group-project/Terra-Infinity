
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include <memory>
#include "Parameters.hpp"

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

using namespace std;

/*
    This class will hold all of the settings for the project which will include the specific
    settings for the project along with settings for the renderer such as the window size and the
    graphics settings and the render distance.
*/

/**
 * @brief This enum is used to define the different pages of the UI that can be rendered.
 *
 */
enum UIPage {Home, WorldMenuOpen, Loading, WorldMenuClosed};

/**
 * @brief This class is used to hold all of the settings for the project. It contains the window size, the graphics
 * settings, and the render distance.
 *
 */
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
    /*Fog settings*/
    float fogStart; // The start distance of the fog
    float fogEnd; // The end distance of the fog
    float fogDensity; // The density of the fog
    glm::vec3 fogColor; // The color of the fog

    bool use1kTextures; // Whether to use 1k textures or not

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
        shared_ptr<Parameters> inParameters,
        // Fog settings
        float inFogStart,
        float inFogEnd,
        float inFogDensity,
        glm::vec3 inFogColor,
        // Texture settings
        bool inUse1kTextures
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
        parameters(inParameters),
        fogStart(inFogStart),
        fogEnd(inFogEnd),
        fogDensity(inFogDensity),
        fogColor(inFogColor),
        use1kTextures(inUse1kTextures)
        {};
    Settings(): Settings(
        1920,
        1080,
        700,
        true,
        16,
        1024,
        32,
        1,
        '/',
        192.0f,
        0.2f,
        1024.0f,
        UIPage::Home,
        "",
        nullptr,
        0.0f,
        512.0f,
        1.0f,
        glm::vec3(0.5f, 0.5f, 0.5f),
        true
    ) {};
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

    float getFogStart() { return fogStart; }
    float getFogEnd() { return fogEnd; }
    float getFogDensity() { return fogDensity; }
    glm::vec3 getFogColor() { return fogColor; }

    bool getUse1kTextures() { return use1kTextures; }

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
        shared_ptr<Parameters> inParameters,
        float inFogStart,
        float inFogEnd,
        float inFogDensity,
        glm::vec3 inFogColor,
        bool inUse1kTextures
    );

    ostream& operator<< (ostream &os);
    string getProjectRoot() {return string(getenv("PROJECT_ROOT"));}
    string getRenderPath() {return string(getenv("PRISM_ROOT"));}
    string getShaderPath() {return string(getenv("SHADER_ROOT"));}
    string getTexturePath() {return string(getenv("TEXTURE_ROOT"));}
    string getDataPath() {return string(getenv("DATA_ROOT"));}
};

#endif // SETTINGS_HPP
