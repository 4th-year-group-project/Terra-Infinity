#ifndef CHUNK_HPP
#define CHUNK_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif


#include <vector>
#include <unordered_map>

#include "Settings.hpp"
#include "IRenderable.hpp"
#include "SubChunk.hpp"

using namespace std;

class SubChunk; // Forward declaration of the SubChunk class

class Chunk: public IRenderable {
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
    // Using ids 0-1023 we can have a unique id for each subchunk within the chunk
    unordered_map<int, shared_ptr<SubChunk>> loadedSubChunks; // Keeping track of the subchunks within the chunk that are loaded
    unordered_map<int, shared_ptr<SubChunk>> cachedSubChunks; // Keeping track of the subchunks within the chunk that are cached
    shared_ptr<Shader> terrainShader; // The shader for the terrain object

public:
    Chunk(
        long inId,
        shared_ptr<Settings> settings,
        vector<int> inChunkCoords,
        vector<vector<float>> inHeightmapData,
        shared_ptr<Shader> inTerrainShader
    ):
        id(inId),
        size(settings->getChunkSize()),
        subChunkSize(settings->getSubChunkSize()),
        subChunkResolution(settings->getSubChunkResolution()),
        settings(settings),
        chunkCoords(inChunkCoords),
        heightmapData(inHeightmapData),
        terrainShader(inTerrainShader)
    {
        loadedSubChunks = unordered_map<int, shared_ptr<SubChunk>>(); // Initialize the map to be empty
        cachedSubChunks = unordered_map<int, shared_ptr<SubChunk>>(); // Initialize the map to be empty
    }
    ~Chunk();

    long getId() { return id; }
    vector<int> getChunkCoords() { return chunkCoords; }
    vector<vector<float>> getHeightmapData() { return heightmapData; }
    int getSize() { return size; }
    int getSubChunkSize() { return subChunkSize; }
    int getSubChunkResolution() { return subChunkResolution; }
    void setHeightmapData(vector<vector<float>> inHeightmapData) { heightmapData = inHeightmapData; }
    void setChunkCoords(vector<int> inChunkCoords) { chunkCoords = inChunkCoords; }
    void setId(long inId) { id = inId; }
    shared_ptr<Shader> getTerrainShader() { return terrainShader; }
    void setTerrainShader(shared_ptr<Shader> inTerrainShader) { terrainShader = inTerrainShader;}

    vector<float> getChunkWorldCoords();
    vector<float> getSubChunkWorldCoords(int id);
    vector<shared_ptr<SubChunk>> getLoadedSubChunks();

    int getSubChunkId(glm::vec3 position);
    void addSubChunk(int id);
    void updateLoadedSubChunks(glm::vec3 playerPos, Settings settings);
    void unloadSubChunk(int id);
    void deleteSubChunk(int id);
    vector<int> checkRenderDistance(glm::vec3 playerPos, Settings settings);
    float getDistanceToChunk(glm::vec3 playerPos);

    // Testing function
    void loadAllSubChunks();

    void render(glm::mat4 view, glm::mat4 projection) override;
    void setupData() override;
    void updateData() override;
};

#endif // CHUNK_HPP