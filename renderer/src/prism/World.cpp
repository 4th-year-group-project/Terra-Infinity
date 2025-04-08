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
    seed = 23; // This needs to be changed mannually to try out a different world.
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

    string diffuseTextureRoot = getenv("DIFFUSE_TEXTURE_ROOT");

    std::vector<std::string> texturePaths = {
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateRainforestTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateRainforestTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateRainforestTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateRainforestTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getBorealForestTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getBorealForestTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getBorealForestTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getBorealForestTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getGrasslandTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getGrasslandTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getGrasslandTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getGrasslandTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTundraTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTundraTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTundraTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTundraTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getSavannaTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getSavannaTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getSavannaTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getSavannaTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getWoodlandTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getWoodlandTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getWoodlandTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getWoodlandTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTropicalRainforestTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTropicalRainforestTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTropicalRainforestTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTropicalRainforestTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateForestTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateForestTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateForestTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getTemperateForestTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getDesertTexture1(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getDesertTexture2(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getDesertTexture3(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + settings.getParameters()->getDesertTexture4(),
        diffuseTextureRoot + settings.getFilePathDelimitter() + "snow_1k.jpg", // Set ocean to snow for now as we should not see it
        diffuseTextureRoot + settings.getFilePathDelimitter() + "snow_1k.jpg",
        diffuseTextureRoot + settings.getFilePathDelimitter() + "snow_1k.jpg",
        diffuseTextureRoot + settings.getFilePathDelimitter() + "snow_1k.jpg"
    };
       
    // Generate the biome texture array
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);

    // Load the first image to get size and format
    unsigned char* data = stbi_load(texturePaths[0].c_str(), &width, &height, &channels, 4);
    if (!data) throw std::runtime_error("Failed to load texture: " + texturePaths[0]);
    stbi_image_free(data);

    GLuint textureArrayID;
    glGenTextures(1, &textureArrayID);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArrayID);

    // Allocate the full texture array
    glTexImage3D(
        GL_TEXTURE_2D_ARRAY, 0, GL_RGBA8, 
        width, height, static_cast<GLsizei>(texturePaths.size()), 
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr
    );

    // Upload each texture to a layer in the array
    for (size_t i = 0; i < texturePaths.size(); ++i) {
        int w, h, c;
        unsigned char* image = stbi_load(texturePaths[i].c_str(), &w, &h, &c, 4);
        if (!image) {
            throw std::runtime_error("Failed to load texture: " + texturePaths[i]);
        }

        if (w != width || h != height) {
            stbi_image_free(image);
            throw std::runtime_error("All textures must be the same size: " + texturePaths[i]);
        }

        glTexSubImage3D(
            GL_TEXTURE_2D_ARRAY,
            0,
            0, 0, static_cast<GLint>(i),
            width, height, 1,
            GL_RGBA, GL_UNSIGNED_BYTE,
            image
        );

        stbi_image_free(image);
    }

    // Mipmap + filtering
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    biomeTextureArray = textureArrayID;

    
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
    // Update the chunks
    updateLoadedChunks();
    // Iterate through the chunks to determine the subchunks that need to be loaded
    for (int i = 0; i < static_cast<int>(chunks.size()); i++){
        chunks[i]->updateLoadedSubChunks(player->getPosition(), *chunks[i]->getSettings());
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
    packetData->lenHeightmapData = *reinterpret_cast<uint32_t*>(data + index);
    index += sizeof(uint32_t);
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
    packetData->biomeDataSize = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->lenBiomeData = *reinterpret_cast<uint32_t*>(data + index);
    index += sizeof(uint32_t);
    // Extract the heightmap data
    for (int z = 0; z < packetData->vz; z++){
        vector<float> heightmapRow = vector<float>();
        for (int x = 0; x < packetData->vx; x++){
            // We know that each element in the heightmap data is size bits long (16 bits)
            uint16_t entry = *reinterpret_cast<uint16_t*>(data + index);
            index += sizeof(uint16_t);
            // We need to ensure that the value ranges from 0 to 1
            float entryFloat = static_cast<float>(entry) / 65535.0f;

            heightmapRow.push_back(entryFloat);
        }
        packetData->heightmapData.push_back(heightmapRow);
    }
    // Extract the biome data
    for (int z = 0; z < packetData->vz; z++){
        vector<uint8_t> biomeRow = vector<uint8_t>();
        for (int x = 0; x < packetData->vx; x++){
            // We know that each element in the biome data is 8 bits long
            uint8_t entry = *reinterpret_cast<uint8_t*>(data + index);
            index += sizeof(uint8_t);
            // We need to ensure that the value ranges from 0 to 1
            biomeRow.push_back(entry);
        }
        packetData->biomeData.push_back(biomeRow);
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
    // cout << "Getting chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
    if (chunkCoords.size() != 2){
        cerr << "ERROR: The chunk coordinates are not of the correct size" << endl;
        return nullptr;
    }
    string dataPath;
#ifdef DEPARTMENT_BUILD
    dataPath = "/dcs/large/efogahlewem/chunks/backups";
#else
    dataPath = getenv("PROJECT_ROOT");
    // dataPath += std::string(1, settings.getFilePathDelimitter()) + "chunks" + std::string(1, settings.getFilePathDelimitter()) + "backups";
    dataPath += std::string(1, settings.getFilePathDelimitter()) + "chunks" + std::string(1, settings.getFilePathDelimitter()) + "biomes";
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
    // cout << "Seed: " << packetData->seed << endl;
    // cout << "cx: " << packetData->cx << endl;
    // cout << "cz: " << packetData->cz << endl;
    // cout << "num_vertices: " << packetData->num_vertices << endl;
    // cout << "size: " << packetData->size << endl;
    // cout << "lenHeightmapData: " << packetData->lenHeightmapData << endl;
    // Use the the data to create a new chunk
    shared_ptr<Chunk> chunk = make_shared<Chunk>(
        packetData->cx + packetData->cz * numeric_limits<int>::max(),
        make_shared<Settings>(settings),
        chunkCoords,
        packetData->heightmapData,
        packetData->biomeData,
        terrainShader,
        oceanShader,
        terrainTextures,
        biomeTextureArray
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
    // We need to get the current chunk that the player is in
    // We need to get the player's position and then determine which chunk the player is in
    glm::vec3 playerPosition = player->getPosition();
    int chunkSize = settings->getChunkSize();
    int chunkX = static_cast<int>(floor(playerPosition.x / chunkSize));
    int chunkZ = static_cast<int>(floor(playerPosition.z / chunkSize));
    return vector<int>{chunkX, chunkZ};
}

float World::distanceToChunkCenter(vector<int> chunkCoords, shared_ptr<Settings> settings){
    // Get the world coordinates of the chunk
    vector<float> chunkWorldCoords = vector<float>(2);
    chunkWorldCoords[0] = chunkCoords[0] * settings->getChunkSize();
    chunkWorldCoords[1] = chunkCoords[1] * settings->getChunkSize();
    // Get the distance between the players current position and the center of the chunk
    glm::vec3 playerPos = player->getPosition();

    float chunkMidX = chunkWorldCoords[0] + settings->getChunkSize() / 2;
    float chunkMidZ = chunkWorldCoords[1] + settings->getChunkSize() / 2;
    float distance = sqrt(pow(playerPos.x - chunkMidX, 2) + pow(playerPos.z - chunkMidZ, 2));
    return distance;
}

void World::updateLoadedChunks(){
    // We need to check the neighbouring chunks of the player to determine which chunks need to be
    // loaded or unloaded
    // We need to get the current chunk that the player is in and then check the neighbouring chunks with a radius of 2 chunks
    shared_ptr<Settings> settings = chunks[0]->getSettings();  // Get the settings from the first chunk
    vector<int> playerChunk = getPlayersCurrentChunk(settings);
    // We need to iterate through the chunks and determine which chunks need to be loaded or unloaded
    vector<shared_ptr<Chunk>> newChunks;
    for (int x = -2; x < 3; x++){
        for (int z = -2; z < 3; z++){
            vector<int> chunkCoords = {playerChunk[0] + x, playerChunk[1] + z};
            // Check if the center of the chunk is within 2 times the render distance
            if (distanceToChunkCenter(chunkCoords, settings) < settings->getRequestDistance()){
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
                    cout << "Requesting chunk: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
                    shared_ptr<Chunk> chunk = requestNewChunk(chunkCoords, *settings);
                    if (chunk == nullptr){
                        cerr << "ERROR: Failed to load the chunk at seed: " << seed << " and coords: " << chunkCoords[0] << ", " << chunkCoords[1] << endl;
                        return;
                    }
                    newChunks.push_back(chunk);
                }
            }
        }
    }
    chunks.clear();
    chunks = newChunks;
}
