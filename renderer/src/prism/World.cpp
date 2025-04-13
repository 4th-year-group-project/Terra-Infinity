/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#include <vector>
#include <memory>
#include <mutex>
#include <future>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <curl/curl.h> // This will be used to complete the http requests
#include <nlohmann/json.hpp> // This will be used to parse the json data

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
#include "TextureArray.hpp"
#include "WaterFrameBuffer.hpp"
#include "SkyBox.hpp"


World::World(
    std::shared_ptr<Settings> settings,
    std::shared_ptr<Player> player,
    std::shared_ptr<WaterFrameBuffer> inReflectionBuffer,
    std::shared_ptr<WaterFrameBuffer> inRefractionBuffer
): 
    settings(settings),
    player(player) ,
    reflectionBuffer(inReflectionBuffer),
    refractionBuffer(inRefractionBuffer)
{
    seed = settings->getParameters()->getSeed();
    seaLevel = settings->getSeaLevel();
    maxHeight = settings->getMaximumHeight();  //This is the renderers max height not the generator
    // Ensure that the vector of chunks and requests is empty
    std::lock_guard<std::mutex> lock(chunkMutex);
    std::lock_guard<std::mutex> lock2(requestMutex);
    chunks.clear();
    chunkRequests.clear();


    std::string shaderRoot = getenv("SHADER_ROOT");

    string textureRoot = getenv("TEXTURE_ROOT");
    // Create the skybox
    vector<string> skyboxTextures = {
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "right.bmp"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "left.bmp"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "top.bmp"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "bottom.bmp"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "front.bmp"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "back.bmp"),
    };
    skyBox = make_shared<SkyBox>(skyboxTextures, settings);
    terrainShader = make_shared<Shader>(
        shaderRoot + settings->getFilePathDelimitter() + "terrain_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "terrain_shader.fs"
    );
    oceanShader = make_shared<Shader>(
        shaderRoot + settings->getFilePathDelimitter() + "ocean_shader.vs",
        shaderRoot + settings->getFilePathDelimitter() + "ocean_shader.fs"
    );
    
    // Noise displacement map
    Texture noiseTexture = Texture(
        textureRoot + settings->getFilePathDelimitter() + "noise_image.png",
        "texture_diffuse",
        "noiseTexture"
    );
    terrainTextures.push_back(make_shared<Texture>(noiseTexture));

    // We need to iterate through the list of textures and bind them in order
    for (int i = 0; i < static_cast<int> (terrainTextures.size()); i++){
        terrainTextures[i]->bind(i + 1); 
    }
    // Ocean textures
    oceanTextures = std::vector<shared_ptr<Texture>>();
    oceanTextures.push_back(make_shared<Texture>(
        textureRoot + settings->getFilePathDelimitter() + "water" + settings->getFilePathDelimitter() + "normal.png",
        "texture_normal",
        "normalTexture"
    ));
    oceanTextures.push_back(make_shared<Texture>(
        textureRoot + settings->getFilePathDelimitter() + "water" + settings->getFilePathDelimitter() + "dudv.png",
        "texture_dudv",
        "dudvTexture"
    ));
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
void World::render(
    glm::mat4 view,
    glm::mat4 projection,
    vector<shared_ptr<Light>> lights,
    glm::vec3 viewPos,
    bool isWaterPass,
    bool isShadowPass,
    glm::vec4 plane
){
    // We are going to render the skybox first
    skyBox->render(view, projection, lights, viewPos, isWaterPass, isShadowPass, plane);
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    for (auto chunk : chunks){
        chunk->render(view, projection, lights, viewPos, isWaterPass, isShadowPass, plane);
    }
}
#pragma GCC diagnostic pop

void World::setupData(){
    // for (auto terrain : terrains){
    //     terrain->setupData();
    // }
    // Do nothing
}

