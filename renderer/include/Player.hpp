/*
    This class represents the player in the scene. It stores the player's camera along with their
    position and size which will be used to calculate the bounding box of the player, preventing
    them from moving through the terrain.
*/
#ifndef PLAYER_HPP
#define PLAYER_HPP

#include <string>
#include <vector>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glm/glm.hpp>
#endif

#include "Camera.hpp"
#include "Window.hpp"
#include "Cursor.hpp"
#include "Settings.hpp"

// The player is 1.8x0.4x0.4 meters
// The camera will be located at 1.68x0.2x0.2 meters from the origin

using namespace std;

class Player{
private:
    shared_ptr<Camera> camera; // The camera that the player will use (see from)
    shared_ptr<Cursor> cursor; // The cursor that the player will use
    glm::vec3 position; // The origin of the player (bottom left corner)
    glm::vec3 size; // The size of the player
    int mode; // What mode the player is in

public:

    Player(
        shared_ptr<Camera> camera,
        shared_ptr<Cursor> cursor,
        glm::vec3 position,
        glm::vec3 size,
        int mode
    ):
        camera(move(camera)),
        cursor(move(cursor)),
        position(position),
        size(size),
        mode(mode)
    {};
    Player(Settings settings);
    Player(Settings settings, glm::vec3 position);
    Player();
    ~Player();

    // Getters and Setters
    shared_ptr<Camera> getCamera(){return camera;};
    shared_ptr<Cursor> getCursor(){return cursor;};
    glm::vec3 getPosition(){return position;};
    glm::vec3 getSize(){return size;};
    int getMode(){return mode;};
    void setMode(int mode){this->mode = mode;};
    void setPosition(glm::vec3 position){this->position = position;};
    void setSize(glm::vec3 size){this->size = size;};
    void setCamera(shared_ptr<Camera> camera){this->camera = move(camera);};
    void setCursor(shared_ptr<Cursor> cursor){this->cursor = move(cursor);};

    // Controller Functions
    void processKeyBoardInput(shared_ptr<Window> window, float deltaTime);

    vector<glm::vec3> getBboxVertices();
};
#endif
