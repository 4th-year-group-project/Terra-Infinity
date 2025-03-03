/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <limits>
#include <chrono>
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
    // seed = generateRandomSeed();
    seed = 70; // This needs to be changed mannually to try out a different world.
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

    // Noise displacement map
    Texture noiseTexture = Texture(
        textureRoot + settings.getFilePathDelimitter() + "noise_image.png",
        "texture_diffuse",
        "noiseTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(noiseTexture));

    // set up the initial chunks
    setUpInitialChunks(settings);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void World::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos
){
    double start = omp_get_wtime();
    cout << "Player position: " << player->getPosition().x << ", " << player->getPosition().y << ", " << player->getPosition().z << endl;
    // We are going to render the skybox first
    skyBox->render(view, projection, lights, viewPos);
    for (auto chunk : chunks){
        chunk->render(view, projection, lights, viewPos);
    }
    double end = omp_get_wtime();
    cout << "Time to render world: " << end - start << endl;
}
#pragma GCC diagnostic pop

void World::setupData(){
    // for (auto terrain : terrains){
    //     terrain->setupData();
    // }
    // Do nothing
}


void World::updateData(){
    // double start = omp_get_wtime();
    // skyBox->updateData();
    // // Update the chunks
    // updateLoadedChunks();
    // cout << "Number of chunks: " << chunks.size() << endl;
    // // Iterate through the chunks to determine the subchunks that need to be loaded
    // for (auto chunk : chunks){
    //     chunk->updateLoadedSubChunks(player->getPosition(), *chunk->getSettings());
    // }
    // double end = omp_get_wtime();
    // cout << "Time to update world: " << end - start << endl;

    // Temp
    // Render the 3x3 grid of chunks centered around (512, 512)
    // for (auto chunk : chunks){
    //     chunk->addSubChunk(544, 1);
    //     chunk->addSubChunk(545, 1);
    //     chunk->addSubChunk(546, 1);
    //     // Next row
    //     chunk->addSubChunk(577, 1);
    //     chunk->addSubChunk(578, 1);
    //     chunk->addSubChunk(579, 1);
    //     // Next row
    //     chunk->addSubChunk(610, 1);
    //     chunk->addSubChunk(611, 1);
    //     chunk->addSubChunk(612, 1);
    // }

    for (auto chunk : chunks){
        chunk->addSubChunk(0, 1);
        chunk->addSubChunk(1, 1);
        chunk->addSubChunk(2, 1);
        // Next row
        chunk->addSubChunk(33, 1);
        chunk->addSubChunk(34, 1);
        chunk->addSubChunk(35, 1);
        // Next row
        chunk->addSubChunk(66, 1);
        chunk->addSubChunk(67, 1);
        chunk->addSubChunk(68, 1);
    }
}

long World::generateRandomSeed(){
    // Get the current time without using time function and initialise srand
    auto now = chrono::system_clock::now();
    auto duration = now.time_since_epoch();
    auto millis = chrono::duration_cast<chrono::milliseconds>(duration).count();
    srand(millis);
    int msbRandom = rand();
    int lsbRandom = rand();
    uint64_t u_seed = (static_cast<uint64_t>(msbRandom) << 32) | static_cast<uint64_t>(lsbRandom);
    long seed = static_cast<long>(u_seed);
    return seed;
}



unique_ptr<PacketData> World::readPacketData(char *data, int len){
    unique_ptr<PacketData> packetData = make_unique<PacketData>();
    // We are going to iterate through the data that we have received and extract each field
    // in turn
    int index = 0;
    // Extract the seed
    packetData->seed = *reinterpret_cast<long*>(data + index);
#ifdef _WIN32
    index += sizeof(long) + sizeof(int);
#else
    index += sizeof(long);
#endif
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
    // cout << "Seed: " << packetData->seed << endl;
    // cout << "cx: " << packetData->cx << endl;
    // cout << "cz: " << packetData->cz << endl;
    // cout << "num_vertices: " << packetData->num_vertices << endl;
    // cout << "vx: " << packetData->vx << endl;
    // cout << "vz: " << packetData->vz << endl;
    // cout << "size: " << packetData->size << endl;
    // cout << "lenHeightmapData: " << packetData->lenHeightmapData << endl;
    // Ensure that the length of the heightmap data is correct
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
            uint16_t entry = *reinterpret_cast<uint16_t*>(data + index);
            index += sizeof(uint16_t);
            // We need to ensure that the value ranges from 0 to 1
            float entryFloat = static_cast<float>(entry) / 65535.0f;

            heightmapRow.push_back(entryFloat);
        }
        packetData->heightmapData.push_back(heightmapRow);
    }
    cout << "Index: " << index << endl;
    cout << "len: " << len << endl;
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
    double start = omp_get_wtime();
    cout << "Getting chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
    if (chunkCoords.size() != 2){
        cerr << "ERROR: The chunk coordinates are not of the correct size" << endl;
        return nullptr;
    }
    string dataPath;
