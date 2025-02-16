/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#include <vector>
#include <memory>
#include <string>
#include <optional>

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
#include "Chunk.hpp"
#include "Terrain.hpp"
#include "Settings.hpp"
#include "Utility.hpp"
#include "Player.hpp"
#include "World.hpp"

using namespace std;

World::World(
    long seed,
    vector<shared_ptr<Chunk>> chunks,
    Settings settings,
    shared_ptr<Player> player
): seed(seed), chunks(chunks), player(player){
    seaLevel = settings.getSeaLevel();
    maxHeight = settings.getMaximumHeight();
}

World::World(Settings settings, shared_ptr<Player> player): player(player){
    seed = generateRandomSeed();
    seaLevel = settings.getSeaLevel();
    maxHeight = settings.getMaximumHeight();
    cout << "Starting to generate the world" << endl;
    // This is temporary code to read a specific heightmap file to generate the heightmap
    string dataPath = getenv("DATA_ROOT");
    string heightmapPath = dataPath + settings.getFilePathDelimitter() + "noise_map_1026.raw";
    optional<vector<vector<float>>> heightmap = Utility::readHeightmap(heightmapPath.c_str(), settings.getChunkSize() + 2);
    if (!heightmap.has_value()){
        cout << "Failed to read the heightmap" << endl;
        return;
    }
    // Convert the heightmap into a 2D array of glm::vec3
    vector<vector<glm::vec3>> heightmapData = vector<vector<glm::vec3>>(
        settings.getChunkSize() + 3,
        vector<glm::vec3>(settings.getChunkSize() + 3)
    );
    for (int z = 0; z < settings.getChunkSize() + 2; z++){
        for (int x = 0; x < settings.getChunkSize() + 2; x++){
            heightmapData[z][x] = glm::vec3(x, (*heightmap)[z][x], z);
        }
    }

    // Temp duplicate the the last row and column of the heightmap to make it the right size
    // for (int z = 0; z < settings.getChunkSize() + 3; z++){
    //     heightmapData[z][settings.getChunkSize() + 2] = heightmapData[z][settings.getChunkSize() + 1];
    // }
    // for (int x = 0; x < settings.getChunkSize() + 3; x++){
    //     heightmapData[settings.getChunkSize() + 2][x] = heightmapData[settings.getChunkSize() + 1][x];
    // }

    // We are going to generate (subchunk_size + 1) x (subchunk_size + 1) terrains every
    // subchunk_size vertices in the heightmap so that they overlap by 2 vertices
    // int subchunkSize = settings.getSubChunkSize();
    vector<float> worldCoords = vector<float>(2);
    worldCoords[0] = -512.0f;
    worldCoords[1] = -512.0f;
    // worldCoords[0] = 0.0f;
    // worldCoords[1] = 0.0f;

    string shaderRoot = getenv("SHADER_ROOT");
    shared_ptr<Shader> terrainShader = make_shared<Shader>(
        shaderRoot + settings.getFilePathDelimitter() + "terrain_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "terrain_shader.fs"
    );

    Chunk chunk = Chunk(
        (long) 0,
        make_shared<Settings>(settings),
        vector<int>{0, 0},
        heightmap.value(),
        terrainShader
    );

    shared_ptr<Chunk> chunkPtr = make_shared<Chunk>(chunk);
    chunkPtr->loadAllSubChunks();
    addChunk(chunkPtr);

}

void World::render(glm::mat4 view, glm::mat4 projection){
    for (auto chunk : chunks){
        chunk->render(view, projection);
    }
}

void World::setupData(){
    // for (auto terrain : terrains){
    //     terrain->setupData();
    // }
    // Do nothing
}

void World::updateData(){
    // Iterate through the chunks to determine the subchunks that need to be loaded
    for (auto chunk : chunks){
        chunk->updateLoadedSubChunks(player->getPosition(), *chunk->getSettings());
    }
}

long World::generateRandomSeed(){
    return 0;
}