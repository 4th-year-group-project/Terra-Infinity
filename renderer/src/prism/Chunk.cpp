/**
 * @file Chunk.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Chunk class, which is used to represent a superchunk generated
 * by the world generation scripts.
 * @details The Chunk class is responsible for managing the subchunks within the chunk, loading and unloading subchunks,
 * and rendering the chunk. A superchunk is a large 1024x1024 chunk which is split into smaller 32x32 subchunks, though
 * these values are configurable.
 * @version 1.0
 * @date 2025
 *
 */

#include <vector>
#include <unordered_map>
#include <memory>
#include <cmath>
#include <omp.h>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Chunk.hpp"
#include "SubChunk.hpp"
#include "Settings.hpp"
#include "IRenderable.hpp"
#include "Shader.hpp"
#include "Utility.hpp"
#include "Texture.hpp"
#include "WaterFrameBuffer.hpp"

/**
 * @brief Construct a new Chunk object from the given parameters, other values are set to default values.
 *
 * @details This constructor will create a chunk at the given coordinates and with the given heightmap data.
 * This data is provided by the world generation scripts. It also stores pointers to shared information such as
 * shaders, textures, and framebuffers which are all shared across subchunks. 
 *
 * @param inId [in] long The unique identifier for the chunk which is chunkX + chunkZ * MAX_INT
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * @param inChunkCoords [in] std::vector<int> The coordinates of the chunk in the chunk space
 * @param inHeightmapData [in] std::vector<std::vector<float>> The heightmap data for the chunk
 * @param inBiomeData [in] std::vector<std::vector<uint8_t>> The biome data for the chunk
 * @param inTerrainShader [in] std::shared_ptr<Shader> The shader for the terrain object
 * @param inOceanShader [in] std::shared_ptr<Shader> The shader for the ocean object
 * @param inTerrainTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the terrain
 * @param inTerrainTextureArrays [in] std::vector<std::shared_ptr<TextureArray>> The texture arrays for the terrain
 * @param inReflectionBuffer [in] std::shared_ptr<WaterFrameBuffer> The framebuffer that will be used for the reflection
 * @param inRefractionBuffer [in] std::shared_ptr<WaterFrameBuffer> The framebuffer that will be used for the refraction
 * @param inOceanTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the ocean
 * @param subbiomeTextureArrayMap [in] const int* The texture array map for the subbiomes
 *
 */
Chunk::Chunk(
    long inId,  // The unique identifier for the chunk which is chunkX + chunkZ * 1024
    shared_ptr<Settings> settings,
    vector<int> inChunkCoords,
    vector<vector<float>> inHeightmapData,
    vector<vector<uint8_t>> inBiomeData,
    shared_ptr<Shader> inTerrainShader,
    shared_ptr<Shader> inOceanShader,
    vector<shared_ptr<Texture>> inTerrainTextures,
    vector<shared_ptr<TextureArray>> inTerrainTextureArrays,
    shared_ptr<WaterFrameBuffer> inReflectionBuffer,
    shared_ptr<WaterFrameBuffer> inRefractionBuffer,
    vector<shared_ptr<Texture>> inOceanTextures,
    const int* subbiomeTextureArrayMap
):
    id(inId),
    size(settings->getChunkSize()),
    subChunkSize(settings->getSubChunkSize()),
    subChunkResolution(settings->getSubChunkResolution()),
    settings(settings),
    chunkCoords(inChunkCoords),
    heightmapData(inHeightmapData),
    biomeData(inBiomeData),
    terrainShader(inTerrainShader),
    oceanShader(inOceanShader),
    terrainTextures(inTerrainTextures),
    terrainTextureArrays(inTerrainTextureArrays),
    reflectionBuffer(inReflectionBuffer),
    refractionBuffer(inRefractionBuffer),
    oceanTextures(inOceanTextures),
    subbiomeTextureArrayMap(subbiomeTextureArrayMap)
{
    // Initialize the loadedSubChunks and cachedSubChunks vectors to the size of the chunk
    loadedSubChunks = vector<shared_ptr<SubChunk>>((size - 1) / (subChunkSize - 1) * (size - 1) / (subChunkSize - 1));
    cachedSubChunks = vector<shared_ptr<SubChunk>>((size - 1) / (subChunkSize - 1) * (size - 1) / (subChunkSize - 1));
    // Make all of the entries in the loadedSubChunks map nullptr
    for (int i = 0; i < ((size - 1) / (subChunkSize - 1)) * ((size - 1) / (subChunkSize - 1)); i++){
        loadedSubChunks[i] = nullptr;
        cachedSubChunks[i] = nullptr;
    }
    setupData();
}

