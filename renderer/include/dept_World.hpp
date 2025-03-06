#ifndef WORLD_HPP
#define WORLD_HPP
/*
    This is the main class for the world which will hold all of the data that is required to be
    rendered in the world. This will include the loaded chunks, settings values, and the player.
*/

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Chunk.hpp"
#include "Player.hpp"
#include "IRenderable.hpp"

using namespace std;


class World : public IRenderable {
private:
    long seed; // The seed for the world
    vector<shared_ptr<Chunk>> chunks; // The chunks that are loaded in the world
    shared_ptr<Player> player; // The player in the world
    // Sun sun; // The sun in the world
    // Moon moon; // The moon in the world
    // SkyBox skyBox; // The skybox in the world
    float seaLevel; // The sea level of the world
    float maxHeight; // The maximum height of the world

    long generateRandomSeed();
public:
    World(
        long seed,
        vector<shared_ptr<Chunk>> chunks,
        shared_ptr<Player> player,
        float seaLevel,
        float maxHeight
    ): seed(seed), chunks(chunks), player(player), seaLevel(seaLevel), maxHeight(maxHeight) {};
    World();
    World(long seed);
    World(long seed, shared_ptr<Player> player);
    ~World() {};

    long getSeed() {return seed;}
    vector<shared_ptr<Chunk>> getChunks() {return chunks;}
    shared_ptr<Player> getPlayer() {return player;}
    float getSeaLevel() {return seaLevel;}
    float getMaxHeight() {return maxHeight;}
    void setSeed(long inSeed) {seed = inSeed;}
    void setChunks(vector<shared_ptr<Chunk>> inChunks) {chunks = inChunks;}
    void setPlayer(shared_ptr<Player> inPlayer) {player = inPlayer;}
    void setSeaLevel(float inSeaLevel) {seaLevel = inSeaLevel;}
    void setMaxHeight(float inMaxHeight) {maxHeight = inMaxHeight;}

    void addChunk(shared_ptr<Chunk> chunk) {chunks.push_back(chunk);}
    void removeChunk(shared_ptr<Chunk> chunk);

    void render(glm::mat4 view, glm::mat4 projection);
    void setupData();
    void updateData();

    void checkForNewChunks();
    void setUpInitialChunks();

    void requestNewChunk(int x, int z){requestNewChunk({x, z});};
    void requestNewChunk(vector<int> chunkCoords);
};

#endif // WORLD_HPP