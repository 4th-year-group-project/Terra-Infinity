/**
 * @file World.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the World class.
 * @details This class is responsible for rendering the world and managing the chunks of the world.
 * It handles the loading and unloading of chunks, as well as the rendering of the terrain and ocean.
 * It also handles the rendering of the skybox and the water reflection and refraction.
 * @version 1.0
 * @date 2025
 * 
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

/**
 * @brief Construct a new World object with the given parameters
 * 
 * @details This constructor will create a world object with the given parameters. It will set up
 * the world with the given parameters and create the skybox, terrain shader, ocean shader, and
 * the noise texture. It will also create the ocean textures and bind them to the shader.
 * 
 * 
 * 
 * @param settings [in] std::shared_ptr<Settings> The settings object
 * @param player [in] std::shared_ptr<Player> The player object
 * @param inReflectionBuffer [in] std::shared_ptr<WaterFrameBuffer> The reflection buffer
 * @param inRefractionBuffer [in] std::shared_ptr<WaterFrameBuffer> The refraction buffer
 * 
 */
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
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "right.png"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "left.png"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "top.png"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "bottom.png"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "front.png"),
        (textureRoot + settings->getFilePathDelimitter() + "skybox" + settings->getFilePathDelimitter() + "back.png"),
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
/**
 * @brief This function will render the world
 * 
 * @details This function will render the world by rendering the skybox and then rendering each chunk
 * in the world. We have to ensure that the chunks are rendered in a thread safe manner.
 * 
 * @param view [in] glm::mat4 The view matrix
 * @param projection [in] glm::mat4 The projection matrix
 * @param lights [in] std::vector<std::shared_ptr<Light>> The lights in the world
 * @param viewPos [in] glm::vec3 The position of the camera
 * @param isWaterPass [in] bool Whether or not this is a water pass
 * @param isShadowPass [in] bool Whether or not this is a shadow pass
 * @param plane [in] glm::vec4 The plane for the water reflection
 * 
 * @return void
 * 
 */
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

/**
 * @brief This function will set up the data for the world
 * 
 * @details There is no data to set up for the world, as each component of the world is set
 * up within its constructor. 
 * 
 * @return void
 * 
 */
void World::setupData(){
    // Do nothing
}

/**
 * @brief This function will update the data for the world
 * 
 * @details This function will update the data for the world, by checking which chunks need to be
 * loaded and which chunks need to be unloaded. It will also check each of the chunks to determine
 * if their loaded subchunks need to be updated. 
 * 
 * @param regenerate [in] bool Whether to regenerate the world or not
 * 
 * @return void
 * 
 */
