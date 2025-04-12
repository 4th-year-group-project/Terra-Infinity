/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
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
    vector<vector<float>> heightmapData;
    vector<vector<uint8_t>> biomeData;
};

class World : public IRenderable {
private:
    long seed; // The seed for the world
    std::vector<std::shared_ptr<Chunk>> chunks; // The chunks that are loaded in the world
    std::vector<std::pair<int, int>> chunkRequests; // The chunks that are currently being generated to duplicate generation requests
    std::mutex chunkMutex; // The mutex for the chunk requests
    std::mutex requestMutex; // The mutex for the chunk requests

    std::shared_ptr<Settings> settings; // The settings for the world
    std::shared_ptr<Player> player; // The player object in the world
    std::shared_ptr<SkyBox> skyBox; // The sky box of the world
    float seaLevel; // The sea level of the world
    float maxHeight; // The maximum height of the world
    shared_ptr<Shader> terrainShader; // The shader for the terrain
    shared_ptr<Shader> oceanShader; // The shader for the ocean
    std::vector<std::shared_ptr<Texture>> terrainTextures; // The textures for the terrain
    GLuint biomeTextureArray; // The texture array for the biome textures

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
    World(std::shared_ptr<Settings> settings, std::shared_ptr<Player> player);
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
    float getMaxHeight() {return maxHeight;}
    void setMaxHeight(float inMaxHeight) {maxHeight = inMaxHeight;}
    std::pair<int, int> getPlayersCurrentChunk();
    void updateLoadedChunks();
    float distanceToChunkCenter(std::pair<int, int> chunkCoords);

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData(bool regenerate) override;
};

#endif // WORLD_HPP