void World::updateData(bool regenerate){
    // Check if the world needs to be regenerated
    // This is blocking the main thread
    if (regenerate){
        player->setPosition(glm::vec3(0.0f, 80.0f, 0.0f)); // Player should always spawn at the origin
        player->getCamera()->setPosition(glm::vec3(0.0f, 80.0f, 0.0f) + glm::vec3(1.68f, 0.2f, 0.2f));

        // Regenerate the spawn chunks
        seed = settings->getParameters()->getSeed();
        regenerateSpawnChunks(player->getPosition());

        // Load texture array images into memory here
        string textureRoot = getenv("TEXTURE_ROOT");
        std::vector<std::string> diffuseTexturePaths = {
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateRainforestTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateRainforestTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateRainforestTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateRainforestTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealForestTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealForestTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealForestTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealForestTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrasslandTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrasslandTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrasslandTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrasslandTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTundraTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTundraTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTundraTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTundraTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTropicalRainforestTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTropicalRainforestTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTropicalRainforestTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTropicalRainforestTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateForestTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateForestTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateForestTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateForestTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture1(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture2(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture3(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture4(), settings->getFilePathDelimitter(), "_diff"), // Ocean for now
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture4(), settings->getFilePathDelimitter(), "_diff"),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDesertTexture4(), settings->getFilePathDelimitter(), "_diff")
        };

    
        shared_ptr<TextureArray> diffuseTextureArray = make_shared<TextureArray>(
            diffuseTexturePaths,
            "texture_diffuse",
            "diffuseTextureArray"
        );

        diffuseTextureArray->loadTextureData();
        
        terrainTextureArrays.push_back(diffuseTextureArray);
        return;
    }

    // This cannot be performed in the background thread while the loading screen is active due to the calls to OpenGL which are not thread-safe. 
    if (!terrainTextureArrays.empty() && !terrainTextureArrays[0]->getUploaded()){
        // We need to iterate through the list of texture arrays, upload them to GPU and bind them in order
        for (int i = 0; i < static_cast<int> (terrainTextureArrays.size()); i++){
            terrainTextureArrays[i]->uploadToGPU();
            terrainTextureArrays[i]->bind(i + 1 + terrainTextures.size()); 
        }
        // Update the chunks to hold the new terrain texture arrays
        for (size_t i = 0; i < chunks.size(); i++){
            std::shared_ptr<Chunk> chunkPtr;
            {
                std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
                chunkPtr = chunks[i];
                chunkPtr->setTerrainTextureArrays(terrainTextureArrays);
            }
        }
    }

    // Update the skybox
    skyBox->updateData(regenerate);
    // Update the chunks
    updateLoadedChunks();
    for (size_t i = 0; i < chunks.size(); i++){
        std::shared_ptr<Chunk> chunkPtr;
        {
            std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
            chunkPtr = chunks[i];
        }
        // Update the chunk's subchunks
        chunkPtr->updateLoadedSubChunks(player->getPosition(), *settings);
    }
}

void World::updateLoadedChunks(){
    // We are going to check the neighbouring 5x5 chunks of the player to determine which chunks
    // need to be loaded or unloaded

    // We will start by checking all of the loaded chunks to see if they need to be removed
    std::vector<std::pair<int, int>> chunksToRemove;
    // We have to create these special scopes to ensure that the mutex is released after checking
    // all existing chunks
    {
        std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
        for (auto chunk : chunks){
            std::pair<int, int> chunkCoords = {
                chunk->getChunkCoords()[0],
                chunk->getChunkCoords()[1]
            };
            // Check if the chunk is within the request
            if (distanceToChunkCenter(chunkCoords) > settings->getRequestDistance()){
                chunksToRemove.push_back(chunkCoords);
            }
        }
    }
    // Remove the chunks that are too far away
    for (auto chunkCoords : chunksToRemove){
        removeChunk(chunkCoords.first, chunkCoords.second);
    }
    // Now we need to check the neighbouring chunks of the player to determine which chunks need
    // to be loaded as well
    std::pair<int, int> playerChunk = getPlayersCurrentChunk();
    std::vector<std::pair<int, int>> chunksToAdd;
    // Again a special scope to ensure that the mutex is released after checking neighbouring
    // chunks
    {
        std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
        for (int x = -2; x < 3; x++){
            for (int z = -2; z < 3; z++){
                std::pair<int, int> chunkCoords = {
                    playerChunk.first + x,
                    playerChunk.second + z
                };
                bool isFound;
                // Check if the chunk is already loaded or requested
                if (
                    isChunkRequested(chunkCoords.first, chunkCoords.second) ||
                    getChunk(chunkCoords.first, chunkCoords.second, isFound) != nullptr
                ){
                    continue;  // Skip this chunk as it is already loaded or requested
                }
                // Check if the chunk is within the request distance
                if (distanceToChunkCenter(chunkCoords) < settings->getRequestDistance()){
                    // Request the chunk to be loaded
                    chunksToAdd.push_back(chunkCoords);
                }
            }
        }
    }
    // For each chunk that needs to be added we will request it asyncronously
    for (auto chunkCoords : chunksToAdd){
        requestNewChunkAsync(chunkCoords.first, chunkCoords.second);
    }
}

