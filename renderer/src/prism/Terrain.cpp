/**
 * @file Terrain.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Terrain class.
 * @details This class is used to render the terrain in the scene. It will load the heightmap and
 * then generate the terrain mesh from the heightmap. It will also handle the rendering of the
 * terrain.
 * @version 1.0
 * @date 2025
 * 
 */
#include <vector>
#include <memory>
#include <optional>
#include <string>
#include <omp.h>

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
#include "Utility.hpp"
#include "Settings.hpp"
#include "Terrain.hpp"

using namespace std;

/**
 * @brief This function will generate the render vertices for the terrain heightmap values
 * 
 * @details This function will generate the render vertices for the terrain heightmap values. It
 * will scaling the heightmap values by the height scaling factor. If there is no pixel in the
 * heightmap then it will use bicubic interpolation to get the height of the created vertex.
 * 
 * @param inHeights [in] std::vector<std::vector<float>> The heightmap values
 * @param heightScalingFactor [in] float The height scaling factor
 * 
 * @return std::vector<std::vector<glm::vec3>> The render vertices for the terrain
 * 
 */
vector<vector<glm::vec3>> Terrain::generateRenderVertices(
    vector<vector<float>> inHeights,
    float heightScalingFactor
){
    // The resolution determines the number of rendered vertices that will be generated between
    // the heightmap vertices of the subchunk. If the resolution is 1 then the subchunk will be
    // rendered with the same number of vertices as the heightmap vertices.
    int numberOfVerticesPerAxis = (size+2) * resolution;
    vector<vector<glm::vec3>> renderVertices = vector<vector<glm::vec3>>(numberOfVerticesPerAxis, vector<glm::vec3>(numberOfVerticesPerAxis));
    // How much each step needs to change in the x and z direction to get the next vertex
    float stepSize = static_cast<float>(size + 2) / static_cast<float>(numberOfVerticesPerAxis);

    // We are going to assume that our chunk has a 1 vertex border around the edge of the chunk
    // resulting in (size+2) x (size+2) values from the heightmap. We only want to generate the
    #pragma omp parallel for
    for (int i = 0; i < numberOfVerticesPerAxis; i++){
        for (int j = 0; j < numberOfVerticesPerAxis; j++){
            // We need to calculate the position of the vertex in the heightmap that we are going to
            // interpolate from
            float x = i * stepSize;
            float z = j * stepSize;
            // We need to calculate the position of the vertex in the heightmap that we are going to
            // interpolate from
            int x1 = static_cast<int>(x);
            int x2 = static_cast<int>(x + 1);
            int z1 = static_cast<int>(z);
            int z2 = static_cast<int>(z + 1);
            // We can check if the vertex will be on a whole vertex and then not need to complete
            // the interpolation
            if ((x2 >= size+2 || z2 >= size+2) || (x1 == x && z1 == z && x2 == x+1 && z2 == z+1)){
                renderVertices[j][i] = glm::vec3(
                    x,
                    Utility::height_scaling(inHeights[z1][x1], heightScalingFactor),
                    z
                );
            } else {
                // We need to interpolate the height of the vertex from the heightmap
                float height = Utility::bicubic_interpolation(
                    glm::vec2(x, z),
                    inHeights
                );
                renderVertices[j][i] = glm::vec3(
                    x,
                    Utility::height_scaling(height, heightScalingFactor),
                    z
                );
            }
        }
    }
    return renderVertices;
}

/**
 * @brief This function will compute the normal contribution for a triangle
 * 
 * @details This function will compute the normal contribution for a triangle. It will take the
 * three vertices of the triangle and calculate the normal contribution using the cross product
 * of the two edges of the triangle.
 * 
 * @param A [in] glm::vec3 The first vertex of the triangle
 * @param B [in] glm::vec3 The second vertex of the triangle
 * @param C [in] glm::vec3 The third vertex of the triangle
 * 
 * @return glm::vec3 The normal contribution for the triangle
 * 
 */
