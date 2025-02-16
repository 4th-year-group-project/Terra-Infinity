/**
 * This file contains the class for the terrain object. This object is responsible for containing
 * the renderable terrain mesh for a subchunk.
 *
 */

#include <vector>
#include <memory>
#include <optional>
#include <string>

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


vector<vector<glm::vec3>> Terrain::generateRenderVertices(vector<vector<float>> inHeights, Settings settings){
    // The resolution determines the number of rendered vertices that will be generated between
    // the heightmap vertices of the subchunk. If the resolution is 1 then the subchunk will be
    // rendered with the same number of vertices as the heightmap vertices.
    float heightScalingFactor = settings.getMaximumHeight();
    int numberOfVerticesPerAxis = (size+2) * resolution;
    vector<vector<glm::vec3>> renderVertices = vector<vector<glm::vec3>>(numberOfVerticesPerAxis, vector<glm::vec3>(numberOfVerticesPerAxis));
    // How much each step needs to change in the x and z direction to get the next vertex
    float stepSize = static_cast<float>(size + 2) / static_cast<float>(numberOfVerticesPerAxis);

    // We are going to assume that our chunk has a 1 vertex border around the edge of the chunk
    // resulting in (size+2) x (size+2) values from the heightmap. We only want to generate the

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
                float height = Utility::bilinear_interpolation(
                    glm::vec2(x, z),
                    glm::vec3(x1, inHeights[z1][x1], z1),
                    glm::vec3(x2, inHeights[z1][x2], z1),
                    glm::vec3(x1, inHeights[z2][x1], z2),
                    glm::vec3(x2, inHeights[z2][x2], z2)
                );
                renderVertices[j][i] = glm::vec3(
                    x,
                    height,
                    z
                );
            }
        }
    }
    return renderVertices;
}

glm::vec3 Terrain::computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C){
    return glm::normalize(glm::cross(B - A, C - A));
}

