/**
 * @file Ocean.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Ocean class.
 * @version 1.0
 * @date 2025
 *
 */

#ifndef OCEAN_HPP
#define OCEAN_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Object.hpp"
#include "IRenderable.hpp"
#include "Vertex.hpp"
#include "Settings.hpp"
#include "Shader.hpp"
#include "WaterFrameBuffer.hpp"

using namespace std;

/**
 * @brief This class is used to create an ocean object which contains the information required to renderer a specific
 * ocean quad within the scene. It is a subclass of the Object class and implements the IRenderable interface.
 *
 */
class Ocean : public Object, public IRenderable{
private:
    int size; // The size of the ocean chunk
    float seaLevel; // The sea level of the ocean as a fraction of the maximum height
    shared_ptr<Settings> settings; // The settings for the renderer
    vector<float> oceanQuadOrigin; // The origin of the ocean quad in world space
    vector<Vertex> vertices; // The vertices of the ocean quad
    vector<unsigned int> indices; // The indices of the ocean quad
    vector<float> worldCoords; // The world coordinates of the ocean quad
    shared_ptr<WaterFrameBuffer> reflectionBuffer; // The framebuffer that will be used for the reflection
    shared_ptr<WaterFrameBuffer> refractionBuffer; // The framebuffer that will be used for the refraction
    vector<shared_ptr<Texture>> oceanTextures; // The textures for the ocean object

    float waveSpeed; // The speed of the waves per second
    float currentTime; // The current time of the ocean
    float previousTime; // The previous time of the ocean
    float moveFactor;

public:
    Ocean(
        vector<float> inOceanQuadOrigin,
        vector<float> inWorldCoords,
        shared_ptr<Settings> inSettings,
        shared_ptr<Shader> inShader,
        shared_ptr<WaterFrameBuffer> inReflectionBuffer,
        shared_ptr<WaterFrameBuffer> inRefractionBuffer,
        vector<shared_ptr<Texture>> inOceanTextures
    );
    ~Ocean(){};

    int getSize(){return size;}
    float getSeaLevel(){return seaLevel;}
    vector<float> getOceanQuadOrigin(){return oceanQuadOrigin;}
    vector<Vertex> getVertices(){return vertices;}
    vector<unsigned int> getIndices(){return indices;}

    void setSize(int inSize){size = inSize;}
    void setSeaLevel(float inSeaLevel){seaLevel = inSeaLevel;}
    void setOceanQuadOrigin(vector<float> inOceanQuadOrigin){oceanQuadOrigin = inOceanQuadOrigin;}
    void setVertices(vector<Vertex> inVertices){vertices = inVertices;}
    void setIndices(vector<unsigned int> inIndices){indices = inIndices;}
    void addVertex(Vertex inVertex){vertices.push_back(inVertex);}
    void addIndex(unsigned int inIndex){indices.push_back(inIndex);}

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

#endif // OCEAN_HPP