void World::updateData(bool regenerate){
    // Check if the world needs to be regenerated
    // This is blocking the main thread
    if (regenerate){
        player->setPosition(glm::vec3(0.0f, 80.0f, 0.0f)); // Player should always spawn at the origin
        player->getCamera()->setPosition(glm::vec3(0.0f, 80.0f, 0.0f) + glm::vec3(1.68f, 0.2f, 0.2f));

        // Regenerate the spawn chunks
        seed = settings->getParameters()->getSeed();
        regenerateSpawnChunks(player->getPosition());

        vector<string> diffTextureNames = {"_diff", "_Color","_color","_COLOR","_albedo"}; // A vector to hold common names for diffuse textures

        // Load texture array images into memory here
        string textureRoot = getenv("TEXTURE_ROOT");
        std::vector<std::string> diffuseTexturePaths = {
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBorealTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyStoneTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyStoneTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyStoneTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getGrassyStoneTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSnowyTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSnowyTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSnowyTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSnowyTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getIcyTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getIcyTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getIcyTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getIcyTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSavannaTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getWoodlandTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleMountainsTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleMountainsTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleMountainsTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getJungleMountainsTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getVolcanicTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getVolcanicTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getVolcanicTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getVolcanicTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getTemperateTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSwampTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSwampTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSwampTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSwampTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSeasonalForestTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSeasonalForestTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSeasonalForestTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getSeasonalForestTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getAutumnTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getAutumnTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getAutumnTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getAutumnTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getMesaTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getMesaTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getMesaTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getMesaTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getHotDesertTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getHotDesertTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getHotDesertTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getHotDesertTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDustyTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDustyTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDustyTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getDustyTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBadlandsTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBadlandsTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBadlandsTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getBadlandsTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOasisTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOasisTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOasisTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOasisTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOceanTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOceanTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOceanTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getOceanTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getCliffsTextureLow(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getCliffsTextureMidFlat(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getCliffsTextureMidSteep(), settings->getFilePathDelimitter(), diffTextureNames),
            settings->getParameters()->findTextureFilePath(settings->getParameters()->getCliffsTextureHigh(), settings->getFilePathDelimitter(), diffTextureNames)
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

/**
 * @brief This function will update the loaded chunks within the world
 * 
 * @details This function will check the loaded chunks and determine which chunks need to be
 * loaded and which chunks need to be unloaded. It will check the neighbouring chunks of the player
 * and determine if they are within the request distance. If they are, it will request them to be
 * loaded. If they are not, it will remove them from the loaded chunks.
 * 
 * @return void
 * 
 */ 
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

/**
 * @brief This function determines the chunk coordinates that the player is currently in
 * 
 * @returns std::pair<int, int> The chunk coordinates of the player
 */ 
std::pair<int, int> World::getPlayersCurrentChunk(){
    std::pair<int, int> chunkCoords = {
        static_cast<int>(floor(player->getPosition()[0] / settings->getChunkSize())),
        static_cast<int>(floor(player->getPosition()[2] / settings->getChunkSize()))
    };
    return chunkCoords;
}

/**
 * @brief This function will compute the distance to the chunk center from the current player
 * position
 * 
 * @details This function will compute the distance to the chunk center from the current player
 * position. It will use the chunk coordinates to determine the world coordinates of the chunk
 * and then compute the distance between the player and the chunk center.
 * 
 * @param chunkCoords [in] std::pair<int, int> The chunk coordinates
 * 
 * @return float The distance to the chunk center
 * 
 */ 
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

/**
 * @brief This function will clear the loaded chunks from the world
 * 
 * @details This function will clear the loaded chunks in a thread safe manner.
 * 
 * @return void
 * 
 */
void World::clearChunks(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.clear();
    std::lock_guard<std::mutex> lock2(terrainTextureArraysMutex);  //Lock the guard to ensure safe access
    terrainTextureArrays.clear();
}

/**
 * @brief This function will get the number of currently loaded chunks in the world
 * 
 * @details This is a thread safe function that will return the number of chunks in the world.
 * 
 * @return int The number of chunks in the world
 * 
 */
int World::getChunkCount(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    return chunks.size();
}

/**
 * @brief This function will add a chunk to the world
 * 
 * @details This function will add a chunk to the world in a thread safe manner.
 * 
 * @param chunk [in] std::shared_ptr<Chunk> The chunk to add
 * 
 * @return void
 * 
 */
void World::addChunk(shared_ptr<Chunk> chunk){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.push_back(chunk);
}

/**
 * @brief This function will remove a chunk from the world
 * 
 * @details This function will remove a chunk from the world in a thread safe manner.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return void
 * 
 */
void World::removeChunk(int cx, int cz){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    chunks.erase(std::remove_if(chunks.begin(), chunks.end(),
        [cx, cz](const std::shared_ptr<Chunk>& chunk) {
            return chunk->getChunkCoords()[0] == cx && chunk->getChunkCoords()[1] == cz;
        }), chunks.end()
    );
}

/**
 * @brief This function will get a chunk from the world
 * 
 * @details This function will get a chunk from the world in a thread safe manner.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return std::shared_ptr<Chunk> The chunk
 * 
 */
std::shared_ptr<Chunk> World::getChunk(int cx, int cz){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    for (const auto& chunk : chunks) {
        if (chunk->getChunkCoords()[0] == cx && chunk->getChunkCoords()[1] == cz) {
            return chunk;
        }
    }
    return nullptr;
}

/**
 * @brief This function will get a chunk from the world
 * 
 * @details This function will get a chunk from the world in a gaurdless manner. This must
 * only be used when the mutex is already acquired within the calling function.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * @param found [out] bool Whether or not the chunk was found
 * 
 * @return std::shared_ptr<Chunk> The chunk
 * 
 */
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

/**
 * @brief This function will check if a chunk is requested
 * 
 * @details This function will check if a chunk is requested in a thread safe manner.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return bool Whether or not the chunk is requested
 * 
 */
bool World::isChunkRequested(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    for (const auto& request : chunkRequests) {
        if (request.first == cx && request.second == cz) {
            return true;
        }
    }
    return false;
}

/**
 * @brief This function will add a chunk request
 * 
 * @details This function will add a chunk request in a thread safe manner.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return void
 * 
 */
void World::addChunkRequest(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    chunkRequests.push_back(std::make_pair(cx, cz));
}

/**
 * @brief This function will remove a chunk request
 * 
 * @details This function will remove a chunk request in a thread safe manner.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return void
 * 
 */
void World::removeChunkRequest(int cx, int cz){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    chunkRequests.erase(std::remove_if(chunkRequests.begin(), chunkRequests.end(),
        [cx, cz](const std::pair<int, int>& request) {
            return request.first == cx && request.second == cz;
        }), chunkRequests.end()
    );
}

/**
 * @brief This function will read the packet data from the server
 * 
 * @details This function will read the packet data aggregated across all incoming packets for
 * a specific request and return the processed packet data. It will extract the seed, chunk 
 * coordinates, number of vertices, size of the heightmap data, length of the heightmap data,
 * length of the biome data, number of trees, and the coordinates of the trees. It will also
 * check if the length of the heightmap data is correct and if the length of the data matches
 * the expected length. If the length of the data does not match, it will return nullptr.
 * 
 * @param data [in] char* The data received from the server
 * @param len [in] int The length of the data received from the server
 * 
 * @return std::unique_ptr<PacketData> The packet data
 * @return nullptr if the length of the data does not match
 * 
 */
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
    packetData->treesSize = *reinterpret_cast<int*>(data + index);
    index += sizeof(int);
    packetData->treesCount = *reinterpret_cast<uint32_t*>(data + index);
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
    // Extract the trees data
    /*
        We know that there is packetData->treesCount number of values 
        Each two values form a pair of coordinates (x, z) for the tree
    */
    for (int i = 0; i < packetData->treesCount; i+=2){
        // We know that each coordinate is 16 bits long
        float x = *reinterpret_cast<float*>(data + index);
        index += sizeof(float);
        float z = *reinterpret_cast<float*>(data + index);
        index += sizeof(float);
        packetData->treesCoords.push_back(std::make_pair(x, z));
    }

    // Ensure that we have read all the data
    if (index != len){
        return nullptr;
        cerr << "ERROR: The length of the data does not match the expected length" << endl;
    }
    return packetData;
}

/**
 * @brief This function will print the current requests in the world
 * 
 * @details This function will print the current requests in the world in a thread safe manner.
 * 
 * @return void
 * 
 */
void World::printRequests(){
    std::lock_guard<std::mutex> lock(requestMutex);  //Lock the guard to ensure safe access
    std::cout << "Current requests: ";
    for (const auto& request : chunkRequests) {
        std::cout << "(" << request.first << ", " << request.second << "), ";
    }
    std::cout << "\n";
}

/**
 * @brief This function will print the current chunks in the world
 * 
 * @details This function will print the current chunks in the world in a thread safe manner.
 * 
 * @return void
 * 
 */
void World::printChunks(){
    std::lock_guard<std::mutex> lock(chunkMutex);  //Lock the guard to ensure safe access
    std::cout << "Current chunks: ";
    for (const auto& chunk : chunks) {
        std::cout << "(" << chunk->getChunkCoords()[0] << ", " << chunk->getChunkCoords()[1] << "), ";
    }
    std::cout << "\n";
}

/**
 * @brief This callback function will be called by libcurl when packet data is received
 * 
 * @details This function will be called by libcurl when packet data is received. It will
 * aggregate the data into a buffer for each request to be used in subsequent processing.
 * 
 * @param contents [in] void* The data received from the server
 * @param size [in] size_t The size of the data received
 * @param nmemb [in] size_t The number of elements received
 * @param userp [out] void* The user pointer to the buffer
 * @return size_t The size of the data received
 */
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

/**
 * @brief This function will request a new chunk from the server
 * 
 * @details This function will request a new chunk from the server. It will create a JSON
 * object with the parameters and send it to the server. It will also set the request to be
 * asynchronous.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return std::unique_ptr<PacketData> The packet data received from the server
 * 
 */
std::unique_ptr<PacketData> World::requestNewChunk(int cx, int cz){
    /*Create the JSON Request Object (This format needs to match the servers expected format)*/
    nlohmann::json payload = {
        {"mock_data", false},
        {"seed", settings->getParameters()->getSeed()},
        {"cx", cx},
        {"cy", cz},
        {"global_max_height", settings->getParameters()->getGlobalMaxHeight()},
        {"global_tree_density", settings->getParameters()->getGlobalTreeDensity()},
        {"ocean_coverage", settings->getParameters()->getOceanCoverage()},
        {"continent_size", settings->getParameters()->getContinentSize()},
        {"biome_size", settings->getParameters()->getBiomeSize()},
        {"warmth", settings->getParameters()->getWarmth()},
        {"wetness", settings->getParameters()->getWetness()},
        {"river_frequency", settings->getParameters()->getRiverFrequency()},
        {"river_width", settings->getParameters()->getRiverWidth()},
        {"river_depth", settings->getParameters()->getRiverDepth()},
        {"river_meanderiness", settings->getParameters()->getRiverMeandering()},
        {"debug", false},
        {"global_ruggedness", settings->getParameters()->getGlobalRuggedness()},
        {"boreal_forest", {
            {"selected", settings->getParameters()->getBorealForestSelected()},
            {"plains", {
                {"max_height", settings->getParameters()->getBorealForestPlainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getBorealForestPlainsOccurrenceProbability()},
                {"evenness", settings->getParameters()->getBorealForestPlainsEvenness()},
                {"tree_density", settings->getParameters()->getBorealForestPlainsTreeDensity()}
            }},
            {"hills", {
                {"max_height", settings->getParameters()->getBorealForestHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getBorealForestHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getBorealForestHillsBumpiness()},
                {"tree_density", settings->getParameters()->getBorealForestHillsTreeDensity()}
            }},
            {"mountains", {
                {"max_height", settings->getParameters()->getBorealForestMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getBorealForestMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getBorealForestMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getBorealForestMountainsTreeDensity()}
            }}
        }},
    
        {"grassland", {
            {"selected", settings->getParameters()->getGrasslandSelected()},
            {"plains", {
                {"max_height", settings->getParameters()->getGrasslandPlainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getGrasslandPlainsOccurrenceProbability()},
                {"evenness", settings->getParameters()->getGrasslandPlainsEvenness()},
                {"tree_density", settings->getParameters()->getGrasslandPlainsTreeDensity()}
            }},
            {"hills", {
                {"max_height", settings->getParameters()->getGrasslandHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getGrasslandHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getGrasslandHillsBumpiness()},
                {"tree_density", settings->getParameters()->getGrasslandHillsTreeDensity()}
            }},
            {"rocky_fields", {
                {"max_height", settings->getParameters()->getGrasslandRockyFieldsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getGrasslandRockyFieldsOccurrenceProbability()},
                {"rockiness", settings->getParameters()->getGrasslandRockyFieldsRockiness()},
                {"tree_density", settings->getParameters()->getGrasslandRockyFieldsTreeDensity()}
            }},
            {"terraced_fields", {
                {"max_height", settings->getParameters()->getGrasslandTerracedFieldsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getGrasslandTerracedFieldsOccurrenceProbability()},
                {"size", settings->getParameters()->getGrasslandTerracedFieldsSize()},
                {"tree_density", settings->getParameters()->getGrasslandTerracedFieldsTreeDensity()},
                {"smoothness", settings->getParameters()->getGrasslandTerracedFieldsSmoothness()},
                {"number_of_terraces", settings->getParameters()->getGrasslandTerracedFieldsNumberOfTerraces()}
            }}
        }},
    
        {"tundra", {
            {"selected", settings->getParameters()->getTundraSelected()},
            {"plains", {
                {"max_height", settings->getParameters()->getTundraPlainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTundraPlainsOccurrenceProbability()},
                {"evenness", settings->getParameters()->getTundraPlainsEvenness()},
                {"tree_density", settings->getParameters()->getTundraPlainsTreeDensity()}
            }},
            {"blunt_mountains", {
                {"max_height", settings->getParameters()->getTundraBluntMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTundraBluntMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getTundraBluntMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getTundraBluntMountainsTreeDensity()}
            }},
            {"pointy_mountains", {
                {"max_height", settings->getParameters()->getTundraPointyMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTundraPointyMountainsOccurrenceProbability()},
                {"steepness", settings->getParameters()->getTundraPointyMountainsSteepness()},
                {"frequency", settings->getParameters()->getTundraPointyMountainsFrequency()},
                {"tree_density", settings->getParameters()->getTundraPointyMountainsTreeDensity()}
            }}
        }},
    
        {"savanna", {
            {"selected", settings->getParameters()->getSavannaSelected()},
            {"plains", {
                {"max_height", settings->getParameters()->getSavannaPlainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSavannaPlainsOccurrenceProbability()},
                {"evenness", settings->getParameters()->getSavannaPlainsEvenness()},
                {"tree_density", settings->getParameters()->getSavannaPlainsTreeDensity()}
            }},
            {"mountains", {
                {"max_height", settings->getParameters()->getSavannaMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSavannaMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getSavannaMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getSavannaMountainsTreeDensity()}
            }}
        }},
    
        {"woodland", {
            {"selected", settings->getParameters()->getWoodlandSelected()},
            {"hills", {
                {"max_height", settings->getParameters()->getWoodlandHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getWoodlandHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getWoodlandHillsBumpiness()},
                {"tree_density", settings->getParameters()->getWoodlandHillsTreeDensity()}
            }}
        }},
    
        {"tropical_rainforest", {
            {"selected", settings->getParameters()->getTropicalRainforestSelected()},
            {"plains", {
                {"max_height", settings->getParameters()->getTropicalRainforestPlainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTropicalRainforestPlainsOccurrenceProbability()},
                {"evenness", settings->getParameters()->getTropicalRainforestPlainsEvenness()},
                {"tree_density", settings->getParameters()->getTropicalRainforestPlainsTreeDensity()}
            }},
            {"mountains", {
                {"max_height", settings->getParameters()->getTropicalRainforestMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTropicalRainforestMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getTropicalRainforestMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getTropicalRainforestMountainsTreeDensity()}
            }},
            {"hills", {
                {"max_height", settings->getParameters()->getTropicalRainforestHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTropicalRainforestHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getTropicalRainforestHillsBumpiness()},
                {"tree_density", settings->getParameters()->getTropicalRainforestHillsTreeDensity()}
            }},
            {"volcanoes", {
                {"max_height", settings->getParameters()->getTropicalRainforestVolcanoesMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTropicalRainforestVolcanoesOccurrenceProbability()},
                {"size", settings->getParameters()->getTropicalRainforestVolcanoesSize()},
                {"tree_density", settings->getParameters()->getTropicalRainforestVolcanoesTreeDensity()},
                {"thickness", settings->getParameters()->getTropicalRainforestVolcanoesThickness()},
                {"density", settings->getParameters()->getTropicalRainforestVolcanoesDensity()}
            }}
        }},
    
        {"temperate_rainforest", {
            {"selected", settings->getParameters()->getTemperateRainforestSelected()},
            {"hills", {
                {"max_height", settings->getParameters()->getTemperateRainforestHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTemperateRainforestHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getTemperateRainforestHillsBumpiness()},
                {"tree_density", settings->getParameters()->getTemperateRainforestHillsTreeDensity()}
            }},
            {"mountains", {
                {"max_height", settings->getParameters()->getTemperateRainforestMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTemperateRainforestMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getTemperateRainforestMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getTemperateRainforestMountainsTreeDensity()}
            }},
            {"swamp", {
                {"max_height", settings->getParameters()->getTemperateRainforestSwampMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTemperateRainforestSwampOccurrenceProbability()},
                {"wetness", settings->getParameters()->getTemperateRainforestSwampWetness()},
                {"tree_density", settings->getParameters()->getTemperateRainforestSwampTreeDensity()}
            }}
        }},
    
        {"temperate_seasonal_forest", {
            {"selected", settings->getParameters()->getTemperateSeasonalForestSelected()},
            {"hills", {
                {"max_height", settings->getParameters()->getTemperateSeasonalForestHillsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTemperateSeasonalForestHillsOccurrenceProbability()},
                {"bumpiness", settings->getParameters()->getTemperateSeasonalForestHillsBumpiness()},
                {"tree_density", settings->getParameters()->getTemperateSeasonalForestHillsTreeDensity()},
                {"autumnal_occurrence", settings->getParameters()->getTemperateSeasonalForestHillsAutumnalOccurrence()}
            }},
            {"mountains", {
                {"max_height", settings->getParameters()->getTemperateSeasonalForestMountainsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getTemperateSeasonalForestMountainsOccurrenceProbability()},
                {"ruggedness", settings->getParameters()->getTemperateSeasonalForestMountainsRuggedness()},
                {"tree_density", settings->getParameters()->getTemperateSeasonalForestMountainsTreeDensity()},
                {"autumnal_occurrence", settings->getParameters()->getTemperateSeasonalForestMountainsAutumnalOccurrence()}
            }}
        }},
    
        {"subtropical_desert", {
            {"selected", settings->getParameters()->getSubtropicalDesertSelected()},
            {"dunes", {
                {"max_height", settings->getParameters()->getSubtropicalDesertDunesMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSubtropicalDesertDunesOccurrenceProbability()},
                {"size", settings->getParameters()->getSubtropicalDesertDunesSize()},
                {"tree_density", settings->getParameters()->getSubtropicalDesertDunesTreeDensity()},
                {"dune_frequency", settings->getParameters()->getSubtropicalDesertDunesDuneFrequency()},
                {"dune_waviness", settings->getParameters()->getSubtropicalDesertDunesDuneWaviness()},
                {"bumpiness", settings->getParameters()->getSubtropicalDesertDunesBumpiness()}
            }},
            {"mesas", {
                {"max_height", settings->getParameters()->getSubtropicalDesertMesasMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSubtropicalDesertMesasOccurrenceProbability()},
                {"size", settings->getParameters()->getSubtropicalDesertMesasSize()},
                {"tree_density", settings->getParameters()->getSubtropicalDesertMesasTreeDensity()},
                {"number_of_terraces", settings->getParameters()->getSubtropicalDesertMesasNumberOfTerraces()},
                {"steepness", settings->getParameters()->getSubtropicalDesertMesasSteepness()}
            }},
            {"ravines", {
                {"max_height", settings->getParameters()->getSubtropicalDesertRavinesMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSubtropicalDesertRavinesOccurrenceProbability()},
                {"density", settings->getParameters()->getSubtropicalDesertRavinesDensity()},
                {"tree_density", settings->getParameters()->getSubtropicalDesertRavinesTreeDensity()},
                {"ravine_width", settings->getParameters()->getSubtropicalDesertRavinesRavineWidth()},
                {"smoothness", settings->getParameters()->getSubtropicalDesertRavinesSmoothness()},
                {"steepness", settings->getParameters()->getSubtropicalDesertRavinesSteepness()}
            }},
            {"oasis", {
                {"max_height", settings->getParameters()->getSubtropicalDesertOasisMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSubtropicalDesertOasisOccurrenceProbability()},
                {"size", settings->getParameters()->getSubtropicalDesertOasisSize()},
                {"flatness", settings->getParameters()->getSubtropicalDesertOasisFlatness()},
                {"tree_density", settings->getParameters()->getSubtropicalDesertOasisTreeDensity()},
                {"dune_frequency", settings->getParameters()->getSubtropicalDesertOasisDuneFrequency()}
            }},
            {"cracked", {
                {"max_height", settings->getParameters()->getSubtropicalDesertCrackedMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getSubtropicalDesertCrackedOccurrenceProbability()},
                {"size", settings->getParameters()->getSubtropicalDesertCrackedSize()},
                {"flatness", settings->getParameters()->getSubtropicalDesertCrackedFlatness()},
                {"tree_density", settings->getParameters()->getSubtropicalDesertCrackedTreeDensity()}
            }}
        }},
    
        {"ocean", {
            {"selected", settings->getParameters()->getOceanSelected()},
            {"flat_seabed", {
                {"max_height", settings->getParameters()->getOceanFlatSeabedMaxHeight()},
                {"evenness", settings->getParameters()->getOceanFlatSeabedEvenness()},
                {"occurrence_probability", settings->getParameters()->getOceanFlatSeabedOccurrenceProbability()}
            }},
            {"volcanic_islands", {
                {"max_height", settings->getParameters()->getOceanVolcanicIslandsMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getOceanVolcanicIslandsOccurrenceProbability()},
                {"size", settings->getParameters()->getOceanVolcanicIslandsSize()},
                {"thickness", settings->getParameters()->getOceanVolcanicIslandsThickness()},
                {"density", settings->getParameters()->getOceanVolcanicIslandsDensity()}
            }},
            {"water_stacks", {
                {"max_height", settings->getParameters()->getOceanWaterStacksMaxHeight()},
                {"occurrence_probability", settings->getParameters()->getOceanWaterStacksOccurrenceProbability()},
                {"size", settings->getParameters()->getOceanWaterStacksSize()}
            }},
            {"trenches", {
                {"max_height", settings->getParameters()->getOceanTrenchesMaxHeight()},
                {"density", settings->getParameters()->getOceanTrenchesDensity()},
                {"occurrence_probability", settings->getParameters()->getOceanTrenchesOccurrenceProbability()},
                {"trench_width", settings->getParameters()->getOceanTrenchesTrenchWidth()},
                {"smoothness", settings->getParameters()->getOceanTrenchesSmoothness()}
            }}
        }}
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
    // std::cout << "=========================== PACKET DATA ===========================" << std::endl;
    // std::cout << "Seed: " << packetData->seed << std::endl;
    // std::cout << "cx: " << packetData->cx << std::endl;
    // std::cout << "cz: " << packetData->cz << std::endl;
    // std::cout << "num_vertices: " << packetData->num_vertices << std::endl;
    // std::cout << "vx: " << packetData->vx << std::endl;
    // std::cout << "vz: " << packetData->vz << std::endl;
    // std::cout << "size: " << packetData->size << std::endl;
    // std::cout << "lenHeightmapData: " << packetData->lenHeightmapData << std::endl;
    // std::cout << "biomeDataSize: " << packetData->biomeDataSize << std::endl;
    // std::cout << "lenBiomeData: " << packetData->lenBiomeData << std::endl;
    // std::cout << "treesSize: " << packetData->treesSize << std::endl;
    // std::cout << "treesCount: " << packetData->treesCount << std::endl;
    // // We want to print out the height values in index (0,0), (0,1), (1,0), (1,1) and (1024, 1024),
    // // (1024, 1025), (1025, 1024), (1025, 1025)
    // std::cout << "Heightmap data: " << std::endl;
    // std::cout << "Index (0,0): " << packetData->heightmapData[0][0] << std::endl;
    // std::cout << "Index (0,1): " << packetData->heightmapData[0][1] << std::endl;
    // std::cout << "Index (1,0): " << packetData->heightmapData[1][0] << std::endl;
    // std::cout << "Index (1,1): " << packetData->heightmapData[1][1] << std::endl;
    // std::cout << "Index (1024, 1024): " << packetData->heightmapData[1024][1024] << std::endl;
    // std::cout << "Index (1024, 1025): " << packetData->heightmapData[1024][1025] << std::endl;
    // std::cout << "Index (1025, 1024): " << packetData->heightmapData[1025][1024] << std::endl;
    // std::cout << "Index (1025, 1025): " << packetData->heightmapData[1025][1025] << std::endl;
    // std::cout << "Index (0, 1024): " << packetData->heightmapData[0][1024] << std::endl;
    // std::cout << "Index (0, 1025): " << packetData->heightmapData[0][1025] << std::endl;
    // std::cout << "Index (1, 1024): " << packetData->heightmapData[1][1024] << std::endl;
    // std::cout << "Index (1, 1025): " << packetData->heightmapData[1][1025] << std::endl;
    // std::cout << "Index (1024, 0): " << packetData->heightmapData[1024][0] << std::endl;
    // std::cout << "Index (1025, 0): " << packetData->heightmapData[1025][0] << std::endl;
    // std::cout << "Index (1024, 1): " << packetData->heightmapData[1024][1] << std::endl;
    // std::cout << "Index (1025, 1): " << packetData->heightmapData[1025][1] << std::endl;

    // std::cout << "===================================================================" << std::endl;
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    return packetData;
}

/**
 * @brief This function will request the initial chunks to be loaded
 * 
 * @details This function will request the initial chunks to be loaded. It will launch the requests
 * asynchronously and wait for them to finish. It will also retry the requests that failed.
 * This ensures that the four initial chunks are all received before any rendering is done.
 * 
 * @param initialChunks [in] std::vector<std::pair<int, int>> The initial chunks to be loaded
 * 
 * @return int 0 if successful, -1 if failed
 * 
 */
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
            oceanTextures,
            subbiomeTextureArrayMap
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
            oceanTextures,
            subbiomeTextureArrayMap
        );
        // We are going to add the chunk to the world
        addChunk(newChunk);
        // We are going to remove the request from the list of requests
        removeChunkRequest(packetData->cx, packetData->cz);
    }
    return 0;
}

/**
 * @brief This function will regenerate the spawn chunks
 * 
 * @details This function will regenerate the spawn chunks. It will clear the chunks and request
 * the initial chunks to be loaded. It will also set the player position to (0, 80, 0) for now.
 * 
 * @param playerPos [in] glm::vec3 The player position
 * 
 * @return int 0 if successful, -1 if failed
 * 
 */
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
    // We are going to need to set the players position to the height of the vertex at chunk (0,0)
    // and coordinate (0,0)
    // Acquire the chunk lock
    {
        std::unique_lock<std::mutex> lock(chunkMutex);
        float newHeight = 0.0f;
        for (auto& chunk : chunks) {
            if (chunk->getChunkCoords()[0] == 0 && chunk->getChunkCoords()[1] == 0) {
                newHeight = chunk->getHeightmapData()[1][1] * settings->getMaximumHeight();
                break;
            }
        }
        // Ensures the player does not spawn below sea level
        newHeight = std::max(newHeight, settings->getMaximumHeight()* settings->getSeaLevel());
        // Set the player position to the new height and the camera position to the new height
        player->setPosition(glm::vec3(0.0f, newHeight, 0.0f));
        player->getCamera()->setPosition(glm::vec3(1.68f, newHeight + 10.0f, 0.2f));
    }
    return 0;
}