/**
 * @brief This method will convert the chunk coordinates which are in chunk space to world space coordinates
 *
 * @returns std::vector<float> A vector of floats containing the world coordinates of the chunk
 *
 */
vector<float> Chunk::getChunkWorldCoords()
{
    vector<float> worldCoords;
    worldCoords.push_back(chunkCoords[0] * (size -1));
    worldCoords.push_back(chunkCoords[1] * (size -1));
    return worldCoords;
}

/**
 * @brief This method will return a vector of all the loaded subchunks within the chunk
 *
 * @returns std::vector<shared_ptr<SubChunk>> A vector of shared pointers to the loaded subchunks
 *
 */
vector<shared_ptr<SubChunk>> Chunk::getLoadedSubChunks()
{
    vector<shared_ptr<SubChunk>> subChunks;
    for (auto subChunk : loadedSubChunks){
        if (subChunk != nullptr){
            subChunks.push_back(subChunk);
        }
    }
    return subChunks;
}


/**
 * @brief This method will take a world position and return the subchunk id of that position within the
 * chunk. The subchunk id is a unique identifier for each subchunk within the chunk. If the position
 * is not within the chunk then -1 will be returned.
 *
 * @param position [in] glm::vec3 The position of the subchunk in world coordinates
 *
 * @returns int The subchunk id of the position within the chunk
 * @returns -1 if the position is not within the chunk
 */
int Chunk::getSubChunkId(glm::vec3 position)
{
    double d_size = static_cast<double>(size);
    // Determine if the position is within the chunk on the global chunk space grid
    int chunkCoordX = floor((position.x) / d_size);
    int chunkCoordZ = floor((position.z) / d_size);
    // Compare it to the global chunk coordinates of the chunk
    if (chunkCoordX != chunkCoords[0] || chunkCoordZ != chunkCoords[1]){
        return -1;
    }
    // Determine the subchunk coordinates within the chunk
    int startingChunkX = (chunkCoords[0] * size) - (size /2);
    int startingChunkZ = (chunkCoords[1] * size) - (size /2);
    // The subchunk ids are 0-1023 and we start with the subchunk in the bottom left corner of the
    // chunk and move to the right and then up
    glm::vec3 localPosition = glm::vec3(position.x - startingChunkX, position.y, position.z - startingChunkZ);
    int subChunkX = floor(localPosition.x / static_cast<double>(subChunkSize));
    int subChunkZ = floor(localPosition.z / static_cast<double>(subChunkSize));
    // The x coordinate determines the column and the z coordinate determines the row if we were to
    // represent the subchunks in a 32x32 grid
    return (subChunkZ * subChunkSize) + subChunkX;
}

/**
 * @brief This method will add a subchunk to the loadedSubChunks map. If the subchunk is already loaded
 * then it will be reloaded with the new resolution. If the subchunk is not loaded then it will be
 * generated and added to the loadedSubChunks map.
 *
 * @param id [in] int The id of the subchunk to add
 * @param resolution [in] float The resolution of the subchunk
 *
 * @returns void
 *
 */
