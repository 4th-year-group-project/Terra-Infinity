/**
 * @file Terrain.hpp
 * @author King Attalus II
 * @brief This file contains the Terrain class, which is used to represent the subchunk of terrain within a chunk.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef TERRAIN_HPP
#define TERRAIN_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
#else
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glad/glad.h>
#endif

#include "IRenderable.hpp"
#include "Object.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"
#include "Settings.hpp"
#include "TextureArray.hpp"

using namespace std;

class Player; // Forward declaration of the Player class

/**
 * @brief This class represents the terrain within a subchunk of a chunk. It is responsible for rendering the terrain
 * and its associated textures.
 *
 * @details The Terrain class is a subclass of the IRenderable interface and contains the information required to render
 * a specific subchunk of terrain within a chunk. It also contains the information required to render the textures
 * associated with the terrain.
 *
 */
class Terrain : public Object, public IRenderable{
private:
    vector<Vertex> vertices; // The vertices of the terrain
    vector<unsigned int> indices; // The indices of the terrain
    shared_ptr<vector<vector<uint8_t>>> biomes; // The biomes of the subchunk
    float resolution; // The resolution of the terrain
    int size;  // The number of vertices per axis in the heightmap data
    vector<float> worldCoords; // The world coordinates of origin of the terrain subchunk
    GLuint biomeTextureID; // The texture ID of the biome map texture
    vector<shared_ptr<Texture>> textures; // The textures for the terrain
    vector<shared_ptr<TextureArray>> textureArrays; // The texture arrays for the terrain
    shared_ptr<Settings> settings; // The settings for the terrain
    const int* subbiomeTextureArrayMap; // The texture array map for the subbiomes

    glm::vec3 computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C);
    void createMesh(vector<vector<float>> inHeights, float heightScalingFactor);
    vector<vector<glm::vec3>> generateRenderVertices(vector<vector<float>> inHeights, float heightScalingFactor);
    vector<unsigned int> generateIndexBuffer(int numberOfVerticesPerAxis);
    vector<vector<glm::vec3>> generateNormals(vector<vector<glm::vec3>> inVertices, vector<unsigned int> indicies);
    vector<vector<vector<glm::vec3>>> cropBorderVerticesAndNormals(
        vector<vector<glm::vec3>> inVertices,
        vector<vector<glm::vec3>> inNormals
    );
    vector<glm::vec3> flatten2DVector(vector<vector<glm::vec3>> inVector);
    glm::mat4 generateTransformMatrix();
public:
    Terrain(
        vector<vector<float>> inHeights,
        shared_ptr<vector<vector<uint8_t>>> inBiomes,
        shared_ptr<Settings> inSettings,
        vector<float> inWorldCoords,
        shared_ptr<Shader> inShader,
        vector<shared_ptr<Texture>> inTextures,
        vector<shared_ptr<TextureArray>> inTextureArrays,
        const int* subbiomeTextureArrayMap
    );
    Terrain(
        vector<vector<float>> inHeights,
        shared_ptr<vector<vector<uint8_t>>> inBiomes,
        float inResolution,
        shared_ptr<Settings> inSettings,
        vector<float> inWorldCoords,
        shared_ptr<Shader> inShader,
        vector<shared_ptr<Texture>> inTextures,
        vector<shared_ptr<TextureArray>> inTextureArrays,
        const int* subbiomeTextureArrayMap
    );
    ~Terrain();

    shared_ptr<Vertex> getVertex(int index){return make_shared<Vertex>(vertices[index]);}

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
    void updateData(bool regenerate, int frame_counter) override;
};

#endif // TERRAIN_HPP
