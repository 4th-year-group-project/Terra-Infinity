/*
    Each large "super chunk" will be split up into 32x32 subchunks which will be loaded and unloaded
    dynamically by the renderer based on the player's position in the world and their view distance.

    This will allow us to generate and render subchunks at high resolutions than previously as we
    will only need to generate the subchunks that are within the player's view distance.
*/
#include <vector>
#include <memory>
#include <cmath>
#include <omp.h>
#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
#endif


#include "SubChunk.hpp"
#include "Chunk.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "WaterFrameBuffer.hpp"

/*
    This method will use the parents world coordinates and its Id to generate the world coordinates
    of the subchunk. This will allow the renderer to determine when to load and unload the subchunk
    based on the player's position in the world.
*/
vector<float> SubChunk::getSubChunkWorldCoords(shared_ptr<Settings> settings)
{
    // Get the world coordinates of the parent chunk
    vector<float> parentWorldCoords = parentChunk->getChunkWorldCoords();
    int parentSize = settings->getChunkSize();
    parentSize++;
    // Get the local coordinates of the subchunk
    vector<int> subChunkLocalCoords = getSubChunkCoords();
    // Calculate the world coordinates of the subchunk
    float x = parentWorldCoords[0] + subChunkLocalCoords[0]; //- (parentSize - 1) / 2;
    float z = parentWorldCoords[1] + subChunkLocalCoords[1]; // - (parentSize - 1) / 2;
    return vector<float>{x, z};
}

SubChunk::SubChunk(
    int inId,
    shared_ptr<Chunk> inParentChunk,
    shared_ptr<Settings> settings,
    vector<int> inSubChunkCoords,
    vector<vector<float>> inHeights,
    vector<vector<uint8_t>> inBiomes,
    shared_ptr<Shader> inTerrainShader,
    shared_ptr<Shader> inOceanShader,
    vector<shared_ptr<Texture>> inTerrainTextures,
    shared_ptr<WaterFrameBuffer> inReflectionBuffer,
    shared_ptr<WaterFrameBuffer> inRefractionBuffer,
    vector<shared_ptr<Texture>> inOceanTextures
):
    id(inId),
    size(settings->getSubChunkSize()),
    resolution(settings->getSubChunkResolution()),
    parentChunk(inParentChunk),
    subChunkCoords(inSubChunkCoords),
    heights(inHeights),
    biomes(inBiomes),
    terrainShader(inTerrainShader),
    oceanShader(inOceanShader),
    terrainTextures(inTerrainTextures),
    reflectionBuffer(inReflectionBuffer),
    refractionBuffer(inRefractionBuffer),
    oceanTextures(inOceanTextures)
{
    // Generate the terrain object for the subchunk
    terrain = make_shared<Terrain>(
        inHeights,
        make_shared<vector<vector<uint8_t>>>(inBiomes),
        settings,
        getSubChunkWorldCoords(settings),
        inTerrainShader,
        inTerrainTextures,
        parentChunk->getTerrainTextureArrays(),
        parentChunk->getSubbiomeTextureArrayMap()
    );

    ocean = make_shared<Ocean>(
        vector<float>{0.0f, 0.0f},
        getSubChunkWorldCoords(settings),
        settings,
        inOceanShader,
        inReflectionBuffer,
        inRefractionBuffer,
        inOceanTextures
    );
}

SubChunk::SubChunk(
    int inId,
    shared_ptr<Chunk> inParentChunk,
    shared_ptr<Settings> settings,
    float inResolution,
    vector<int> inSubChunkCoords,
    vector<vector<float>> inHeights,
    vector<vector<uint8_t>> inBiomes,
    shared_ptr<Shader> inTerrainShader,
    shared_ptr<Shader> inOceanShader,
    vector<shared_ptr<Texture>> inTerrainTextures,
    shared_ptr<WaterFrameBuffer> inReflectionBuffer,
    shared_ptr<WaterFrameBuffer> inRefractionBuffer,
    vector<shared_ptr<Texture>> inOceanTextures
):
    id(inId),
    size(settings->getSubChunkSize()),
    resolution(inResolution),
    parentChunk(inParentChunk),
    subChunkCoords(inSubChunkCoords),
    heights(inHeights),
    biomes(inBiomes),
    terrainShader(inTerrainShader),
    oceanShader(inOceanShader),
    terrainTextures(inTerrainTextures),
    reflectionBuffer(inReflectionBuffer),
    refractionBuffer(inRefractionBuffer),
    oceanTextures(inOceanTextures)
{
    // Generate the terrain object for the subchunk
    terrain = make_shared<Terrain>(
        inHeights,
        make_shared<vector<vector<uint8_t>>>(inBiomes),
        inResolution,
        settings,
        getSubChunkWorldCoords(settings),
        inTerrainShader,
        inTerrainTextures,
        parentChunk->getTerrainTextureArrays(),
        parentChunk->getSubbiomeTextureArrayMap()
    );

    ocean = make_shared<Ocean>(
        vector<float>{0.0f, 0.0f},
        getSubChunkWorldCoords(settings),
        settings,
        inOceanShader,
        inReflectionBuffer,
        inRefractionBuffer,
        inOceanTextures
    );
}

/*
    THis method is the class destructor which will be called when the subchunk is destroyed
*/
SubChunk::~SubChunk()
{
    // Nothing to do here
}


void SubChunk::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
)
{
    // We only want to render the terrain to produce the reflection or refraction buffers
    terrain->render(view, projection, lights, viewPos, isWaterPass, isShadowPass, plane);
    // Enable alpha belending for the ocean
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    if (!isWaterPass){
        ocean->render(view, projection, lights, viewPos, isWaterPass, isShadowPass, plane);
    }
    // Disable alpha blending for the terrain
    glDisable(GL_BLEND);
    // Render the terrain object
}

void SubChunk::setupData()
{
    // Do nothing
    // // Setup the terrain object
    // terrain->setupData();
}

void SubChunk::updateData(bool)
{
    // Do nothing
    // // Update the terrain object
    // terrain->updateData();
}