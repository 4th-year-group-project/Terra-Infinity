/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <memory>

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

using namespace std;

struct PacketData {
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
    vector<shared_ptr<Chunk>> chunks; // The chunks that are loaded in the world
    shared_ptr<Player> player; // The player object in the world
    shared_ptr<SkyBox> skyBox; // The sky box of the world
    float seaLevel; // The sea level of the world
    float maxHeight; // The maximum height of the world
    vector<long> generatingChunks; // The chunks that are currently being generated to duplicate generation requests
    shared_ptr<Shader> terrainShader; // The shader for the terrain
    shared_ptr<Shader> oceanShader; // The shader for the ocean
    vector<shared_ptr<Texture>> terrainTextures; // The textures for the terrain
    vector<shared_ptr<TextureArray>> terrainTextureArrays; // The texture arrays for the terrain

    long generateRandomSeed();
    unique_ptr<PacketData> readPacketData(char *data, int len);
public:
    World(
        long seed,
        vector<shared_ptr<Chunk>> chunks,
        shared_ptr<Settings> settings,
        shared_ptr<Player> player
    );
    World(shared_ptr<Settings> settings, shared_ptr<Player> player);
    ~World() {};

    long getSeed() {return seed;}
    vector<shared_ptr<Chunk>> getChunks() {return chunks;}
    shared_ptr<Player> getPlayer() {return player;}
    float getSeaLevel() {return seaLevel;}
    float getMaxHeight() {return maxHeight;}
    vector<shared_ptr<Texture>> getTerrainTextures() {return terrainTextures;}
    vector<shared_ptr<TextureArray>> getTerrainTextureArrays() {return terrainTextureArrays;}

    void setPlayer(shared_ptr<Player> inPlayer) {player = inPlayer;}
    void setSeaLevel(float inSeaLevel) {seaLevel = inSeaLevel;}
    void setMaxHeight(float inMaxHeight) {maxHeight = inMaxHeight;}
    void setSeed(long inSeed) {seed = inSeed;}
    void setChunks(vector<shared_ptr<Chunk>> inChunks) {chunks = inChunks;}
    void setTerrainTextures(vector<shared_ptr<Texture>> inTerrainTextures) {terrainTextures = inTerrainTextures;}
    void setTerrainTextureArrays(vector<shared_ptr<TextureArray>> inTerrainTextureArrays) {terrainTextureArrays = inTerrainTextureArrays;}

    void addChunk(shared_ptr<Chunk> chunk) {chunks.push_back(chunk);}
    shared_ptr<Chunk> requestNewChunk(vector<int> chunkCoords, Settings settings);
    void setUpInitialChunks(Settings settings);
    vector<int> getPlayersCurrentChunk(shared_ptr<Settings> settings);
    void updateLoadedChunks();
    float distanceToChunkCenter(vector<int> chunkCoords, shared_ptr<Settings> settings);
    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData() override;
};

#endif // WORLD_HPP