std::pair<int, int> World::getPlayersCurrentChunk(){
    std::pair<int, int> chunkCoords = {
        static_cast<int>(floor(player->getPosition()[0] / settings->getChunkSize())),
        static_cast<int>(floor(player->getPosition()[2] / settings->getChunkSize()))
    };
    return chunkCoords;
}

float World::distanceToChunkCenter(std::pair<int, int> chunkCoords){
    // Get the world coordinates of the chunk
    std::pair<float, float> chunkWorldCoords = {
        static_cast<float>(chunkCoords.first) * settings->getChunkSize(),
        static_cast<float>(chunkCoords.second) * settings->getChunkSize()
    };
    // Get the distance between the players current position and the center of the chunk
    glm::vec3 playerPos = player->getPosition();

    float chunkMidX = chunkWorldCoords.first + settings->getChunkSize() / 2;
    float chunkMidZ = chunkWorldCoords.second + settings->getChunkSize() / 2;
    float distance = sqrt(pow(playerPos.x - chunkMidX, 2) + pow(playerPos.z - chunkMidZ, 2));
    return distance;
}


void World::clearChunks(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.clear();
    std::lock_guard<std::mutex> lock2(terrainTextureArraysMutex);  //Lock the guard to ensure safe access
    terrainTextureArrays.clear();
}

int World::getChunkCount(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    return chunks.size();
}

void World::addChunk(shared_ptr<Chunk> chunk){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.push_back(chunk);
}

void World::removeChunk(int cx, int cz){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.erase(std::remove_if(chunks.begin(), chunks.end(),
        [cx, cz](const std::shared_ptr<Chunk>& chunk) {
            return chunk->getChunkCoords()[0] == cx && chunk->getChunkCoords()[1] == cz;
        }), chunks.end()
    );
}

std::shared_ptr<Chunk> World::getChunk(int cx, int cz){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    for (const auto& chunk : chunks) {
        if (chunk->getChunkCoords()[0] == cx && chunk->getChunkCoords()[1] == cz) {
            return chunk;
        }
    }
    return nullptr;
}

// This is a guardless function to only be used when the mutex is already acquired within
// the calling function
std::shared_ptr<Chunk> World::getChunk(int cx, int cz, bool &found){
    for (const auto& chunk : chunks) {
        if (chunk->getChunkCoords()[0] == cx && chunk->getChunkCoords()[1] == cz) {
            found = true;
            return chunk;
        }
    }
    found = false;
    return nullptr;
}

bool World::isChunkRequested(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    for (const auto& request : chunkRequests) {
        if (request.first == cx && request.second == cz) {
            return true;
        }
    }
    return false;
}

void World::addChunkRequest(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    chunkRequests.push_back(std::make_pair(cx, cz));
}

void World::removeChunkRequest(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    chunkRequests.erase(std::remove_if(chunkRequests.begin(), chunkRequests.end(),
        [cx, cz](const std::pair<int, int>& request) {
            return request.first == cx && request.second == cz;
        }), chunkRequests.end()
    );
}

