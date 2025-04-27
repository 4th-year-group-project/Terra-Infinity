/**
 * @file World.hpp
 * @author King Attalus II
 * @brief This file contains the World class, which is used to represent the world in the game. It handles the loading
 * and rendering of chunks.
 * @details The World class is responsible for managing the chunks in the world, including loading and rendering them.
 * It also handles the generation of the world using a seed and manages the player object within the world.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h> // This will be used to complete the http requests
#include <nlohmann/json.hpp> // This will be used to parse the json data

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "IRenderable.hpp"
#include "Chunk.hpp"
#include "SkyBox.hpp"
#include "Terrain.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "WaterFrameBuffer.hpp"
#include "Texture.hpp"

/**
 * @brief This struct is used to store the data that is received from the server when requesting a new chunk.
 * @details The struct contains the raw data received from the server, as well as the parsed data such as the heightmap,
 * biome data, and tree coordinates. It is used to store the data in a format that can be easily accessed by the World
 * class.
 *
 */
struct PacketData {
    std::vector<char> rawData;
    long seed;
    int cx;
    int cz;
    int num_vertices;
    int vx;
    int vz;
    int size;
    int lenHeightmapData;
    int biomeDataSize;
    int lenBiomeData;
    int treesSize;
    int treesCount;
    std::vector<std::vector<float>> heightmapData;
    std::vector<vector<uint8_t>> biomeData;
    std::vector<std::pair<float, float>> treesCoords;
};

/**
 * @brief This class represents the world in the game. It is responsible for managing the chunks and rendering them.
 * @details The World class is responsible for managing the chunks in the world, including loading and rendering them.
 * It also handles the generation of the world using a seed and manages the player object within the world.
 *
 */
class World : public IRenderable {
private:
    long seed; // The seed for the world
    std::vector<std::shared_ptr<Chunk>> chunks; // The chunks that are loaded in the world
    std::vector<std::pair<int, int>> chunkRequests; // The chunks that are currently being generated to duplicate generation requests
    std::mutex chunkMutex; // The mutex for the chunk requests
    std::mutex requestMutex; // The mutex for the chunk requests
    std::mutex terrainTextureArraysMutex; // The mutex for the terrain texture arrays

    std::shared_ptr<Settings> settings; // The settings for the world
    std::shared_ptr<Player> player; // The player object in the world
    std::shared_ptr<SkyBox> skyBox; // The sky box of the world
    float seaLevel; // The sea level of the world
    float maxHeight; // The maximum height of the world
    shared_ptr<Shader> terrainShader; // The shader for the terrain
    shared_ptr<Shader> oceanShader; // The shader for the ocean
    std::vector<shared_ptr<Texture>> terrainTextures; // The textures for the terrain
    std::vector<shared_ptr<TextureArray>> terrainTextureArrays; // The texture arrays for the terrain
    std::shared_ptr<WaterFrameBuffer> reflectionBuffer; // The framebuffer that will be used for the reflection textures
    std::shared_ptr<WaterFrameBuffer> refractionBuffer; // The framebuffer that will be used for the refraction textures
    std::vector<std::shared_ptr<Texture>> oceanTextures; // The textures for the water rendering
    int subbiomeTextureArrayMap[34] = {
        0,  // [0] Unused or Reserved
        0,  // [1] Boreal Forest Plains
        0,  // [2] Boreal Forest Hills
        0,  // [3] Boreal Forest Mountains
        1,  // [4] Grassland Plains
        1,  // [5] Grassland Hills
        2,  // [6] Grassland Rocky Fields
        1,  // [7] Grassland Terraced Fields
        3,  // [8] Tundra Plains
        3,  // [9] Tundra Blunt Mountains
        4,  // [10] Tundra Pointy Peaks
        5,  // [11] Savanna Plains
        5,  // [12] Savanna Mountains
        6,  // [13] Woodland Hills
        7,  // [14] Tropical Rainforest Plains
        8,  // [15] Tropical Rainforest Mountains
        9,  // [16] Tropical Rainforest Volcanoes
        7,  // [17] Tropical Rainforest Hills
        10, // [18] Temperate Rainforest Hills
        10, // [19] Temperate Rainforest Mountains
        11, // [20] Temperate Rainforest Swamp
        13, // [21] Temperate Seasonal Forest Hills (Autumnal)
        13, // [22] Temperate Seasonal Forest Mountains (Autumnal)
        12, // [23] Temperate Seasonal Forest Hills (Default)
        12, // [24] Temperate Seasonal Forest Mountains (Default)
        14, // [25] Desert Terraces
        15, // [26] Desert Dunes
        18, // [27] Desert Oasis
        17, // [28] Desert Ravines
        16, // [29] Desert Cracked
        19, // [30] Ocean Seabed
        19, // [31] Ocean Trenches
        19, // [32] Ocean Volcanic Islands
        20  // [33] Ocean Water Stacks
    };


