/**
 * @file SubChunk.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the SubChunk class.
 * @details This class will represent a subchunk of the world. It will be used to render the terrain
 * and ocean obects. Each large "superchunk" will be split up into 32x32 subchunks which will be
 * loaded and unloaded dynamically by the renderer based on the player's position in the world
 * and their view distance.
 * 
 * @details This will allow us to generate and render subchunks at high resolutions than previously
 * as we will only need to generate the subchunks that are within the player's view distance.
 * @version 1.0
 * @date 2025
 * 
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

/**
 * @brief This method will get the world coordinates of the subchunk
 * 
 * @details This method will get the world coordinates of the subchunk by getting the world by
 * using its parent chunk's world coordinates and its Id. It's own Id is unique to the superchunk
 * and based its local coordinates in the superchunk.
 * 
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * 
 * @return std::vector<float> The world coordinates of the subchunk
 * 
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

/**
 * @brief Construct a new SubChunk object with the given arguments
 * 
 * @details This constructor will create a subchunk object with the given arguments. It will
 * initialize the subchunk with the given arguments and generate the terrain and ocean objects
 * for the subchunk.
 * 
 * @param inId [in] int The id of the subchunk
 * @param inParentChunk [in] std::shared_ptr<Chunk> The parent chunk of the subchunk
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * @param inSubChunkCoords [in] std::vector<int> The local coordinates of the subchunk
 * @param inHeights [in] std::vector<std::vector<float>> The heights of the subchunk
 * @param inBiomes [in] std::vector<std::vector<uint8_t>> The biomes of the subchunk
 * @param inTerrainShader [in] std::shared_ptr<Shader> The shader for the terrain
 * @param inOceanShader [in] std::shared_ptr<Shader> The shader for the ocean
 * @param inTerrainTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the terrain
 * @param inReflectionBuffer [in] std::shared_ptr<WaterFrameBuffer> The reflection buffer for the ocean
 * @param inRefractionBuffer [in] std::shared_ptr<WaterFrameBuffer> The refraction buffer for the ocean
 * @param inOceanTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the ocean
 * 
 */ 
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

/**
 * @brief Construct a new SubChunk object with the given arguments
 * 
 * @details This constructor will create a subchunk object with the given arguments. It will
 * initialize the subchunk with the given arguments and generate the terrain and ocean objects
 * for the subchunk.
 * 
 * @param inId [in] int The id of the subchunk
 * @param inParentChunk [in] std::shared_ptr<Chunk> The parent chunk of the subchunk
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * @param inResolution [in] float The resolution of the subchunk
 * @param inSubChunkCoords [in] std::vector<int> The local coordinates of the subchunk
 * @param inHeights [in] std::vector<std::vector<float>> The heights of the subchunk
 * @param inBiomes [in] std::vector<std::vector<uint8_t>> The biomes of the subchunk
 * @param inTerrainShader [in] std::shared_ptr<Shader> The shader for the terrain
 * @param inOceanShader [in] std::shared_ptr<Shader> The shader for the ocean
 * @param inTerrainTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the terrain
 * @param inReflectionBuffer [in] std::shared_ptr<WaterFrameBuffer> The reflection buffer for the ocean
 * @param inRefractionBuffer [in] std::shared_ptr<WaterFrameBuffer> The refraction buffer for the ocean
 * @param inOceanTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the ocean
 * 
 */
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

/**
 * @brief Default constructor for the SubChunk class allowing for standard cleanup
 */
SubChunk::~SubChunk()
{
    // Nothing to do here
}

/**
 * @brief Renders the subchunk
 * 
 * @details This method will render the subchunk by rendering the terrain and ocean objects
 * for the subchunk.
 * 
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] std::vector<std::shared_ptr<Light>> The lights in the scene
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether the water pass is being rendered
 * @param isShadowPass [in] bool Whether the shadow pass is being rendered
 * @param plane [in] glm::vec4 The plane used for the water pass
 * 
 * @return void
 * 
 */
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
}

/**
 * @brief This method will set up the data for the subchunk
 * 
 * @details Currently this method does nothing as the data is already set up in the constructor.
 * 
 */
void SubChunk::setupData()
{
    // Do nothing
}

/**
 * @brief This method will update the data for the subchunk
 * 
 * @details Currently this method does nothing as the data it contains is static and does not
 * change.
 * 
 * @param regenerate [in] bool Whether to regenerate the data or not
 * 
 * @return void
 * 
 */
void SubChunk::updateData(bool regenerate, int frame_counter)
{
    // Do nothing

}