void Chunk::addSubChunk(int id, float resolution){
    // First we check to see if the subchunk is already `loaded` in the loadedSubChunks map
    if (loadedSubChunks[id] != nullptr){
        // Get the resolution of the subchunk
        float subChunkResolution = loadedSubChunks[id]->getResolution();
        if (subChunkResolution != resolution){
            // If the subchunk is already loaded but the resolution is different then we need to
            // reload the subchunk with the new resolution
            loadedSubChunks[id].reset();
            loadedSubChunks[id] = nullptr;
            addSubChunk(id, resolution);
        }
    // If the subchunk is not loaded then we check to see if it is in the cachedSubChunks map
    } else if (cachedSubChunks[id] != nullptr){
        // If the subchunk is in the cachedSubChunks map then we move it to the loadedSubChunks map
        float subChunkResolution = cachedSubChunks[id]->getResolution();
        if (subChunkResolution != resolution){
            // If the subchunk is already loaded but the resolution is different then we need to
            // reload the subchunk with the new resolution
            cachedSubChunks[id].reset();
            cachedSubChunks[id] = nullptr;
            addSubChunk(id, resolution);
        } else {
            // If the subchunk is in the cachedSubChunks map and the resolution is the same then we
            // move it to the loadedSubChunks map
            loadedSubChunks[id] = cachedSubChunks[id];
            cachedSubChunks[id].reset();
            cachedSubChunks[id] = nullptr;
        }
    } else {
        // If the subchunk is not in the loadedSubChunks or cachedSubChunks map then we need to
        // generate the subchunk and add it to the loadedSubChunks map
        // We will generate the subchunk using the parent chunk's vertices
        // The subchunk will be generated based on the subchunk id

        // We convert the subchunk id back into the starting chunk local coordinate for the subchunk
        // For example the id is 343 then it is the 10th row and 23rd column of the 32x32 grid
        int bottomLeftX = (id % (subChunkSize + 1)) * (subChunkSize -1);  // The coloumn of the subchunk in the 32x32 grid
        int bottomLeftZ = (id / (subChunkSize + 1)) * (subChunkSize -1);  // The row of the subchunk in the 32x32 grid
        vector<vector<float>> subChunkHeights = vector<vector<float>>(subChunkSize + 2, vector<float>(subChunkSize + 2));
        vector<vector<uint8_t>> subChunkBiomes = vector<vector<uint8_t>>(subChunkSize + 2, vector<uint8_t>(subChunkSize + 2));
        // We also have to account for the border vertices. Suppose we have subchunk 0,0 then
        // the bottom left corner will actually be at 1,1 within the chunk vertices and we need to
        // extract the 34x34 subchunk to account for the border vertices. This would be the same as
        // extracting 0,0 to 33,33 from the chunk vertices. Hence we do not need to modify the
        // bottomLeftX and bottomLeftZ values as we can just complete two additional iterations

        for (int z = bottomLeftZ; z < bottomLeftZ + subChunkSize + 2; z++){
            for (int x = bottomLeftX; x < bottomLeftX + subChunkSize + 2; x++){
                subChunkHeights[z - bottomLeftZ][x - bottomLeftX] = heightmapData[z][x];
            }
        }
        for (int z = bottomLeftZ; z < bottomLeftZ + subChunkSize + 2; z++){
            for (int x = bottomLeftX; x < bottomLeftX + subChunkSize + 2; x++){
                subChunkBiomes[z - bottomLeftZ][x - bottomLeftX] = biomeData[z][x];
            }
        }
        // Generate the subchunk
        shared_ptr<SubChunk> subChunk = make_shared<SubChunk>(
            id,
            shared_from_this(),
            settings,
            resolution,
            vector<int>{bottomLeftX, bottomLeftZ},
            subChunkHeights,
            subChunkBiomes,
            terrainShader,
            oceanShader,
            terrainTextures,
            reflectionBuffer,
            refractionBuffer,
            oceanTextures
        );
        // Add the subchunk to the loadedSubChunks map
        loadedSubChunks[id] = subChunk;
    }
}

/**
 * @brief This method will return the distance from the player to the chunk. This is used to determine
 * if the chunk is within the render distance of the player.
 *
 * @param playerPos [in] glm::vec3 The position of the player in world coordinates
 *
 * @returns float The distance from the player to the chunk
 *
 */