    /*Functions required for async requesting*/
    std::unique_ptr<PacketData> readPacketData(char *data, int size);
    static size_t writeCallback(void *contents, size_t size, size_t nmemb, void *userp);
    std::unique_ptr<PacketData> requestNewChunk(int cx, int cz);
    int requestInitialChunks(std::vector<std::pair<int, int>> initialChunks);

public:
    World(
        long seed,
        std::vector<std::shared_ptr<Chunk>> chunks,
        std::shared_ptr<Settings> settings,
        std::shared_ptr<Player> player
    );
    World(
        std::shared_ptr<Settings> settings,
        std::shared_ptr<Player> player,
        std::shared_ptr<WaterFrameBuffer> reflectionBuffer,
        std::shared_ptr<WaterFrameBuffer> refractionBuffer
    );
    ~World() {};

    // These are the mutex controlled functions
    void addChunk(shared_ptr<Chunk> chunk);
    void removeChunk(int cx, int cz);
    std::shared_ptr<Chunk> getChunk(int cx, int cz);
    std::shared_ptr<Chunk> getChunk(int cx, int cz, bool &found);
    void clearChunks();
    int getChunkCount();
    bool isChunkRequested(int cx, int cz);
    void addChunkRequest(int cx, int cz);
    void removeChunkRequest(int cx, int cz);
    void printRequests();
    void printChunks();
    int regenerateSpawnChunks(glm::vec3 playerPos);
    int requestNewChunkAsync(int cx, int cz);  //The seed will come from the parameters

    long getSeed() {return seed;}
    void setSeed(long inSeed) {seed = inSeed;}
    std::shared_ptr<Settings> getSettings() {return settings;}
    void setSettings(std::shared_ptr<Settings> inSettings) {settings = inSettings;}
    std::shared_ptr<SkyBox> getSkyBox() {return skyBox;}
    void setSkyBox(std::shared_ptr<SkyBox> inSkyBox) {skyBox = inSkyBox;}
    std::shared_ptr<Player> getPlayer() {return player;}
    void setPlayer(std::shared_ptr<Player> inPlayer) {player = inPlayer;}
    float getSeaLevel() {return seaLevel;}
    void setSeaLevel(float inSeaLevel) {seaLevel = inSeaLevel;}
    std::vector<shared_ptr<Texture>> getTerrainTextures() {return terrainTextures;}
    void setTerrainTextures(std::vector<shared_ptr<Texture>> inTerrainTextures) {terrainTextures = inTerrainTextures;}
    std::vector<shared_ptr<TextureArray>> getTerrainTextureArrays() {return terrainTextureArrays;}
    void setTerrainTextureArrays(std::vector<shared_ptr<TextureArray>> inTerrainTextureArrays) {terrainTextureArrays = inTerrainTextureArrays;}
    float getMaxHeight() {return maxHeight;}
    void setMaxHeight(float inMaxHeight) {maxHeight = inMaxHeight;}
    std::pair<int, int> getPlayersCurrentChunk();
    void updateLoadedChunks();
    float distanceToChunkCenter(std::pair<int, int> chunkCoords);

    std::shared_ptr<WaterFrameBuffer> getReflectionBuffer() {return reflectionBuffer;}
    void setReflectionBuffer(std::shared_ptr<WaterFrameBuffer> inReflectionBuffer) {reflectionBuffer = inReflectionBuffer;}
    std::shared_ptr<WaterFrameBuffer> getRefractionBuffer() {return refractionBuffer;}
    void setRefractionBuffer(std::shared_ptr<WaterFrameBuffer> inRefractionBuffer) {refractionBuffer = inRefractionBuffer;}

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos,
        bool isWaterPass,
        bool isShadowPass,
        glm::vec4 plane
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;
};

#endif // WORLD_HPP
