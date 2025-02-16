/**
 * This file contains the class for the terrain object. This object is responsible for containing
 * the renderable terrain mesh for a subchunk.
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

using namespace std;

class Terrain : public Object, public IRenderable{
private:
    vector<Vertex> vertices; // The vertices of the terrain
    vector<unsigned int> indices; // The indices of the terrain
    int resolution; // The resolution of the terrain
    int size;  // The number of vertices per axis in the heightmap data
    vector<float> worldCoords; // The world coordinates of origin of the terrain subchunk

    glm::vec3 computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C);
    vector<vector<glm::vec3>> generateRenderVertices(vector<vector<float>> inHeights, Settings settings);
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
        Settings settings,
        vector<float> inWorldCoords,
        shared_ptr<Shader> inShader
    );
    ~Terrain();


    void render(glm::mat4 view, glm::mat4 projection) override;
    void setupData() override;
    void updateData() override;
};

#endif // TERRAIN_HPP