std::unique_ptr<PacketData> World::readPacketData(char *data, int len){
    std::unique_ptr<PacketData> packetData = make_unique<PacketData>();
    // We are going to iterate through the data that we have received and extract each field
    // in turn
    int index = 0;
    // Extract the seed
    packetData->seed = *reinterpret_cast<long*>(data + index);
#ifdef _WIN32
    index += sizeof(long) + sizeof(long);
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

void World::printRequests(){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    std::cout << "Current requests: ";
    for (const auto& request : chunkRequests) {
        std::cout << "(" << request.first << ", " << request.second << "), ";
    }
    std::cout << "\n";
}

void World::printChunks(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    std::cout << "Current chunks: ";
    for (const auto& chunk : chunks) {
        std::cout << "(" << chunk->getChunkCoords()[0] << ", " << chunk->getChunkCoords()[1] << "), ";
    }
    std::cout << "\n";
}

size_t World::writeCallback(void* contents, size_t size, size_t nmemb, void* userp){
    // This function will be called by libcurl to write the data received from the server
    // We are going to cast the userp to a PacketData object
    size_t totalSize = size * nmemb;
    auto *buffer = static_cast<PacketData*>(userp);
    // We are going to resize the buffer to the size of the data that we have received
    size_t oldSize = buffer->rawData.size();
    buffer->rawData.resize(oldSize + totalSize);
    // Copy the data to the buffer
    std::memcpy(buffer->rawData.data() + oldSize, contents, totalSize);
    // Return the size of the data that we have received
    return totalSize;
}

std::unique_ptr<PacketData> World::requestNewChunk(int cx, int cy){
    /*Create the JSON Request Object (This format needs to match the servers expected format)*/
    nlohmann::json payload = {
        {"mock_data", false},
        {"debug", false},
        /*
            Currently there is a restriction on the world generation that using np.random.seed
            will not allow a value greater than 2^32 - 1. This is a limitation of the numpy library
            and for this reason we are type casting all of our long seeds to uint32_t. If we find
            a solution to get around it then we can remove the static cast and use the long type.
        */
        {"seed", static_cast<uint32_t>(seed)}, //Temporarily we are statically casting it to a positive int
        {"cx", cx},
        {"cy", cy},
        {"biome", nullptr},
        {"debug", true},
        {"biome_size", settings->getParameters()->getBiomeSize()},
        {"ocean_coverage", settings->getParameters()->getOceanCoverage()},
        {"land_water_scale", 50},
        {"global_max_height", settings->getParameters()->getMaximumHeight()},
        {"temperate_rainforest", {
            {"max_height", 30}}},
        {"boreal_forest", {
            {"max_height", 40}}},
        {"grassland", {
            {"max_height", 40}}},
        {"tundra", {
            {"max_height", 50}}},
        {"savanna", {
            {"max_height", 25}}},
        {"woodland", {
            {"max_height", 40}}},
        {"tropical_rainforest", {
            {"max_height", 35}}},
        {"temperate_seasonal_forest", {
            {"max_height", 100}}},
        {"subtropical_desert", {
            {"max_height", 30}}},
    };

    CURL* curl;
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "ERROR: Failed to initialize curl" << std::endl;
        return nullptr;
    }
    curl = curl_easy_init();
    if (!curl) {
        std::cerr << "ERROR: Failed to initialize curl" << std::endl;
        return nullptr;
    }
    struct curl_slist* headers = nullptr;
    // Update header to explicitly specify UTF-8 encoding
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");

    //Setting the curl options
    curl_easy_setopt(curl, CURLoption::CURLOPT_URL, "http://localhost:8000/superchunk");
    curl_easy_setopt(curl, CURLoption::CURLOPT_POST, 1L);
    // Ensure the JSON payload is properly UTF-8 encoded
    std::string jsonPayload = payload.dump();
    curl_easy_setopt(curl, CURLoption::CURLOPT_POSTFIELDS, jsonPayload.c_str());
    curl_easy_setopt(curl, CURLoption::CURLOPT_POSTFIELDSIZE, payload.dump().size());
    // Adding a timeout to the request
    curl_easy_setopt(curl, CURLoption::CURLOPT_TIMEOUT, 120L);
    // Modifying the buffer to be a 50MB buffer
    curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, 1024 * 1024 * 50L); // 50MB buffer
    curl_easy_setopt(curl, CURLoption::CURLOPT_HTTPHEADER, headers);
    // curl_easy_setopt(curl, CURLoption::CURLOPT_VERBOSE, 1L);

    // Setting the write callback function
    std::unique_ptr<PacketData> packetData = std::make_unique<PacketData>();
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, World::writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, packetData.get());

    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "ERROR: Failed to perform curl request: " << curl_easy_strerror(res) << std::endl;
        return nullptr;
    } else {
        // Parse the response data
        packetData =std::move(readPacketData(packetData->rawData.data(), packetData->rawData.size()));
    }

    /*Debug output*/
    std::cout << "=========================== PACKET DATA ===========================" << std::endl;
    std::cout << "Seed: " << packetData->seed << std::endl;
    std::cout << "cx: " << packetData->cx << std::endl;
    std::cout << "cz: " << packetData->cz << std::endl;
    std::cout << "num_vertices: " << packetData->num_vertices << std::endl;
    std::cout << "vx: " << packetData->vx << std::endl;
    std::cout << "vz: " << packetData->vz << std::endl;
    std::cout << "size: " << packetData->size << std::endl;
    std::cout << "lenHeightmapData: " << packetData->lenHeightmapData << std::endl;
    std::cout << "biomeDataSize: " << packetData->biomeDataSize << std::endl;
    std::cout << "lenBiomeData: " << packetData->lenBiomeData << std::endl;
    // We want to print out the height values in index (0,0), (0,1), (1,0), (1,1) and (1024, 1024),
    // (1024, 1025), (1025, 1024), (1025, 1025)
    std::cout << "Heightmap data: " << std::endl;
    std::cout << "Index (0,0): " << packetData->heightmapData[0][0] << std::endl;
    std::cout << "Index (0,1): " << packetData->heightmapData[0][1] << std::endl;
    std::cout << "Index (1,0): " << packetData->heightmapData[1][0] << std::endl;
    std::cout << "Index (1,1): " << packetData->heightmapData[1][1] << std::endl;
    std::cout << "Index (1024, 1024): " << packetData->heightmapData[1024][1024] << std::endl;
    std::cout << "Index (1024, 1025): " << packetData->heightmapData[1024][1025] << std::endl;
    std::cout << "Index (1025, 1024): " << packetData->heightmapData[1025][1024] << std::endl;
    std::cout << "Index (1025, 1025): " << packetData->heightmapData[1025][1025] << std::endl;
    std::cout << "Index (0, 1024): " << packetData->heightmapData[0][1024] << std::endl;
    std::cout << "Index (0, 1025): " << packetData->heightmapData[0][1025] << std::endl;
    std::cout << "Index (1, 1024): " << packetData->heightmapData[1][1024] << std::endl;
    std::cout << "Index (1, 1025): " << packetData->heightmapData[1][1025] << std::endl;
    std::cout << "Index (1024, 0): " << packetData->heightmapData[1024][0] << std::endl;
    std::cout << "Index (1025, 0): " << packetData->heightmapData[1025][0] << std::endl;
    std::cout << "Index (1024, 1): " << packetData->heightmapData[1024][1] << std::endl;
    std::cout << "Index (1025, 1): " << packetData->heightmapData[1025][1] << std::endl;

    std::cout << "===================================================================" << std::endl;
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return packetData;
}

