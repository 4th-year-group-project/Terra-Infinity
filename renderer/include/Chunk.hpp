#ifndef CHUNK_HPP
#define CHUNK_HPP

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif


#include <vector>
#include <unordered_map>

#include "SubChunk.hpp"
#include "Settings.hpp"

using namespace std;

class Chunk {
private:
    long id; // Unique identifier for the chunk
    int size; // The size of the chunk
    int subChunkSize; // The size of the subchunks within the chunk
    int subChunkResolution; // The resolution of the subchunks within the chunk
    vector<int> chunkCoords; // The chunks coordinates within the global chunk space
    // The vertices are ordered in the following way:
    // vertices[x + z * 1024] = vertex at position x, z
    // This is the heightmap data for the chunk
    vector<glm::vec3> vertices; // The vertices of the chunk starting from local space (x=0, z=0)
    // Using ids 0-1023 we can have a unique id for each subchunk within the chunk
    unordered_map<int, shared_ptr<SubChunk>> loadedSubChunks; // Keeping track of the subchunks within the chunk that are loaded
    unordered_map<int, shared_ptr<SubChunk>> cachedSubChunks; // Keeping track of the subchunks within the chunk that are cached

public:
    Chunk(
        long inId,
        int inSize,
        int inSubChunkSize,
        int inSubChunkRes,
        vector<int> inChunkCoords,
        vector<glm::vec3> inVertices
    ): id(inId), size(inSize), subChunkSize(inSubChunkSize), subChunkResolution(inSubChunkRes), chunkCoords(inChunkCoords), vertices(inVertices) {
        loadedSubChunks = unordered_map<int, shared_ptr<SubChunk>>(); // Initialize the map to be empty
        cachedSubChunks = unordered_map<int, shared_ptr<SubChunk>>(); // Initialize the map to be empty
    }
    ~Chunk() {};

    long getId() { return id; }
    vector<int> getChunkCoords() { return chunkCoords; }
    vector<glm::vec3> getVertices() { return vertices; }
    int getSize() { return size; }
    int getSubChunkSize() { return subChunkSize; }
    int getSubChunkResolution() { return subChunkResolution; }
    void setVertices(vector<glm::vec3> inVertices) { vertices = inVertices; }
    void setChunkCoords(vector<int> inChunkCoords) { chunkCoords = inChunkCoords; }
    void setId(long inId) { id = inId; }

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


};

#endif // CHUNK_HPP