/**
 * @brief This function will request a new chunk asynchronously
 * 
 * @details This function will request a new chunk asynchronously. It will check to see if the
 * chunk is already being requested or loaded. If it is, it will return an error. If it is not,
 * it will add the chunk to the list of requests and request the chunk asynchronously.
 * 
 * @note The thread will be detached, so it will run in the background.
 * 
 * @param cx [in] int The chunk x coordinate
 * @param cz [in] int The chunk z coordinate
 * 
 * @return int 0 if successful, 1 if failed
 * 
 */
int World::requestNewChunkAsync(int cx, int cz){
    // Check to see if the chunk is already being requested
    if (isChunkRequested(cx, cz) || getChunk(cx, cz) != nullptr){
        std::cerr << "Chunk at (" << cx << ", " << cz << ") is already being requested or loaded." << std::endl;
        return 1;
    }
    // Add the chunk request to the list of requests
    addChunkRequest(cx, cz);
    // Request the chunk asynchronously
    std::future<std::unique_ptr<PacketData>> future = std::async(
        std::launch::async, &World::requestNewChunk, this, cx, cz
    );
    std::thread([this, future=std::move(future), cx, cz]() mutable {
        // Wait for the request to finish
        auto packetData = future.get();
        // Check that the request was successful
        if (packetData == nullptr){
            std::cerr << "ERROR: Failed to get packet data" << std::endl;
            // Remove the request from the list of requests
            removeChunkRequest(cx, cz);
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
            oceanTextures,
            subbiomeTextureArrayMap
        );
        // Add the chunk to the world
        addChunk(newChunk);
        // Remove the request from the list of requests
        removeChunkRequest(cx, cz);
    }).detach();
    return 0;
}