float Chunk::getDistanceToChunk(glm::vec3 playerPos){
    // Get the world coordinates of the chunk
    vector<float> chunkWorldCoords = getChunkWorldCoords();
    // We know that a chunk starts at chunkX, chunkZ and ends at chunkX + size, chunkZ + size
    // If a player is at a position x, z then we want to calculate the closest x and z to the player
    // that is within the chunk

    // Get the closest point between the playerPos and the chunk to determine the distance
    float closestX = max(chunkWorldCoords[0], min(playerPos.x, chunkWorldCoords[0] + size));
    float closestZ = max(chunkWorldCoords[1], min(playerPos.z, chunkWorldCoords[1] + size));

    return sqrt(pow(playerPos.x - closestX, 2) + pow(playerPos.z - closestZ, 2));
}

/*
    This method will take in a subchunk id (0-(size/subChunkSize)^2) and return the world
    coordinates of the subchunk for its origin (bottom left corner). The subchunk id is a unique
    identifier within the chunk for each subchunk.
*/
/**
 * @brief This method will take in a subchunk id (0-(size/subChunkSize)^2) and return the world
 * coordinates of the subchunk for its origin (bottom left corner). The subchunk id is a unique
 * identifier within the chunk for each subchunk.
 *
 * @param id [in] int The id of the subchunk to get the world coordinates for
 *
 * @returns std::vector<float> A vector of floats containing the world coordinates of the subchunk
 *
 */
vector<float> Chunk::getSubChunkWorldCoords(int id){
    // The size of the subchunks array is (subChunkSize + 1)*(subChunkSize + 1) and the subchunks
    // themselves are (subChunkSize - 1) x (subChunkSize - 1) in world coords

    // Get the world coordinates of the chunk
    vector<float> chunkWorldCoords = getChunkWorldCoords();
    // Get the subchunk coordinates within the chunk
    int bottomLeftX = (id % (subChunkSize + 1)) * (subChunkSize - 1);
    int bottomLeftZ = (id / (subChunkSize + 1)) * (subChunkSize - 1);
    // Get the world coordinates of the subchunk
    float subChunkX = bottomLeftX + chunkWorldCoords[0];
    float subChunkZ = bottomLeftZ + chunkWorldCoords[1];
    return vector<float>{subChunkX, subChunkZ};
}

/*
    This method will use the player's position and their render distance to check which subchunks
    need to be loaded and which subchunks need to be unloaded. These subchunks will be returned as
    a vector of subchunk ids to then be loaded by the renderer. The vector that is returned will be
    (size / subChunkSize) * (size / subChunkSize) with entires -1,0,1 to determine if the subchunk
    needs to be deleted, unloaded or loaded.
*/
/**
 * @brief This method will use the player's position and their render distance to determine the action for each subchunk
 * and whether they need to be loaded, cached, or unloaded.
 *
 * @details These subchunks will be returned as a vector of subchunk ids to then be loaded by the renderer. The vector
 * that is returned will be (size / subChunkSize) * (size / subChunkSize) with entries -1,0,<resolution> to determine
 * if the subchunk needs to be deleted, unloaded or loaded. The resolution is the resolution of the mesh that will
 * be produce.
 *
 * @param playerPos [in] glm::vec3 The position of the player in world coordinates
 * @param settings [in] Settings The settings object
 *
 * @returns std::vector<int> A vector of integers containing the subchunk ids and their render status
 * @returns std::vector<int>() An empty vector if the player is not within the render distance of the chunk
 *
 */
