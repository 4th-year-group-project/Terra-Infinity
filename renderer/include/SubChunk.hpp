#ifndef SUBCHUNK_HPP
#define SUBCHUNK_HPP


#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include <vector>
#include <memory>

#include "Chunk.hpp"



using namespace std;

class SubChunk{
private:
    int id; // Unique identifier for the subchunk within the chunk
    int size; // The size of the subchunk
    float resolution; // The resolution of the subchunk where 1 is the same resolution as the heightmap
    shared_ptr<Chunk> parentChunk; // The parent chunk of the subchunk
    vector<int> subChunkCoords; // The subchunks coordinates within the chunk space
    vector<glm::vec3> vertices; // The world vertices of the subchunk within the chunk

    vector<glm::vec3> renderVertices; // The vertices of the subchunk that will be rendered
    vector<glm::vec3> renderNormals; // The normals of the subchunk that will be rendered

    vector<int> indicies; // The indices that will form the index buffer for the subchunk

    void generateRenderVertices();
    void generateRenderNormals();
    void generateIndexBuffer();
    glm::vec3 computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C);
    void cropToRemoveBorder();

public:
    SubChunk(
        int inId,
        int inSize,
        float inResolution,
        shared_ptr<Chunk> inParentChunk,
        vector<int> inSubChunkCoords,
        vector<glm::vec3> inVertices
    ): id(inId), size(inSize), resolution(inResolution), subChunkCoords(inSubChunkCoords), vertices(inVertices), parentChunk(move(inParentChunk)) {
        parentChunk = move(inParentChunk);
        renderVertices = vector<glm::vec3>();
        renderNormals = vector<glm::vec3>();
        indicies = vector<int>();
    }
    ~SubChunk();

    int getId() { return id; }
    vector<int> getSubChunkCoords() { return subChunkCoords; }
    vector<glm::vec3> getVertices() { return vertices; }
    shared_ptr<Chunk> getParentChunk() { return parentChunk; }
    vector<glm::vec3> getRenderVertices() { return renderVertices; }
    vector<glm::vec3> getRenderNormals() { return renderNormals; }
    vector<int> getIndicies() { return indicies; }
    void setVertices(vector<glm::vec3> inVertices) { vertices = inVertices; }
    void setSubChunkCoords(vector<int> inSubChunkCoords) { subChunkCoords = inSubChunkCoords; }
    void setId(int inId) { id = inId; }

    vector<float> getSubChunkWorldCoords();

    void generateRenderData();

};

#endif