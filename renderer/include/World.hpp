/**
 * This file contains a class for the world object which contains all the information about the
 * renderable world.
 */

#ifndef WORLD_HPP
#define WORLD_HPP

#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "IRenderable.hpp"
#include "Chunk.hpp"
#include "Terrain.hpp"
#include "Settings.hpp"

using namespace std;

class World : public IRenderable {
private:
    long seed; // The seed for the world
    vector<shared_ptr<Chunk>> chunks; // The chunks that are loaded in the world
    shared_ptr<Player> player; // The player object in the world
    float seaLevel; // The sea level of the world
    float maxHeight; // The maximum height of the world

    long generateRandomSeed();
public:
    World(
        long seed,
        vector<shared_ptr<Chunk>> chunks,
        Settings settings,
        shared_ptr<Player> player
    );
    World(Settings settings, shared_ptr<Player> player);
    ~World() {};

    long getSeed() {return seed;}
    vector<shared_ptr<Chunk>> getChunks() {return chunks;}
    shared_ptr<Player> getPlayer() {return player;}
    float getSeaLevel() {return seaLevel;}
    float getMaxHeight() {return maxHeight;}
    void setPlayer(shared_ptr<Player> inPlayer) {player = inPlayer;}
    void setSeaLevel(float inSeaLevel) {seaLevel = inSeaLevel;}
    void setMaxHeight(float inMaxHeight) {maxHeight = inMaxHeight;}
    void setSeed(long inSeed) {seed = inSeed;}
    void setChunks(vector<shared_ptr<Chunk>> inChunks) {chunks = inChunks;}
    void addChunk(shared_ptr<Chunk> chunk) {chunks.push_back(chunk);}
    void render(glm::mat4 view, glm::mat4 projection) override;
    void setupData() override;
    void updateData() override;
};

#endif // WORLD_HPP