vector<int> Chunk::checkRenderDistance(glm::vec3 playerPos, Settings settings){
    // The render distance is the number of subchunks that the player can see in each direction
    // from their current position.
    int renderDistance = settings.getRenderDistance();
    // Check if the player is within the render distance of the chunk
    float distance = getDistanceToChunk(playerPos);
    // We have to multiply by subChunkSize to get the actual distance in world space
    if (distance > 2 * renderDistance * subChunkSize){
        // If the player is not within the render distance of the chunk then we return an empty
        // vector as no subchunks need to be loaded
        return vector<int>();
    }
    // If the player is within the render distance of the chunk then we need to determine which
    // subchunks need to be loaded and which subchunks need to be unloaded
    vector<int> subChunksToLoad = vector<int>( ((size -1) / (subChunkSize - 1)) * ((size -1) / (subChunkSize - 1)));
    // Iterate through all of the subchunks within the chunk and determine their render status
    for (int i = 0; i < static_cast<int>(subChunksToLoad.size()); i++){
        // Get the subchunk id
        int subChunkId = i;
        // Get the world coordinates of the subchunk
        vector<float> subChunkWorldCoords = getSubChunkWorldCoords(subChunkId);
        float subChunkX = subChunkWorldCoords[0];
        float subChunkZ = subChunkWorldCoords[1];
        float subChunkMidX = subChunkX + (subChunkSize / 2.0);
        float subChunkMidZ = subChunkZ + (subChunkSize / 2.0);
        // Get the distance to the subchunk
        float distanceToSubChunk = sqrt(pow(playerPos.x - subChunkMidX, 2) + pow(playerPos.z - subChunkMidZ, 2));
        if (distanceToSubChunk > 2 * renderDistance * subChunkSize){
            // The subchunk is far enough away that it should be deleted to save memory
            subChunksToLoad[i] = -1;
        } else if (distanceToSubChunk > renderDistance * subChunkSize){
            // The subchunk is far enough away that it should be unloaded so it is not rendered
            subChunksToLoad[i] = 0;
        } else {
            // We now need to determine the subchunk's resolution based on the distance from the
            // player to the center of the subchunk where the resolution is settings.getSubChunkResolution()
            // at the player and decays to 1 at the edge of the render distance

            // If the render distance is 16 then we will be able to see 15 subchunks in each direction
            // from the player plus the subchunk that the player is in (roughly).

            // Uncomment this to implement our method of LOD
            // if (distanceToSubChunk < sqrt(2 * pow(subChunkSize * (renderDistance  / 8.0), 2))){
            //     // The subchunk is within the player's render distance and should be loaded and rendered
            //     subChunksToLoad[i] = settings.getSubChunkResolution();
            // } else if (distanceToSubChunk < sqrt(2 * pow(subChunkSize * (renderDistance  / 8.0) * 3.0, 2))){
            //     // The subchunk is within the player's render distance and should be loaded and rendered
            //     subChunksToLoad[i] = settings.getSubChunkResolution() * 0.5;
            // } else if (distanceToSubChunk < sqrt(2 * pow(subChunkSize * (renderDistance  / 8.0) * 5.0, 2))){
            //     // The subchunk is within the player's render distance and should be loaded and rendered
            //     subChunksToLoad[i] = settings.getSubChunkResolution() * 0.25;
            // } else {
            //     // The subchunk is within the player's render distance and should be loaded and rendered
            //     subChunksToLoad[i] = 1;
            // }
            subChunksToLoad[i] = 2;
        }

    }
    // We are going to use the centre of the chunk as the reference point for the render distance
    return subChunksToLoad;
}

/**
 * @brief This method will be used to determine and update the subchunks that are loaded within the
 * chunk based on the player's position in the world and the render distance.
 *
 * @param playerPos [in] glm::vec3 The position of the player in world coordinates
 * @param settings [in] Settings The settings object
 *
 * @returns void
 */
void Chunk::updateLoadedSubChunks(glm::vec3 playerPos, Settings settings){
    // Get the modifications that are required
    // We need to shift the playerPos by the inverse of the mid point of the chunk to get the
    // position relative to the rendered world coordinates
    vector<int> subChunksToLoad = checkRenderDistance(playerPos, settings);
    // Check if the vector is empty as that means nothing needs to be loaded and the loaded
    // subchunks should be empty
    if (subChunksToLoad.size() == 0){
        // We need to ensure that all of the subchunks are unloaded and uncached
        for (int i = 0; i < static_cast<int>(loadedSubChunks.size()); i++){
            if (loadedSubChunks[i] != nullptr){
                loadedSubChunks[i].reset();
                loadedSubChunks[i] = nullptr;
            }
            if (cachedSubChunks[i] != nullptr){
                cachedSubChunks[i].reset();
                cachedSubChunks[i] = nullptr;
            }
        }
        return;
    }
    // Iterate through the subchunks and load, unload or delete them based on the modifications
    for (int i = 0; i < static_cast<int>(subChunksToLoad.size()); i++){
        // Get the subchunk id
        int subChunkId = i;
        // Get the modification that is required
        int modification = subChunksToLoad[i];
        if (modification == -1){
            // The subchunk needs to be deleted
            deleteSubChunk(subChunkId);
        } else if (modification == 0){
            // The subchunk needs to be unloaded
            unloadSubChunk(subChunkId);
        } else {
            // The subchunk needs to be loaded
            addSubChunk(subChunkId, modification);
        }
    }
}

