#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>


using namespace std;

/*
    This class will hold all of the settings for the project which will include the specific
    settings for the project along with settings for the renderer such as the window size and the
    graphics settings and the render distance.
*/
class Settings
{
private:
    int windowWidth; // The width of the window
    int windowHeight; // The height of the window
    bool fullscreen; // Whether the window is fullscreen or not
    int renderDistance; // The render distance of the renderer
    int chunkSize; // The size of the chunks in the world
    int subChunkSize; // The size of the subchunks in the world
    int subChunkResolution; // The resolution of the subchunks in the world
    char filePathDelimitter; // The delimitter for the file paths

public:
    Settings(
        int inWindowWidth,
        int inWindowHeight,
        bool inFullscreen,
        int inRenderDistance,
        int inChunkSize,
        int inSubChunkSize,
        int inSubChunkResolution,
        char inFilePathDelimitter
    ):
        windowWidth(inWindowWidth),
        windowHeight(inWindowHeight),
        fullscreen(inFullscreen),
        renderDistance(inRenderDistance),
        chunkSize(inChunkSize),
        subChunkSize(inSubChunkSize),
        subChunkResolution(inSubChunkResolution),
        filePathDelimitter(inFilePathDelimitter) {};
    Settings(): Settings(1920, 1080, true, 16, 1024, 32, 2, '/') {};
    ~Settings() {};

    int getWindowWidth() { return windowWidth; }
    int getWindowHeight() { return windowHeight; }
    bool getFullscreen() { return fullscreen; }
    int getRenderDistance() { return renderDistance; }
    int getChunkSize() { return chunkSize; }
    int getSubChunkSize() { return subChunkSize; }
    int getSubChunkResolution() { return subChunkResolution; }
    char getFilePathDelimitter() { return filePathDelimitter; }

    void updateSettings(
        int inWindowWidth,
        int inWindowHeight,
        bool inFullscreen,
        int inRenderDistance,
        int inChunkSize,
        int inSubChunkSize,
        int inSubChunkResolution
    );

    ostream& operator<< (ostream &os);
    string getProjectRoot() {return string(getenv("PROJECT_ROOT"));}
    string getRenderPath() {return string(getenv("PRISM_ROOT"));}
    string getShaderPath() {return string(getenv("SHADER_ROOT"));}
    string getTexturePath() {return string(getenv("TEXTURE_ROOT"));}
    string getDataPath() {return string(getenv("DATA_ROOT"));}
};

#endif