int World::requestInitialChunks(std::vector<std::pair<int, int>> initialChunks){
    //Launch the initial chunk requests asynchronously
    std::vector<std::future<std::unique_ptr<PacketData>>> futures;
    for (const auto& [cx, cz] : initialChunks) {
        addChunkRequest(cx, cz);
        futures.push_back(
            std::async(std::launch::async, &World::requestNewChunk, this, cx, cz)
        );
    }
    // We are going also going to track the requests that failed
    std::vector<std::pair<int, int>> failedRequests;
    std::vector<std::future<std::unique_ptr<PacketData>>> failedFutures;
    for (size_t i = 0; i < futures.size(); ++i) {
        auto packetData = futures[i].get();
        if (packetData == nullptr) {
            std::cerr << "ERROR: Failed to get packet data" << std::endl;
            // We are going to add the request to the failed requests
            failedRequests.push_back(initialChunks[i]);
            continue;
        }
        // If the request was successful, we are going to create the chunk
        std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(
            packetData->cx + packetData->cz * std::numeric_limits<int>::max(),
            settings,
            std::vector<int>{packetData->cx, packetData->cz},
            packetData->heightmapData,
            packetData->biomeData,
            terrainShader,
            oceanShader,
            terrainTextures,
            terrainTextureArrays,
            reflectionBuffer,
            refractionBuffer,
            oceanTextures
        );
        // We are going to add the chunk to the world
        addChunk(newChunk);
        // We are going to remove the request from the list of requests
        removeChunkRequest(packetData->cx, packetData->cz);
    }
    // We are going to try to request the failed requests again
    for (const auto& [cx, cz] : failedRequests) {
        std::cerr << "Retrying initial chunk request at (" << cx << ", " << cz << ")" << std::endl;
        addChunkRequest(cx, cz);
        failedFutures.push_back(
            std::async(std::launch::async, &World::requestNewChunk, this, cx, cz)
        );
    }
    // We are going to wait for the failed requests to finish
    for (size_t i = 0; i < failedFutures.size(); ++i) {
        auto packetData = failedFutures[i].get();
        if (packetData == nullptr) {
            std::cerr << "ERROR: Failed to get packet data" << std::endl;
            continue;
        }
        // If the request was successful, we are going to create the chunk
        std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(
            packetData->cx + packetData->cz * std::numeric_limits<int>::max(),
            settings,
            std::vector<int>{packetData->cx, packetData->cz},
            packetData->heightmapData,
            packetData->biomeData,
            terrainShader,
            oceanShader,
            terrainTextures,
            terrainTextureArrays,
            reflectionBuffer,
            refractionBuffer,
            oceanTextures
        );
        // We are going to add the chunk to the world
        addChunk(newChunk);
        // We are going to remove the request from the list of requests
        removeChunkRequest(packetData->cx, packetData->cz);
    }
    return 0;
}