#ifdef DEPARTMENT_BUILD
    dataPath = "/dcs/large/efogahlewem/chunks/backups";
    cout << "Department" << endl;
#else
    cout << "Non department" << endl;
    dataPath = getenv("PROJECT_ROOT");
    dataPath += "/chunks/backups";
#endif
    string filePath;
    filePath = dataPath + settings.getFilePathDelimitter() + to_string(seed) + "_" + to_string(chunkCoords[0]) + "_" + to_string(chunkCoords[1]) + ".bin";
    FILE* file = fopen(filePath.c_str(), "rb");
    if (file == nullptr){
        cerr << "ERROR: Failed to open the file: " << filePath << endl;
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
    double end = omp_get_wtime();
    cout << "Time to load chunk: " << end - start << " for chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
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


void World::setUpInitialChunks(Settings settings){
    // We are going to request the 3x3 chunks around the player to be loaded initially
    for (int x = -1; x < 2; x++){
        for (int z = -1; z < 2; z++){
            vector<int> chunkCoords = {x, z};
            shared_ptr<Chunk> chunk = requestNewChunk(chunkCoords, settings);
            if (chunk == nullptr){
                cerr << "ERROR: Failed to load the chunk at seed: " << seed << " and coords: " << x << ", " << z << endl;
                return;
            }
            chunks.push_back(chunk);
        }
    }
}

vector<int> World::getPlayersCurrentChunk(shared_ptr<Settings> settings){
    // Get the player's position
    glm::vec3 playerPosition = player->getPosition();
    int chunkSize = settings->getChunkSize();
    // Calculate chunk coordinates considering the offset
    // For (0,0) chunk starting at (-chunkSize/2, -chunkSize/2)
    int chunkX = floor((playerPosition.x + chunkSize/2) / static_cast<float>(chunkSize));
    int chunkZ = floor((playerPosition.z + chunkSize/2) / static_cast<float>(chunkSize));
    
    return vector<int>{chunkX, chunkZ};
}

float World::distanceToChunkCenter(vector<int> chunkCoords){
    // Get the world coordinates of the chunk
    vector<float> chunkWorldCoords = vector<float>(2);
    chunkWorldCoords[0] = chunkCoords[0] * 1024;
    chunkWorldCoords[1] = chunkCoords[1] * 1024;
    // Get the distance between the players current position and the center of the chunk
    glm::vec3 playerPos = player->getPosition();
    float chunkMidX = chunkWorldCoords[0] + 512;
    float chunkMidZ = chunkWorldCoords[1] + 512;
    float distance = sqrt(pow(playerPos.x - chunkMidX, 2) + pow(playerPos.z - chunkMidZ, 2));
    // cout << "Distance to chunk center: " << distance << " for chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
    return distance;
}

void World::updateLoadedChunks(){
    double start = omp_get_wtime();
    // We need to check the neighbouring chunks of the player to determine which chunks need to be
    // loaded or unloaded
    // We need to get the current chunk that the player is in and then check the neighbouring chunks with a radius of 2 chunks
    shared_ptr<Settings> settings = chunks[0]->getSettings();  // Get the settings from the first chunk
    vector<int> playerChunk = getPlayersCurrentChunk(settings);

    // We need to iterate through the chunks and determine which chunks need to be loaded or unloaded
    vector<shared_ptr<Chunk>> newChunks = vector<shared_ptr<Chunk>>();
    cout << "Player chunk: " << playerChunk[0] << ", " << playerChunk[1] << endl;
    #pragma omp parallel for
    for (int x = -3; x < 4; x++){
        for (int z = -3; z < 4; z++){
            vector<int> chunkCoords = {playerChunk[0] + x, playerChunk[1] + z};
            // Check if the center of the chunk is within 2 times the render distance
            if (distanceToChunkCenter(chunkCoords) < settings->getRequestDistance()){
                // Check if the chunk is already loaded
                bool chunkLoaded = false;
                for (auto chunk : chunks){
                    if (chunk->getChunkCoords() == chunkCoords){
                        chunkLoaded = true;
                        newChunks.push_back(chunk);
                        break;
                    }
                }
                if (!chunkLoaded){
                    // Request the chunk to be loaded
                    shared_ptr<Chunk> chunk = requestNewChunk(chunkCoords, *settings);
                    if (chunk == nullptr){
                        cerr << "ERROR: Failed to load the chunk at seed: " << seed << " and coords: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
                    }
                    newChunks.push_back(chunk);
                }
            }
        }
    }
    chunks.clear();
    chunks = newChunks;
    double end = omp_get_wtime();
    cout << "Time to update chunks: " << end - start << endl;
}