glm::vec3 Terrain::computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C){
    return glm::normalize(glm::cross(B - A, C - A));
}

/**
 * @brief This function will generate the index buffer for the terrain
 * 
 * @details This function will generate the index buffer for the terrain. It will create a
 * triangle strip for each row of vertices in the heightmap. The index buffer will be used to
 * render the terrain.
 * 
 * @param numberOfVerticesPerAxis [in] int The number of vertices per axis
 * 
 * @return std::vector<unsigned int> The index buffer for the terrain
 * 
 */
vector<unsigned int> Terrain::generateIndexBuffer(int numberOfVerticesPerAxis){
    // The resolution determines the number of rendered vertices that will be generated between
    // the heightmap vertices of the subchunk. If the resolution is 1 then the subchunk will be
    // rendered with the same number of vertices as the heightmap vertices.
    // float stepSize = static_cast<float>(size+2) / static_cast<float>(resolution);
    vector<unsigned int> indices = vector<unsigned int>((numberOfVerticesPerAxis - 1) * (numberOfVerticesPerAxis - 1) * 6);

    #pragma omp parallel for
    for (int x = 0; x < numberOfVerticesPerAxis - 1; x++){
        for (int z = 0; z < numberOfVerticesPerAxis - 1; z++){
            // First triangle of the form [x,z], [x+1, z], [x+1, z+1]
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6)] = x * numberOfVerticesPerAxis + z;
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6) + 1] = (x + 1) * numberOfVerticesPerAxis + z;
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6) + 2] = (x + 1) * numberOfVerticesPerAxis + z + 1;
            // Second triangle of the form [x,z], [x+1, z+1], [x, z+1]
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6) + 3] = x * numberOfVerticesPerAxis + z;
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6) + 4] = (x + 1) * numberOfVerticesPerAxis + z + 1;
            indices[(x * (numberOfVerticesPerAxis - 1) * 6 + z * 6) + 5] = x * numberOfVerticesPerAxis + z + 1;
        }
    }
    return indices;
}

/**
 * @brief This function will generate the normals for the terrain
 * 
 * @details This function will generate the normals for the terrain. It will loop through all of
 * the faces and calculate the normal contributions for each face on their vertices. We will then
 * normalise the final contribution.
 * 
 * @param inVertices [in] std::vector<std::vector<glm::vec3>> The vertices of the terrain
 * @param inIndices [in] std::vector<unsigned int> The index buffer for the terrain
 * 
 * @return std::vector<std::vector<glm::vec3>> The normals for the terrain
 * 
 */
vector<vector<glm::vec3>> Terrain::generateNormals(vector<vector<glm::vec3>> inVertices, vector<unsigned int> inIndices){
    // Loop through all of the faces and calculate the normal contributions for each face on their
    // vertices. We will then normalise the final contribution
    vector<vector<glm::vec3>> normals = vector<vector<glm::vec3>>(inVertices.size(), vector<glm::vec3>(inVertices[0].size()));

    // Loop all of the faces using the index buffer
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int> (inIndices.size()); i += 3){
        // Get the three vertices of the triangle
        unsigned int indexA = inIndices[i];
        unsigned int indexB = inIndices[i+1];
        unsigned int indexC = inIndices[i+2];
        // Convert the indices to the 2d array indices
        unsigned int xA = indexA % inVertices[0].size();
        unsigned int zA = indexA / inVertices[0].size();
        unsigned int xB = indexB % inVertices[0].size();
        unsigned int zB = indexB / inVertices[0].size();
        unsigned int xC = indexC % inVertices[0].size();
        unsigned int zC = indexC / inVertices[0].size();
        glm::vec3 A = inVertices[zA][xA];
        glm::vec3 B = inVertices[zB][xB];
        glm::vec3 C = inVertices[zC][xC];
        // Calculate the normal contribution for the face
        glm::vec3 normal = computeNormalContribution(A, B, C);
        // Add the normal contribution to the vertices
        normals[zA][xA] += normal;
        normals[zB][xB] += normal;
        normals[zC][xC] += normal;
    }

    // Normalise the contributions
    #pragma omp parallel for
    for (int z = 0; z < static_cast<int> (normals.size()); z++){
        for (int x = 0; x < static_cast<int> (normals[0].size()); x++){
            normals[z][x] = glm::normalize(normals[z][x]);
        }
    }
    return normals;
}

