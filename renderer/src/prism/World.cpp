/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <limits>

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


    string textureRoot = getenv("TEXTURE_ROOT");
    // Create the skybox
    vector<string> faces = {
        (textureRoot + settings.getFilePathDelimitter() + "right.bmp"),
        (textureRoot + settings.getFilePathDelimitter() + "left.bmp"),
        (textureRoot + settings.getFilePathDelimitter() + "top.bmp"),
        (textureRoot + settings.getFilePathDelimitter() + "bottom.bmp"),
        (textureRoot + settings.getFilePathDelimitter() + "front.bmp"),
        (textureRoot + settings.getFilePathDelimitter() + "back.bmp"),
    };
    skyBox = make_shared<SkyBox>(
        faces,
        settings
    );

    string shaderRoot = getenv("SHADER_ROOT");
    shared_ptr<Shader> tempShader = make_shared<Shader>(
        shaderRoot + settings.getFilePathDelimitter() + "terrain_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "terrain_shader.fs"
    );

    terrainShader = tempShader;

    shared_ptr<Shader> tempOceanShader = make_shared<Shader>(
        shaderRoot + settings.getFilePathDelimitter() + "ocean_shader.vs",
        shaderRoot + settings.getFilePathDelimitter() + "ocean_shader.fs"
    );
    oceanShader = tempOceanShader;

    // Creating the vector of terrain textures
    Texture grassTexture = Texture(
        textureRoot + settings.getFilePathDelimitter() + "grass_1k.jpg",
        "texture_diffuse",
        "grassTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(grassTexture));
    Texture rockTexture = Texture(
        textureRoot + settings.getFilePathDelimitter() + "rock_1k.jpg",
        "texture_diffuse",
        "rockTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(rockTexture));
    Texture snowTexture = Texture(
        textureRoot + settings.getFilePathDelimitter() + "snow_1k.jpg",
        "texture_diffuse",
        "snowTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(snowTexture));
    Texture sandTexture = Texture(
        textureRoot + settings.getFilePathDelimitter() + "sand_1k.jpg",
        "texture_diffuse",
        "sandTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(sandTexture));

    // set up the initial chunks
    shared_ptr<Chunk> chunk00 = requestNewChunk(vector<int>{0, 0}, settings);
    // chunk00->addSubChunk(0);
    addChunk(chunk00);
    shared_ptr<Chunk> chunk01 = requestNewChunk(vector<int>{0, 1}, settings);
    addChunk(chunk01);
    shared_ptr<Chunk> chunk10 = requestNewChunk(vector<int>{1, 0}, settings);
    addChunk(chunk10);
    shared_ptr<Chunk> chunk11 = requestNewChunk(vector<int>{1, 1}, settings);
    addChunk(chunk11);

    // Chunk chunk = Chunk(
    //     (long) 0,
    //     make_shared<Settings>(settings),
    //     vector<int>{0, 0},
    //     heightmap.value(),
    //     terrainShader
    // );

    // shared_ptr<Chunk> chunkPtr = make_shared<Chunk>(chunk);
    // chunkPtr->addSubChunk(0);
    // addChunk(chunkPtr);
    // // chunkPtr->loadAllSubChunks();
    // addChunk(chunkPtr);

}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void World::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
){
    // We are going to render the skybox first
    skyBox->render(view, projection, lights, viewPos);
    for (auto chunk : chunks){
        chunk->render(view, projection, lights, viewPos);
    }
}
#pragma GCC diagnostic pop

void World::setupData(){
    // for (auto terrain : terrains){
    //     terrain->setupData();
    // }
    // Do nothing
}



void World::updateData(){
    skyBox->updateData();
    // Iterate through the chunks to determine the subchunks that need to be loaded
    for (auto chunk : chunks){
        chunk->updateLoadedSubChunks(player->getPosition(), *chunk->getSettings());
    }
}

long World::generateRandomSeed(){
    time_t currentTime = time(0);
    srand(currentTime);
    int msbRandom = rand();
    int lsbRandom = rand();
    long seed = (static_cast<long>(msbRandom) << 32) | static_cast<long>(lsbRandom);
    return seed;
}



unique_ptr<PacketData> World::readPacketData(char *data, int len){
    unique_ptr<PacketData> packetData = make_unique<PacketData>();
    // We are going to iterate through the data that we have received and extract each field
    // in turn
    int index = 0;
    // Extract the seed
    packetData->seed = *reinterpret_cast<long*>(data + index);
    index += sizeof(long);
    packetData->cx = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->cz = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->num_vertices = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->vx = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->vz = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->size = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->lenHeightmapData = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    // Ensure that lenHeightmapData is cx * cz * size
    if (packetData->lenHeightmapData != packetData->num_vertices * (packetData->size / 8)){
        return nullptr;
        cerr << "ERROR: The length of the heightmap data does not match the expected length" << endl;
    }
    packetData->heightmapData = vector<vector<float>>();
    // Extract the heightmap data
    for (int z = 0; z < packetData->vz; z++){
        vector<float> heightmapRow = vector<float>();
        for (int x = 0; x < packetData->vx; x++){
            // We know that each element in the heightmap data is size bits long
            int16_t entry = *reinterpret_cast<uint16_t*>(data + index);
            index += sizeof(uint16_t);
            
            // We need to ensure that the value ranges from 0 to 1
            float entryFloat = static_cast<float>(entry) / 65535.0f;

            heightmapRow.push_back(entryFloat);
        }
        packetData->heightmapData.push_back(heightmapRow);
    }
    // Ensure that we have read all the data
    if (index != len){
        return nullptr;
        cerr << "ERROR: The length of the data does not match the expected length" << endl;
    }
    return packetData;
}

/**
 * This function is going to be calling the master script for the world generation which is
 * written in python. The script will be asynchronously called to allow the renderer to continue
 * to run while the chunk is being generated.
 *
 * We will use popen to call the python script from the command line and then read the output
 * from the script to get the data for the chunk
 */
shared_ptr<Chunk> World::requestNewChunk(vector<int> chunkCoords, Settings settings){
    cout << "Getting chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
    if (chunkCoords.size() != 2){
        cerr << "ERROR: The chunk coordinates are not of the correct size" << endl;
        return nullptr;
    }
    string dataPath = getenv("PROJECT_ROOT");
    string filePath;
    if (chunkCoords[0] == 0 && chunkCoords[1] == 0){
        // Read the file output0.bin file and store it to a string
        filePath = dataPath + "/output0.bin";
    } else if (chunkCoords[0] == 0 && chunkCoords[1] == 1){
        filePath = dataPath + "/output1.bin";
    } else if (chunkCoords[0] == 1 && chunkCoords[1] == 0){
        filePath = dataPath + "/output2.bin";
    } else if (chunkCoords[0] == 1 && chunkCoords[1] == 1){
        filePath = dataPath + "/output3.bin";
    }
    FILE* file = fopen(filePath.c_str(), "rb");
    if (file == nullptr){
        cerr << "ERROR: Failed to open the file" << endl;
        return nullptr;
    }
    fseek(file, 0, SEEK_END);
    long len = ftell(file);
    fseek(file, 0, SEEK_SET);
    char* data = new char[len];
    fread(data, 1, len, file);
    fclose(file);
    // Parse the data to extract the chunk data
    unique_ptr<PacketData> packetData = readPacketData(data, len);
    if (packetData == nullptr){
        cerr << "ERROR: Failed to read the packet data" << endl;
        return nullptr;
    }
    // print out the data for debugging
    cout << "Seed: " << packetData->seed << endl;
    cout << "cx: " << packetData->cx << endl;
    cout << "cz: " << packetData->cz << endl;
    cout << "num_vertices: " << packetData->num_vertices << endl;
    cout << "size: " << packetData->size << endl;
    cout << "lenHeightmapData: " << packetData->lenHeightmapData << endl;
    // Use the the data to create a new chunk
    shared_ptr<Chunk> chunk = make_shared<Chunk>(
        packetData->cx + packetData->cz * numeric_limits<int>::max(),
        make_shared<Settings>(settings),
        chunkCoords,
        packetData->heightmapData,
        terrainShader,
        oceanShader,
        terrainTextures
    );
    return chunk;
}
    // string command = "cd ..; python3 -m master_script.master_script --seed" + to_string(seed) + "--cx" + to_string(chunkCoords[0]) + "--cy" + to_string(chunkCoords[1]) + "; cd renderer";
    // FILE* pipe = popen(command.c_str(), "r");
    // if (!pipe) {
    //     cerr << "Failed to run python script" << endl;
    //     return nullptr;
    // }

    // char buffer[1024 * 1024 * 8 * 50]; //50MB buffer
    // string result = "";
    // while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
    //     result += buffer;
    // }
    // pclose(pipe);



    // Parse the result to extract chunk data
    // Assuming the script returns data in a specific format, e.g., JSON
    // You can use a JSON library to parse the result
    // For simplicity, let's assume the result is a simple space-separated string of heightmap values


void World::setUpInitialChunks(){
    return;
}

float World::distanceToChunkCenter(long chunkId){
    cout << chunkId << endl;
    return 0.0f;
}

void World::updateLoadedChunks(){
    // We need to check the neighbouring chunks of the player to determine which chunks need to be
    // loaded or unloaded
    
    return;
}