/**
 * @brief This method will unload a subchunk from the loadedSubChunks map and move it to the
 * cachedSubChunks map. This is used to save memory when the subchunk is not in use.
 *
 * @param id [in] int The id of the subchunk to unload
 *
 * @returns void
 */
void Chunk::unloadSubChunk(int id){
    // Check if the specific index is not nullptr
    if (loadedSubChunks[id] != nullptr){
        // Move the subchunk to the cachedSubChunks vector and delete the subchunk from the loadedSubChunks vector
        cachedSubChunks[id] = loadedSubChunks[id];
        loadedSubChunks[id].reset();
        loadedSubChunks[id] = nullptr;
    }
}

/**
 * @brief This method will delete a subchunk from the loadedSubChunks and cachedSubChunks maps.
 * This is used to save memory when the subchunk is not in use.
 *
 * @param id [in] int The id of the subchunk to delete
 *
 * @returns void
 *
 */
void Chunk::deleteSubChunk(int id){
    // Check to see if the subchunk is loaded in the loadedSubChunks vector
    if (loadedSubChunks[id] != nullptr){
        // Delete the subchunk from the loadedSubChunks vector
        loadedSubChunks[id].reset();
        loadedSubChunks[id] = nullptr;
    }
    // Check to see if the subchunk is loaded in the cachedSubChunks vector
    if (cachedSubChunks[id] != nullptr){
        // Delete the subchunk from the cachedSubChunks vector
        cachedSubChunks[id].reset();
        cachedSubChunks[id] = nullptr;
    }
}

/**
 * @brief This method will load all of the subchunks within the chunk. This is used to load all
 * of the subchunks when the chunk is first created.
 *
 * @details This method is very useful for debugging and testing purposes.
 * 
 * @returns void
 *
 */
void Chunk::loadAllSubChunks(){
    // Iterate through all of the subchunks and load them
    for (int i = 0; i < ((size - 1) / (subChunkSize - 1)) * ((size - 1) / (subChunkSize - 1)); i++){
        addSubChunk(i, 1);
    }
}

/**
 * @brief This destructor will call the default destructor operations for this class.
 *
 */
Chunk::~Chunk()
{
    // Nothing to do here
}

/**
 * @brief Renders the axes in the scene
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
void Chunk::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
)
{
    // Render all of the loaded subchunks
    for (int i = 0; i < static_cast<int>(loadedSubChunks.size()); i++){
        if (loadedSubChunks[i] != nullptr){
            loadedSubChunks[i]->render(
                view,
                projection,
                lights,
                viewPos,
                isWaterPass,
                isShadowPass,
                plane
            );
        }
    }
}

/**
 * @brief This method will setup the data for the chunk. This is used to setup the data for the
 * subchunks within the chunk.
 *
 * @returns void
 *
 */
void Chunk::setupData()
{
    // Do nothing
}

/**
 * @brief This method will update the data for the chunk. This is used to update the data for the
 * subchunks within the chunk.
 *
 * @param bool [in] bool Whether to update the data or not
 *
 * @returns void
 *
 */
void Chunk::updateData(bool)
{
    // Do nothing
}


shared_ptr<SubChunk> Chunk::getSubChunk(int cx, int cz){
    // Get the subchunk id from the coordinates
    int id = (cz * (size - 1) / (subChunkSize - 1)) + cx;
    // Check if the subchunk is loaded
    if (loadedSubChunks[id] != nullptr){
        return loadedSubChunks[id];
    }
    // Check if the subchunk is cached
    if (cachedSubChunks[id] != nullptr){
        return cachedSubChunks[id];
    }
    // If the subchunk is not loaded or cached then return nullptr
    return nullptr;
}