/**
 * @brief This function will flatten a 2D vector into a 1D vector
 * 
 * @details This function will flatten a 2D vector into a 1D vector. It will iterate through the
 * 2D vector and add each element to the 1D vector.
 * 
 * @param inVector [in] std::vector<std::vector<glm::vec3>> The 2D vector to flatten
 * 
 * @return std::vector<glm::vec3> The flattened 1D vector
 * 
 */
vector<glm::vec3> Terrain::flatten2DVector(vector<vector<glm::vec3>> inVector){
    // We assume that the 2D vector is a square matrix
    vector<glm::vec3> flattenedVector = vector<glm::vec3>(inVector.size() * inVector[0].size());
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int> (inVector.size()); i++){
        for (int j = 0; j < static_cast<int> (inVector[0].size()); j++){
            flattenedVector[i * inVector[0].size() + j] = inVector[i][j];
        }
    }
    return flattenedVector;
}

/**
 * @brief This function will crop the border vertices and normals from the terrain
 * 
 * @details This function will crop the border vertices and normals from the terrain. It will
 * remove the 1*resolution wide vertex border around the edge of the subchunk.
 * 
 * @param inVertices [in] std::vector<std::vector<glm::vec3>> The vertices of the terrain
 * @param inNormals [in] std::vector<std::vector<glm::vec3>> The normals of the terrain
 * 
 * @return std::vector<std::vector<std::vector<glm::vec3>>> The cropped vertices and normals
 * 
 */
vector<vector<vector<glm::vec3>>> Terrain::cropBorderVerticesAndNormals(
    vector<vector<glm::vec3>> inVertices,
    vector<vector<glm::vec3>> inNormals
){
    vector<vector<vector<glm::vec3>>> croppedData = vector<vector<vector<glm::vec3>>>(2);
    // We want to extract the (size x size) centred region of the subchunk. This will remove the
    // 1*resolution wide vertex border around the edge.
    int numberOfVerticesPerAxis = (size + 2) * resolution;
    croppedData[0] = vector<vector<glm::vec3>>((size - 1) * resolution + 1, vector<glm::vec3>((size - 1) * resolution + 1));
    croppedData[1] = vector<vector<glm::vec3>>((size - 1) * resolution + 1, vector<glm::vec3>((size - 1) * resolution + 1));
    // We want to iterate through the 2D mesh aonly keep the central (size*resolution x size*resolution)
    //
    float stepSize = static_cast<float>(size + 2) / static_cast<float>(numberOfVerticesPerAxis);
    #pragma omp parallel for
    for (int z = 0; z < numberOfVerticesPerAxis; z++){
        for (int x = 0; x < numberOfVerticesPerAxis; x++){
            if (
                x >= resolution && x < numberOfVerticesPerAxis - (2 * resolution) + 1 &&
                z >= resolution && z < numberOfVerticesPerAxis - (2 * resolution) + 1
            ){
                croppedData[0][z - resolution][x - resolution] = glm::vec3(
                    (x - resolution) * stepSize,
                    inVertices[z][x].y,
                    (z - resolution) * stepSize
                );
                croppedData[1][z - resolution][x - resolution] = inNormals[z][x];
            }
        }
    }
    return croppedData;
}

/**
 * @brief This function will generate the transform matrix for the terrain
 * 
 * @details This function will generate the transform matrix for the terrain. It will use the
 * world coordinates of the subchunk to generate the transform matrix.
 * 
 * @return glm::mat4 The transform matrix for the terrain
 */
