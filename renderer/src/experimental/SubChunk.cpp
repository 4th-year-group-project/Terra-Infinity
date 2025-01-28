/*
    Each large "super chunk" will be split up into 32x32 subchunks which will be loaded and unloaded
    dynamically by the renderer based on the player's position in the world and their view distance.

    This will allow us to generate and render subchunks at high resolutions than previously as we
    will only need to generate the subchunks that are within the player's view distance.
*/

#include <SubChunk.hpp>
#include <Utility.hpp>

/*
    This method will generate the render vertices for the subchunk based on the resolution
    of the subchunk. The resolution will determine how many vertices are generated for the
    subchunk.
*/
void SubChunk::generateRenderVertices()
{
    // The resolution determines the number of rendered vertices that will be generated between
    // the heightmap vertices of the subchunk. If the resolution is 1 then the subchunk will be
    // rendered with the same number of vertices as the heightmap vertices.

    //TODO: Consider implementing this as a variable which can be controlled within settings
    float heightScalingFactor = 192.0f;
    int numberOfVerticesPerAxis = (size+2) * resolution;
    vector<glm::vec3> renderVertices = vector<glm::vec3>(numberOfVerticesPerAxis * numberOfVerticesPerAxis);
    float stepSize = static_cast<float>(size+2) / static_cast<float>(resolution);

    // We are going to assume that our chunk size is 32x32 but we have been given the border around
    // the chunk resulting in 34x34 values from the heightmap. We only want to generate the
    // vertices for the 32x32 chunk before the chunk is saved the border vertices will have to
    // be removed.
    for (int i = 0; i < numberOfVerticesPerAxis; i++){
        for (int j = 0; j < numberOfVerticesPerAxis; j++){
            float x = i * stepSize;
            float z = j * stepSize;
            int x1 = static_cast<int>(x);
            int z1 = static_cast<int>(z);
            int x2 = x1 + 1;
            int z2 = z1 + 1;
            if ((x2 >= size || z2 >= size) || (x1 == x && z1 == z && x2 == x + 1 && z2 == z + 1)){
                renderVertices[j * numberOfVerticesPerAxis + i] = glm::vec3(
                    x,
                    Utility::height_scaling(vertices[z1 * size + x1].y, heightScalingFactor),
                    z
                );
            } else {
                renderVertices[j * numberOfVerticesPerAxis + i] = glm::vec3(
                    x,
                    Utility::height_scaling(
                        Utility::bilinear_interpolation(
                            glm::vec2(x, z),
                            vertices[z1 * size + x1], // bottom left
                            vertices[z1 * size + x2], // bottom right
                            vertices[z2 * size + x1], // top left
                            vertices[z2 * size + x2]  // top right
                        ),
                        heightScalingFactor
                    ),
                    z
                );
            }
        }
    }

}


glm::vec3 SubChunk::computeNormalContribution(glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
    return glm::normalize(glm::cross(B - A, C - A));
}

/*
    This method will take the generated vertices of the subchunk and generate the normals based
    on the adjacent vertices. This will allow us to calculate the lighting of the subchunk
    when rendering it.
*/
void SubChunk::generateRenderNormals()
{
    // Loop through all of the triangles (3 adjacent indexes) and calculate the normal for each
    // triangle. Add this contribution to each vertex normal and then normalize the vertex normal
    // to get the final normal for the vertex.
    vector<glm::vec3> renderNormals = vector<glm::vec3>(renderVertices.size());

    // Loop through the indexes in threes to calculate the normal for each triangle
    for (int i = 0; i < indicies.size(); i += 3){
        // Get the three vertices of the triangle
        glm::vec3 A = renderVertices[indicies[i]];
        glm::vec3 B = renderVertices[indicies[i+1]];
        glm::vec3 C = renderVertices[indicies[i+2]];
        // Calculate the normal for the triangle
        glm::vec3 triangleNormal = computeNormalContribution(A, B, C);
        // Add the normal to the vertex normals
        renderNormals[indicies[i]] += triangleNormal;
        renderNormals[indicies[i+1]] += triangleNormal;
        renderNormals[indicies[i+2]] += triangleNormal;
    }

    // Now we need to normalize the vertex normals
    for (int i = 0; i < renderNormals.size(); i++){
        renderNormals[i] = glm::normalize(renderNormals[i]);
    }
}