int World::regenerateSpawnChunks(glm::vec3 playerPos){
    clearChunks();
    // We are going to request the 2x2 chunks around the player to be loaded
    int cx = static_cast<int>(floor(playerPos.x / settings->getChunkSize()));
    int cz = static_cast<int>(floor(playerPos.z / settings->getChunkSize()));
    std::vector<std::pair<int, int>> initialChunks;

    // float xOffset = fmod(0, settings->getChunkSize()); 
    // float zOffset = fmod(0, settings->getChunkSize()); 
    // This code does not work in its current form, possibly due to mismanagement of negative values. For now we will always spawn at (0, 80, 0)
    // if (xOffset < settings->getChunkSize() / 2 && zOffset < settings->getChunkSize() / 2){
    //     initialChunks = {
    //         {cx - 1, cz - 1},
    //         {cx, cz - 1},
    //         {cx - 1, cz},
    //         {cx, cz}
    //     };
    // } else if (xOffset < settings->getChunkSize() / 2 && zOffset >= settings->getChunkSize() / 2){
    //     initialChunks = {
    //         {cx - 1, cz},
    //         {cx, cz},
    //         {cx - 1, cz + 1},
    //         {cx, cz + 1}
    //     };
    // } else if (xOffset >= settings->getChunkSize() / 2 && zOffset < settings->getChunkSize() / 2){
    //     initialChunks = {
    //         {cx, cz - 1},
    //         {cx + 1, cz - 1},
    //         {cx, cz},
    //         {cx + 1, cz}
    //     };
    // } else {
    // initialChunks = {
    //     {cx, cz},
    //     {cx + 1, cz},
    //     {cx, cz + 1},
    //     {cx + 1, cz + 1}
    // };
    
    initialChunks = {
        {cx - 1, cz - 1},
        {cx, cz - 1},
        {cx - 1, cz},
        {cx, cz}
    };
    requestInitialChunks(initialChunks);
    return 0;
}

int World::requestNewChunkAsync(int cx, int cy){
    // Check to see if the chunk is already being requested
    if (isChunkRequested(cx, cy) || getChunk(cx, cy) != nullptr){
        std::cerr << "Chunk at (" << cx << ", " << cy << ") is already being requested or loaded." << std::endl;
        return 1;
    }
    // Add the chunk request to the list of requests
    addChunkRequest(cx, cy);
    // Request the chunk asynchronously
    std::future<std::unique_ptr<PacketData>> future = std::async(
        std::launch::async, &World::requestNewChunk, this, cx, cy
    );
    std::thread([this, future=std::move(future), cx, cy]() mutable {
        // Wait for the request to finish
        auto packetData = future.get();
        // Check that the request was successful
        if (packetData == nullptr){
            std::cerr << "ERROR: Failed to get packet data" << std::endl;
            // Remove the request from the list of requests
            removeChunkRequest(cx, cy);
            return;
        }
        // Create the new chunk
        std::shared_ptr<Chunk> newChunk = std::make_shared<Chunk>(
            packetData->cx + packetData->cz * std::numeric_limits<int>::max(),
            settings,
            std::vector<int>{packetData->cx, packetData->cz},
            packetData->heightmapData,
            packetData->biomeData,
            terrainShader,
            oceanShader,
            terrainTextures,
            terrainTextureArrays,
            reflectionBuffer,
            refractionBuffer,
            oceanTextures
        );
        // Add the chunk to the world
        addChunk(newChunk);
        // Remove the request from the list of requests
        removeChunkRequest(cx, cy);
    }).detach();
    return 0;
}