glm::mat4 Terrain::generateTransformMatrix(){
    // We know the world coordinates of the origin of the subchunk so we can use this to generate
    // the transform matrix for the terrain which will transform (0,0) to the world coordinates
    float worldX = worldCoords[0];
    float worldZ = worldCoords[1];
    // if (worldX == 0 && worldZ == 0){
    //     return glm::mat4(1.0f);
    // }
    return glm::translate(glm::mat4(1.0f), glm::vec3(worldX, 0.0f, worldZ));
}

/**
 * @brief This function will create the mesh for the terrain
 * 
 * @details This function will create the mesh for the terrain. It will generate the vertices,
 * indices and normals for the terrain. It will then crop the border of the terrain out and
 * flatten the vertices and normals into a 1D vector. It will then create the size of the vertices
 * array and use the utility function to write the mesh to an obj file.
 * 
 * @param inHeights [in] std::vector<std::vector<float>> The heightmap values
 * @param heightScalingFactor [in] float The height scaling factor
 * 
 * @return void
 * 
 */
void Terrain::createMesh(vector<vector<float>> inHeights, float heightScalingFactor){
    // Generate the vertices, indices and normals for the terrain
    vector<vector<glm::vec3>> renderVertices = generateRenderVertices(inHeights, heightScalingFactor);
    vector<unsigned int> tempIndices = generateIndexBuffer((size + 2) * resolution);
    vector<vector<glm::vec3>> normals = generateNormals(renderVertices, tempIndices);

    // Crop the border of the terrain out
    vector<vector<vector<glm::vec3>>> croppedData = cropBorderVerticesAndNormals(renderVertices, normals);
    vector<vector<glm::vec3>> croppedVertices = croppedData[0];
    vector<vector<glm::vec3>> croppedNormals = croppedData[1];
    vector<unsigned int> croppedIndices;
    if (resolution == 1){
        croppedIndices = generateIndexBuffer(size);
    } else {
        croppedIndices = generateIndexBuffer(size * resolution - resolution + 1);
    }
    vector<glm::vec3> flattenedVertices = flatten2DVector(croppedVertices);
    vector<glm::vec3> flattenedNormals = flatten2DVector(croppedNormals);

    // Create the size of the vertices array
    vertices = vector<Vertex>(flattenedVertices.size());
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int> (flattenedVertices.size()); i++){
        vertices[i] = Vertex(flattenedVertices[i], flattenedNormals[i], glm::vec2(0.0f, 0.0f));
        // vertices.push_back(Vertex(flattenedVertices[i], flattenedNormals[i], glm::vec2(0.0f, 0.0f)));
    }
    indices = croppedIndices;

    // Use the utility function to write the mesh to an obj file
    // string outputPath = getenv("DATA_ROOT");
    // string filename = outputPath + settings.getFilePathDelimitter() + "terrain.obj";
    // Utility::storeHeightmapToObj(filename.c_str(), flattenedVertices, flattenedNormals, indices);
}

/**
 * @brief Construct a new Terrain object with the given arguments
 * 
 * @param inHeights [in] std::vector<std::vector<float>> The heightmap values
 * @param inBiomes [in] std::shared_ptr<std::vector<std::vector<uint8_t>>> The biomes of the terrain
 * @param inSettings [in] std::shared_ptr<Settings> The settings object
 * @param inWorldCoords [in] std::vector<float> The world coordinates of the subchunk
 * @param inShader [in] std::shared_ptr<Shader> The shader for the terrain
 * @param inTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the terrain
 * @param inTextureArrays [in] std::vector<std::shared_ptr<TextureArray>> The texture arrays for the terrain
 * @param inSubbiomeTextureArrayMap [in] const int* The subbiome texture array map
 * 
 */