void SubChunk::cropToRemoveBorder(){
    // We want to extract the (size x size) centred region of the subchunk. This will remove the
    // 1*resolution wide vertex border around the edge. This will also remove 2*resolution number
    // of triangles from the border as well.
    int numberOfVerticesPerAxis = (size + 2) * resolution;
    vector<glm::vec3> extractedVertices = vector<glm::vec3>();
    vector<glm::vec3> extractedNormals = vector<glm::vec3>();
    vector<int> extractedIndicies = vector<int>();
    // Extracting the vertices and normals
    for (int x = 0; x < numberOfVerticesPerAxis; x++){
        for (int z = 0; z < numberOfVerticesPerAxis; z++){
            // Check if the vertex is within the bottom border
            if (x < resolution || z < resolution || x >= numberOfVerticesPerAxis - resolution || z >= numberOfVerticesPerAxis - resolution){
                continue;
            }
            extractedVertices.push_back(renderVertices[z * numberOfVerticesPerAxis + x]);
            extractedNormals.push_back(renderNormals[z * numberOfVerticesPerAxis + x]);
        }
    }
    // Extracting the indicies
    for (int i = 0; i < indicies.size(); i += 6){
        // Get the x and z coordinates of the bottom left vertex of the quad
        int x = indicies[i] % numberOfVerticesPerAxis;
        int z = indicies[i] / numberOfVerticesPerAxis;
        // Check if the quad is within the border
        if (x < resolution || z < resolution || x >= numberOfVerticesPerAxis - resolution || z >= numberOfVerticesPerAxis - resolution){
            continue;
        }
        // Add the indicies to the extracted indicies
        extractedIndicies.push_back(indicies[i]);
        extractedIndicies.push_back(indicies[i+1]);
        extractedIndicies.push_back(indicies[i+2]);
        extractedIndicies.push_back(indicies[i+3]);
        extractedIndicies.push_back(indicies[i+4]);
        extractedIndicies.push_back(indicies[i+5]);
    }
    // Finally set the render vertices, normals and indicies to the extracted values
    renderVertices = extractedVertices;
    renderNormals = extractedNormals;
    indicies = extractedIndicies;
}
/*
    This method will generate the index buffer for the subchunk based on the resolution of the
    mesh that will be generated for the subchunk.
*/
void SubChunk::generateIndexBuffer()
{
    int numberOfVerticesPerAxis = (size+2) * resolution;
    float stepSize = static_cast<float>(size+2) / static_cast<float>(resolution);
    indicies = vector<int>((numberOfVerticesPerAxis - 1) * (numberOfVerticesPerAxis - 1) * 2);
    for (int x = 0; x < numberOfVerticesPerAxis - 1; x++){
        for (int z = 0; z < numberOfVerticesPerAxis - 1; z++){
            // First triangle of the form [x,z], [x+1, z], [x+1, z+1]
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6)] = x * numberOfVerticesPerAxis + z;
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6) + 1] = (x + 1) * numberOfVerticesPerAxis + z;
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6) + 2] = (x + 1) * numberOfVerticesPerAxis + z + 1;
            // Second triangle of the form [x,z], [x+1, z+1], [x, z+1]
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6) + 3] = x * numberOfVerticesPerAxis + z;
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6) + 4] = (x + 1) * numberOfVerticesPerAxis + z + 1;
            indicies[(x * (numberOfVerticesPerAxis - 1) * 6) + 5] = x * numberOfVerticesPerAxis + z + 1;
        }
    }
}

/*
    This method will use the parents world coordinates and its Id to generate the world coordinates
    of the subchunk. This will allow the renderer to determine when to load and unload the subchunk
    based on the player's position in the world.
*/
vector<float> SubChunk::getSubChunkWorldCoords()
{
    // Get the world coordinates of the parent chunk
    vector<float> parentWorldCoords = parentChunk->getChunkWorldCoords();
    // Get the local coordinates of the subchunk
    vector<int> subChunkLocalCoords = getSubChunkCoords();
    // Calculate the world coordinates of the subchunk
    float x = parentWorldCoords[0] + subChunkLocalCoords[0] * size;
    float z = parentWorldCoords[1] + subChunkLocalCoords[1] * size;
    return vector<float>{x, z};
}


/*
    This method will generate the render data for the subchunk which will be used to render the
    subchunk in the world.
*/
void SubChunk::generateRenderData()
{
    // Start by computing the render vertices and normals
    generateRenderVertices();
    generateRenderNormals();
    // Generate the index buffer
    generateIndexBuffer();
    // Crop the render data to remove the border
    cropToRemoveBorder();
}


/*
    THis method is the class destructor which will be called when the subchunk is destroyed
*/
SubChunk::~SubChunk()
{
    // Nothing to do here
}