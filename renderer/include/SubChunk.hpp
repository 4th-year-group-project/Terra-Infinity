#ifndef SUBCHUNK_HPP
#define SUBCHUNK_HPP


#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include <vector>
#include <memory>

#include "Terrain.hpp"
#include "Ocean.hpp"
#include "IRenderable.hpp"
#include "Shader.hpp"
#include "Settings.hpp"
#include "Chunk.hpp"
#include "Texture.hpp"
#include "Light.hpp"
#include "WaterFrameBuffer.hpp"


using namespace std;

class Chunk; // Forward declaration of the Chunk class

class SubChunk: public IRenderable {
private:
    int id; // Unique identifier for the subchunk within the chunk
    int size; // The size of the subchunk
    float resolution; // The resolution of the subchunk where 1 is the same resolution as the heightmap
    shared_ptr<Chunk> parentChunk; // The parent chunk of the subchunk
    vector<int> subChunkCoords; // The subchunks coordinates within the chunk space
    vector<vector<float>> heights; // The heightmap data for the subchunk
    vector<vector<uint8_t>> biomes; // The biome data for the subchunk
    shared_ptr<Terrain> terrain; // The terrain object for the subchunk
    shared_ptr<Shader> terrainShader; // The shader for the terrain object
    shared_ptr<Ocean> ocean; // The ocean object for the subchunk
    shared_ptr<Shader> oceanShader; // The shader for the ocean object
    vector<shared_ptr<Texture>> terrainTextures; // The textures for the terrain object
    shared_ptr<WaterFrameBuffer> reflectionBuffer; // The framebuffer that will be used for the reflection
    shared_ptr<WaterFrameBuffer> refractionBuffer; // The framebuffer that will be used for the refraction
    vector<shared_ptr<Texture>> oceanTextures; // The textures for the ocean object

public:
    SubChunk(
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
    );
    SubChunk(
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
    );
    ~SubChunk();

    int getId() { return id; }
    vector<int> getSubChunkCoords() { return subChunkCoords; }
    vector<vector<float>> getHeights() { return heights; }
    vector<vector<uint8_t>> getBiomes() { return biomes; }
    float getResolution() { return resolution; }
    shared_ptr<Chunk> getParentChunk() { return parentChunk; }
    void setSubChunkCoords(vector<int> inSubChunkCoords) { subChunkCoords = inSubChunkCoords; }
    void setId(int inId) { id = inId; }

    vector<float> getSubChunkWorldCoords(shared_ptr<Settings> settings);

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

#endif