Terrain::Terrain(
    vector<vector<float>> inHeights,
    shared_ptr<vector<vector<uint8_t>>> inBiomes,
    shared_ptr<Settings> inSettings,
    vector<float> inWorldCoords,
    shared_ptr<Shader> inShader,
    vector<shared_ptr<Texture>> inTextures,
    vector<shared_ptr<TextureArray>> inTextureArrays,
    const int* inSubbiomeTextureArrayMap
){
    // Use the settings to set the size and resolution of the subchunk terrain
    settings = inSettings;
    resolution = settings->getSubChunkResolution();
    size = settings->getSubChunkSize();
    worldCoords = inWorldCoords;

    // Setting the subchunk biome data
    biomes = inBiomes;

    createMesh(inHeights, settings->getMaximumHeight());

    shader = inShader;
    textures = inTextures;
    textureArrays = inTextureArrays;
    subbiomeTextureArrayMap = inSubbiomeTextureArrayMap;

    // Generate the transform matrix for the terrain
    model = generateTransformMatrix();
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    setupData();
}

/**
 * @brief Construct a new Terrain object with the given arguments
 * 
 * @param inHeights [in] std::vector<std::vector<float>> The heightmap values
 * @param inBiomes [in] std::shared_ptr<std::vector<std::vector<uint8_t>>> The biomes of the terrain
 * @param inResolution [in] float The resolution of the subchunk
 * @param inSettings [in] std::shared_ptr<Settings> The settings object
 * @param inWorldCoords [in] std::vector<float> The world coordinates of the subchunk
 * @param inShader [in] std::shared_ptr<Shader> The shader for the terrain
 * @param inTextures [in] std::vector<std::shared_ptr<Texture>> The textures for the terrain
 * @param inTextureArrays [in] std::vector<std::shared_ptr<TextureArray>> The texture arrays for the terrain
 * @param inSubbiomeTextureArrayMap [in] const int* The subbiome texture array map
 * 
 */
Terrain::Terrain(
    vector<vector<float>> inHeights,
    shared_ptr<vector<vector<uint8_t>>> inBiomes,
    float inResolution,
    shared_ptr<Settings> inSettings,
    vector<float> inWorldCoords,
    shared_ptr<Shader> inShader,
    vector<shared_ptr<Texture>> inTextures,
    vector<shared_ptr<TextureArray>> inTextureArrays,
    const int* inSubbiomeTextureArrayMap
){
    // Use the settings to set the size and resolution of the subchunk terrain
    settings = inSettings;
    resolution = inResolution;
    size = settings->getSubChunkSize();
    worldCoords = inWorldCoords;

    // Setting the subchunk biome data
    biomes = inBiomes;


    createMesh(inHeights, settings->getMaximumHeight());

    shader = inShader;
    textures = inTextures;
    textureArrays = inTextureArrays;
    subbiomeTextureArrayMap = inSubbiomeTextureArrayMap;

    // Generate the transform matrix for the terrain
    model = generateTransformMatrix();
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
    setupData();
}

/**
 * @brief Destroy the Terrain object by the standard destructor
 * 
 */