vector<unsigned int> Terrain::generateIndexBuffer(int numberOfVerticesPerAxis){
    // The resolution determines the number of rendered vertices that will be generated between
    // the heightmap vertices of the subchunk. If the resolution is 1 then the subchunk will be
    // rendered with the same number of vertices as the heightmap vertices.
    // float stepSize = static_cast<float>(size+2) / static_cast<float>(resolution);
    vector<unsigned int> indices = vector<unsigned int>((numberOfVerticesPerAxis - 1) * (numberOfVerticesPerAxis - 1) * 6);
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

vector<vector<glm::vec3>> Terrain::generateNormals(vector<vector<glm::vec3>> inVertices, vector<unsigned int> inIndices){
    // Loop through all of the faces and calculate the normal contributions for each face on their
    // vertices. We will then normalise the final contribution
    vector<vector<glm::vec3>> normals = vector<vector<glm::vec3>>(inVertices.size(), vector<glm::vec3>(inVertices[0].size()));

    // Loop all of the faces using the index buffer
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
    for (int z = 0; z < static_cast<int> (normals.size()); z++){
        for (int x = 0; x < static_cast<int> (normals[0].size()); x++){
            normals[z][x] = glm::normalize(normals[z][x]);
        }
    }
    return normals;
}

vector<glm::vec3> Terrain::flatten2DVector(vector<vector<glm::vec3>> inVector){
    // We assume that the 2D vector is a square matrix
    vector<glm::vec3> flattenedVector = vector<glm::vec3>(inVector.size() * inVector[0].size());
    for (int i = 0; i < static_cast<int> (inVector.size()); i++){
        for (int j = 0; j < static_cast<int> (inVector[0].size()); j++){
            flattenedVector[i * inVector[0].size() + j] = inVector[i][j];
        }
    }
    return flattenedVector;
}

vector<vector<vector<glm::vec3>>> Terrain::cropBorderVerticesAndNormals(
    vector<vector<glm::vec3>> inVertices,
    vector<vector<glm::vec3>> inNormals
){
    vector<vector<vector<glm::vec3>>> croppedData = vector<vector<vector<glm::vec3>>>(2);
    // We want to extract the (size x size) centred region of the subchunk. This will remove the
    // 1*resolution wide vertex border around the edge.
    int numberOfVerticesPerAxis = (size + 2) * resolution;
    croppedData[0] = vector<vector<glm::vec3>>(); // Extracted vertex data
    croppedData[1] = vector<vector<glm::vec3>>(); // Extracted normal data

    // We want to iterate through the 2D mesh aonly keep the central (size*resolution x size*resolution)
    //

    for (int z = 0; z < numberOfVerticesPerAxis; z++){
        vector<glm::vec3> vertexRow = vector<glm::vec3>();
        vector<glm::vec3> normalRow = vector<glm::vec3>();
        for (int x = 0; x < numberOfVerticesPerAxis; x++){
            if (
                x >= resolution && x < numberOfVerticesPerAxis - resolution &&
                z >= resolution && z < numberOfVerticesPerAxis - resolution
            ){
                vertexRow.push_back(inVertices[z][x]);
                normalRow.push_back(inNormals[z][x]);
            }
        }
        if (vertexRow.size() > 0){
            croppedData[0].push_back(vertexRow);
            croppedData[1].push_back(normalRow);
        }
    }

    return croppedData;
}

glm::mat4 Terrain::generateTransformMatrix(){
    // We know the world coordinates of the origin of the subchunk so we can use this to generate
    // the transform matrix for the terrain which will transform (0,0) to the world coordinates
    float worldX = worldCoords[0];
    float worldZ = worldCoords[1];
    // cout << "World Coords: " << worldX << ", " << worldZ << endl;
    return glm::translate(glm::mat4(1.0f), glm::vec3(worldX, 0.0f, worldZ));
}

Terrain::Terrain(
    vector<vector<float>> inHeights,
    Settings settings,
    vector<float> inWorldCoords,
    shared_ptr<Shader> inShader
){
    // Use the settings to set the size and resolution of the subchunk terrain
    // cout << "===== Terrain Settings =====" << endl;
    resolution = settings.getSubChunkResolution();
    size = settings.getSubChunkSize();
    worldCoords = inWorldCoords;
    // Generate the vertices, indices and normals for the terrain
    vector<vector<glm::vec3>> renderVertices = generateRenderVertices(inHeights, settings);
    vector<unsigned int> tempIndices = generateIndexBuffer(size + 2);
    vector<vector<glm::vec3>> normals = generateNormals(renderVertices, tempIndices);

    // Crop the border of the terrain out
    vector<vector<vector<glm::vec3>>> croppedData = cropBorderVerticesAndNormals(renderVertices, normals);
    vector<vector<glm::vec3>> croppedVertices = croppedData[0];
    vector<vector<glm::vec3>> croppedNormals = croppedData[1];
    vector<unsigned int> croppedIndices = generateIndexBuffer(size);

    vector<glm::vec3> flattenedVertices = flatten2DVector(croppedVertices);
    vector<glm::vec3> flattenedNormals = flatten2DVector(croppedNormals);
    for (int i = 0; i < static_cast<int> (flattenedVertices.size()); i++){
        vertices.push_back(Vertex(flattenedVertices[i], flattenedNormals[i], glm::vec2(0.0f, 0.0f)));
    }
    indices = croppedIndices;

    shader = inShader;

    // Use the utility function to write the mesh to an obj file
    // string outputPath = getenv("DATA_ROOT");
    // string filename = outputPath + settings.getFilePathDelimitter() + "terrain.obj";
    // Utility::storeHeightmapToObj(filename.c_str(), flattenedVertices, flattenedNormals, indices);

    // Generate the transform matrix for the terrain
    model = generateTransformMatrix();
    normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    // Generate the texture for the terrain
    // TODO: Complete this work for the textures

    setupData();
}

Terrain::~Terrain(){
    // Do nothing
}

void Terrain::render(glm::mat4 view, glm::mat4 projection){
    // Use the shader
    shader->use();
    // Set the model, view and projection matrices
    shader->setMat4("model", model);
    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
    shader->setMat3("normalMatrix", normalMatrix);
    shader->setVec3("colour", glm::vec3(1.0f, 0.5f, 0.31f));
    // Bind the VAO
    glBindVertexArray(VAO);
    // Draw the terrain
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    // Unbind the VAO
    glBindVertexArray(0);
    shader->deactivate();
}

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
}

void Terrain::updateData(){
    // Do nothing
}
