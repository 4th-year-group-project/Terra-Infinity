#ifndef CHUNK_HPP
#define CHUNK_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif


#include <vector>

#include "Settings.hpp"
#include "IRenderable.hpp"
#include "SubChunk.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Light.hpp"

using namespace std;

class SubChunk; // Forward declaration of the SubChunk class

class Chunk: public IRenderable, public enable_shared_from_this<Chunk> {
private:
    long id; // Unique identifier for the chunk
    int size; // The size of the chunk
    int subChunkSize; // The size of the subchunks within the chunk
    int subChunkResolution; // The resolution of the subchunks within the chunk
    shared_ptr<Settings> settings; // The settings for the renderer
    vector<int> chunkCoords; // The chunks coordinates within the global chunk space
    // The vertices are ordered in the following way:
    // vertices[x + z * 1024] = vertex at position x, z
    // This is the heightmap data for the chunk
    vector<vector<float>> heightmapData;
    vector<vector<uint8_t>> biomeData; // The biome data for the chunk
    // Using ids 0-1023 we can have a unique id for each subchunk within the chunk
    vector<shared_ptr<SubChunk>> loadedSubChunks; // Tracks the subchunks that are loaded
    vector<shared_ptr<SubChunk>> cachedSubChunks; // Tracks the subchunks that are cached
    shared_ptr<Shader> terrainShader; // The shader for the terrain object
    shared_ptr<Shader> oceanShader; // The shader for the ocean object
    vector<shared_ptr<Texture>> terrainTextures; // The textures for the terrain
    vector<shared_ptr<TextureArray>> terrainTextureArrays; // The texture arrays for the terrain

public:
    Chunk(
        long inId,
        std::shared_ptr<Settings> settings,
        std::vector<int> inChunkCoords,
        std::vector<std::vector<float>> inHeightmapData,
        std::vector<std::vector<uint8_t>> inBiomeData,
        std::shared_ptr<Shader> inTerrainShader,
        std::shared_ptr<Shader> inOceanShader,
        std::vector<std::shared_ptr<Texture>> inTerrainTextures,
        std::vector<std::shared_ptr<TextureArray>> inTerrainTextureArrays
    );
    ~Chunk();

    long getId() { return id; }
    vector<int> getChunkCoords() { return chunkCoords; }
    vector<vector<float>> getHeightmapData() { return heightmapData; }
    vector<vector<uint8_t>> getBiomeData() { return biomeData; }
    int getSize() { return size; }
    int getSubChunkSize() { return subChunkSize; }
    int getSubChunkResolution() { return subChunkResolution; }
    shared_ptr<Settings> getSettings() { return settings; }
    void setHeightmapData(vector<vector<float>> inHeightmapData) { heightmapData = inHeightmapData; }
    void setBiomeData(vector<vector<uint8_t>> inBiomeData) { biomeData = inBiomeData; }
    void setChunkCoords(vector<int> inChunkCoords) { chunkCoords = inChunkCoords; }
    void setId(long inId) { id = inId; }
    shared_ptr<Shader> getTerrainShader() { return terrainShader; }
    void setTerrainShader(shared_ptr<Shader> inTerrainShader) { terrainShader = inTerrainShader;}

    vector<float> getChunkWorldCoords();
    vector<float> getSubChunkWorldCoords(int id);
    vector<shared_ptr<SubChunk>> getLoadedSubChunks();
    vector<shared_ptr<TextureArray>> getTerrainTextureArrays() { return terrainTextureArrays; }
    vector<shared_ptr<Texture>> getTerrainTextures() { return terrainTextures; }
    void setTerrainTextures(vector<shared_ptr<Texture>> inTerrainTextures) { terrainTextures = inTerrainTextures; }
    void setTerrainTextureArrays(vector<shared_ptr<TextureArray>> inTerrainTextureArrays) { terrainTextureArrays = inTerrainTextureArrays; }


    int getSubChunkId(glm::vec3 position);
    void addSubChunk(int id, float resolution);
    void updateLoadedSubChunks(glm::vec3 playerPos, Settings settings);
    void unloadSubChunk(int id);
    void deleteSubChunk(int id);
    vector<int> checkRenderDistance(glm::vec3 playerPos, Settings settings);
    float getDistanceToChunk(glm::vec3 playerPos);

    // Testing function
    void loadAllSubChunks();

    void render(
        glm::mat4 view,
        glm::mat4 projection,
        vector<shared_ptr<Light>> lights,
        glm::vec3 viewPos
    ) override;
    void setupData() override;
    void updateData() override;
};

#endif // CHUNK_HPP