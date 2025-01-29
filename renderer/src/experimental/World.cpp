/*
    This is the main class for the world which will hold all of the data that is required to be
    rendered in the world. This will include the loaded chunks, settings values, and the player.
*/

#include <vector>
#include <memory>
#include <ctime>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Chunk.hpp"
#include "Player.hpp"
#include "IRenderable.hpp"
#include "World.hpp"

using namespace std;

long World::generateRandomSeed(){
    // We are going to generate a seed for the random number generator using the current system time
    time_t currentTime = time(0);
    srand(currentTime);
    // Generate a random long seed by combining two calls to rand()
    int msbRandom = rand();
    int lsbRandom = rand();
    long seed = (static_cast<long>(msbRandom) << 32) | static_cast<long>(lsbRandom);
    return seed;
}

World::World(){
    seed =  generateRandomSeed();
    seaLevel = 0.2f;
    maxHeight = 192.0f;
    chunks = vector<shared_ptr<Chunk>>();
    player = make_shared<Player>();
}

World::World(long seed){
    this->seed = seed;
    seaLevel = 0.2f;
    maxHeight = 192.0f;
    chunks = vector<shared_ptr<Chunk>>();
    player = make_shared<Player>();
}

World::World(long seed, shared_ptr<Player> player){
    this->seed = seed;
    seaLevel = 0.2f;
    maxHeight = 192.0f;
    chunks = vector<shared_ptr<Chunk>>();
    this->player = player;
}

void World::addChunk(shared_ptr<Chunk> chunk){
    chunks.push_back(chunk);
}

void World::removeChunk(shared_ptr<Chunk> chunk){
    // Find the chunk in the vector and remove it
    for (auto it = chunks.begin(); it != chunks.end(); it++){
        if (*it == chunk){
            chunks.erase(it);
            return;
        }
    }
}

void World::render(glm::mat4 view, glm::mat4 projection){
    // Iterate through the chunks and render them
    for (shared_ptr<Chunk> chunk : chunks){
        chunk->render(view, projection);
    }
}

void World::setupData(){
    // Iterate through the chunks and set up their data
    for (shared_ptr<Chunk> chunk : chunks){
        chunk->setupData();
    }
}

void World::updateData(){
    // Iterate through the chunks and update their data
    for (shared_ptr<Chunk> chunk : chunks){
        chunk->updateData();
    }
}

void World::checkForNewChunks(){
    // We are going to check the player's position and see if we need to request any new chunks
    // from the world generation system.
    //
    // If the players position comes within 1.5 x the chunk size of the edge of the chunk then we
    // will request a new chunk from the world generation system.
    
    return;
}

void World::setUpInitialChunks(){
    // We are going to set up the initial chunks that will be loaded into the world. This will
    // include the chunk that the player is currently in and the surrounding chunks.
    // The starting chunks will be the 3x3 grid of chunks that the player is in.
    requestNewChunk({0, 0});            // (0, 0) Chunk
    requestNewChunk({1024, 0});         // (1, 0) Chunk
    requestNewChunk({-1024, 0});        // (-1, 0) Chunk
    requestNewChunk({0, 1024});         // (0, 1) Chunk
    requestNewChunk({0, -1024});        // (0, -1) Chunk
    requestNewChunk({1024, 1024});      // (1, 1) Chunk
    requestNewChunk({1024, -1024});     // (1, -1) Chunk
    requestNewChunk({-1024, 1024});     // (-1, 1) Chunk
    requestNewChunk({-1024, -1024});    // (-1, -1) Chunk
    return;
}

void World::requestNewChunk(vector<int> chunkCoords){
    // TODO write this function with the interaction with the world generation system
    return;
}