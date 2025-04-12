/*
    Each large "super chunk" will be split up into 32x32 subchunks which will be loaded and unloaded
    dynamically by the renderer based on the player's position in the world and their view distance.

    This will allow us to generate and render subchunks at high resolutions than previously as we
    will only need to generate the subchunks that are within the player's view distance.
*/

#include <SubChunk.hpp>
#include <Utility.hpp>

/*
    This method will use the parents world coordinates and its Id to generate the world coordinates
    of the subchunk. This will allow the renderer to determine when to load and unload the subchunk
    based on the player's position in the world.
*/
vector<float> SubChunk::getSubChunkWorldCoords(shared_ptr<Settings> settings)
{
    // Get the world coordinates of the parent chunk
    vector<float> parentWorldCoords = parentChunk->getChunkWorldCoords();
    int parentSize = settings->getChunkSize();
    parentSize++;
    // Get the local coordinates of the subchunk
    vector<int> subChunkLocalCoords = getSubChunkCoords();
    // Calculate the world coordinates of the subchunk
    float x = parentWorldCoords[0] + subChunkLocalCoords[0]; //- (parentSize - 1) / 2;
    float z = parentWorldCoords[1] + subChunkLocalCoords[1]; // - (parentSize - 1) / 2;
    return vector<float>{x, z};
}

SubChunk::SubChunk(
    int inId,
    shared_ptr<Chunk> inParentChunk,
    shared_ptr<Settings> settings,
    vector<int> inSubChunkCoords,
    vector<vector<float>> inHeights,
    vector<vector<uint8_t>> inBiomes,
    shared_ptr<Shader> inTerrainShader,
    shared_ptr<Shader> inOceanShader,
    vector<shared_ptr<Texture>> inTerrainTextures
):
    id(inId),
    size(settings->getSubChunkSize()),
    resolution(settings->getSubChunkResolution()),
    parentChunk(inParentChunk),
    subChunkCoords(inSubChunkCoords),
    heights(inHeights),
    biomes(inBiomes),
    terrainShader(inTerrainShader),
    oceanShader(inOceanShader),
    terrainTextures(inTerrainTextures)
{
    // Generate the terrain object for the subchunk
    terrain = make_shared<Terrain>(
        inHeights,
        make_shared<vector<vector<uint8_t>>>(inBiomes),
        settings,
        getSubChunkWorldCoords(settings),
        inTerrainShader,
        inTerrainTextures,
        parentChunk->getTerrainTextureArrays()
    );

    ocean = make_shared<Ocean>(
        vector<float>{0.0f, 0.0f},
        getSubChunkWorldCoords(settings),
        settings,
        inOceanShader
    );
}

SubChunk::SubChunk(
    int inId,
    shared_ptr<Chunk> inParentChunk,
    shared_ptr<Settings> settings,
    float inResolution,
    vector<int> inSubChunkCoords,
    vector<vector<float>> inHeights,
    vector<vector<uint8_t>> inBiomes,
    shared_ptr<Shader> inTerrainShader,
    shared_ptr<Shader> inOceanShader,
    vector<shared_ptr<Texture>> inTerrainTextures
):
    id(inId),
    size(settings->getSubChunkSize()),
    resolution(inResolution),
    parentChunk(inParentChunk),
    subChunkCoords(inSubChunkCoords),
    heights(inHeights),
    biomes(inBiomes),
    terrainShader(inTerrainShader),
    oceanShader(inOceanShader),
    terrainTextures(inTerrainTextures)
{
    // Generate the terrain object for the subchunk
    terrain = make_shared<Terrain>(
        inHeights,
        make_shared<vector<vector<uint8_t>>>(inBiomes),
        inResolution,
        settings,
        getSubChunkWorldCoords(settings),
        inTerrainShader,
        inTerrainTextures,
        parentChunk->getTerrainTextureArrays()
    );

    ocean = make_shared<Ocean>(
        vector<float>{0.0f, 0.0f},
        getSubChunkWorldCoords(settings),
        settings,
        inOceanShader
    );
}

/*
    THis method is the class destructor which will be called when the subchunk is destroyed
*/
SubChunk::~SubChunk()
{
    // Nothing to do here
}


void SubChunk::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
)
{
    // Render the terrain object
    ocean->render(view, projection, lights, viewPos); // Render this first so that we can make the most of depth culling
    terrain->render(view, projection, lights, viewPos);
}

void SubChunk::setupData()
{
    // Do nothing
    // // Setup the terrain object
    // terrain->setupData();
}

void SubChunk::updateData(bool)
{
    // Do nothing
    // // Update the terrain object
    // terrain->updateData();
}