Terrain::~Terrain(){
    // Do nothing
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
/**
 * @brief Renders the terrain in the scene
 * 
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] std::vector<std::shared_ptr<Light>> The lights in the scene
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether or not this is a water pass
 * @param isShadowPass [in] bool Whether or not this is a shadow pass
 * @param plane [in] glm::vec4 The clipping plane
 * 
 */
void Terrain::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
){
    // Use the shader
    shader->use();
    // Set the model, view and projection matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);
    shader->setVec3("colour", glm::vec3(1.0f, 0.5f, 0.31f));
    shader->setVec2("chunkOrigin", glm::vec2(worldCoords[0], worldCoords[1]));

    // Set the light properties
    shared_ptr<Light> sun = lights[0];
    shader->setVec3("viewPos", viewPos);
    // shader->setVec3("light.position", sun->getPosition());
    shader->setVec3("light.position", glm::vec3(-0.2f, -1.0f, -0.3f));
    shader->setVec3("light.ambient", sun->getAmbient() * sun->getColour());
    shader->setVec3("light.diffuse", sun->getDiffuse() * sun->getColour());
    shader->setVec3("light.specular", sun->getSpecular());

    // Set the material properties
    shader->setVec3("material.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
    shader->setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 0.81f));
    shader->setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader->setFloat("material.shininess", 2.0f);

    // Setting up the terrain parameters
    shader->setFloat("terrainParams.minMidGroundHeight", 0.2f * settings->getMaximumHeight());
    shader->setFloat("terrainParams.maxLowGroundHeight", 0.26f * settings->getMaximumHeight());
    shader->setFloat("terrainParams.minHighGroundHeight", 0.56f * settings->getMaximumHeight());
    shader->setFloat("terrainParams.maxMidGroundHeight", 0.86f * settings->getMaximumHeight());
    shader->setFloat("terrainParams.minFlatSlope", 0.8f);
    shader->setFloat("terrainParams.maxSteepSlope", 0.9f);
    shader->setFloat("terrainParams.seaLevelHeight", settings->getSeaLevel() * settings->getMaximumHeight());

    // Setting the fog parameters
    shader->setFloat("fogParams.fogStart", settings->getFogStart());
    shader->setFloat("fogParams.fogEnd", settings->getFogEnd());
    shader->setFloat("fogParams.fogDensity", settings->getFogDensity());
    shader->setVec3("fogParams.fogColour", settings->getFogColor());
    
    // Set the clipping plane
    shader->setVec4("clippingPlane", plane);

    // Set the subbiome to texture array index map
    shader->setIntArray("subbiomeTextureArrayMap", subbiomeTextureArrayMap, 34);

    // Bind the biome map for this subchunk
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, biomeTextureID);
    shader->setInt("biomeMap", 0); 

    // We need to iterate through the list of textures and set their uniforms in order
    for (int i = 0; i < static_cast<int> (textures.size()); i++){
        shader->setInt(textures[i]->getName(), i + 1);
    }
    
    // We need to iterate through the list of texture arrays and set their uniforms in order
    for (int i = 0; i < static_cast<int> (textureArrays.size()); i++){
        shader->setInt(textureArrays[i]->getName(), i + 1 + textures.size()); 
    }

    // Bind the VAO
    glBindVertexArray(VAO);
    // Draw the terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();

    // Unbind the biome map
    glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
    glBindTexture(GL_TEXTURE_2D, 0); // Unbind from that unit
}
#pragma GCC diagnostic pop


/**
 * @brief This function will set up the data for the terrain
 * 
 * @details This function will set up the data for the terrain. This includes setting up the
 * vertex array object, vertex buffer object, and element buffer object. It will also set up the
 * vertex attribute pointers for the position, normal, and texture coordinates.
 * 
 * @return void
 * 
 */
void Terrain::setupData(){
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Bind the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    // Bind the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    // Set the vertex attribute pointers
    // Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    // Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);
    // Texture coordinates
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));
    glEnableVertexAttribArray(2);

    // Unbind the VAO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    
    // We need to create a 2D texture for the biome map. 
    int height = biomes->size() - 2;      
    int width  = (*biomes)[0].size() - 2;     

    // We need to flatten the 2D vector of biomes into a 1D vector
    std::vector<uint8_t> flatBiomeData;
    flatBiomeData.reserve(width * height);

    for (int z = 1; z < 33; ++z) {
        for (int x = 1; x < 33; ++x) {
            flatBiomeData.push_back((*biomes)[z][x]);
        }
    }

    glGenTextures(1, &biomeTextureID);
    glBindTexture(GL_TEXTURE_2D, biomeTextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8UI, width, height, 0, GL_RED_INTEGER, GL_UNSIGNED_BYTE, flatBiomeData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

/**
 * @brief This function will update the data for the terrain
 * 
 * @details This function will update the data for the terrain it will do nothing for now as 
 * the terrain is static.
 * 
 * @param regenerate [in] bool Whether to regenerate the data or not
 * 
 * @return void
 * 
 */
void Terrain::updateData(bool){
    // Do nothing
}
