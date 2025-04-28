/**
 * @file Player.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Player class.
 * @details This class represents the player in the scene. It stores the player's camera along with
 * their position and size which will be used to calculate the bounding box of the player,
 * preventing them from moving through the terrain.
 * @version 1.0
 * @date 2025
 * 
 */
#include <string>
#include <iostream>
#include <memory>
#include <vector>
#include <mutex>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

#include <Camera.hpp>
#include <Window.hpp>
#include <Cursor.hpp>
#include <Player.hpp>
#include <Settings.hpp>

using namespace std;

/**
 * @brief Construct a new Player object with default values
 * 
 * @details This constructor will create a player at the origin with the default values for the player.
 * The default values are:
 * - camera: Constructed with the following parameters:
 *   - position: The position of the camera (1.68, 0.2, 0.2)
 *   - windowSize: The size of the window (1920-600, 1080)
 *   - farPlane: The far plane of the camera (1000.0)
 * - cursor: Constructed with default values
 * - position: The position of the player (0, 0, 0)
 * - size: The size of the player (1.8, 0.4, 0.4)
 * - mode: The mode of the player (0)
 * 
 */
Player::Player(){
    glm::vec3 tempPosition(0.0f, 0.0f, 0.0f);
    camera = make_shared<Camera>(
        tempPosition + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(1920-600, 1080),
        1000.0f
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>();
    position = tempPosition;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
    cout << "Player created" << endl;
}

/**
 * @brief Construct a new Player object with the given settings
 * 
 * @details This constructor will create a player at the origin with the default values for the player.
 * The default values are:
 * - camera: Constructed with the following parameters:
 *   - position: The position of the camera (1.68, 0.2, 0.2)
 *   - windowSize: The size of the window (1920-600, 1080)
 *   - farPlane: The far plane of the camera (1000.0)
 * - cursor: Constructed with default values
 * - position: The position of the player (0, 0, 0)
 * - size: The size of the player (1.8, 0.4, 0.4)
 * - mode: The mode of the player (0)
 * 
 * @param settings [in] Settings The settings object
 */
Player::Player(Settings settings){
    glm::vec3 tempPosition(0.0f, 0.0f, 0.0f);
    camera = make_shared<Camera>(
        tempPosition + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(settings.getWindowWidth()-settings.getUIWidth(), settings.getWindowHeight()),
        sqrt((settings.getRenderDistance() - 1.25) * settings.getSubChunkSize())
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>(settings);
    position = tempPosition;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
}

/**
 * @brief Construct a new Player object with the given settings and position
 * 
 * @details This constructor will create a player at the given position with the default values for
 * the player. The default values are:
 * - camera: Constructed with the following parameters:
 *   - position: The position of the player plus (1.68, 0.2, 0.2)
 *   - windowSize: The size of the window (1920-600, 1080)
 *   - farPlane: The far plane of the camera (1000.0)
 * - cursor: Constructed with default values
 * - size: The size of the player (1.8, 0.4, 0.4)
 * - mode: The mode of the player (0)
 * 
 * @param settings [in] Settings The settings object
 * @param position [in] glm::vec3 The position of the player
 * 
 */
Player::Player(Settings settings, glm::vec3 position){
    camera = make_shared<Camera>(
        position + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(settings.getWindowWidth()-settings.getUIWidth(), settings.getWindowHeight()),
        sqrt((settings.getRenderDistance() - 1.25) * settings.getSubChunkSize())
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>(settings);
    this->position = position;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
}

/**
 * @brief The default destructor for the Player class
 * 
 * @details This destructor will destroy the player object. This allows default distruction
 * 
 */
Player::~Player(){
    // Nothing to do here
}

/**
 * @brief Processes the keyboard input for the player
 * 
 * @details This method will process the keyboard input for the player. It will check to see if the
 * window is null and if so, it will return. It will then check to see if the player is sprinting by
 * checking both shift keys. It will then check which keys are currently being pressed and call the
 * camera's processKeyboard method with the appropriate parameters.
 * 
 * @param window [in] std::shared_ptr<Window> The window object
 * @param deltaTime [in] float The delta time
 * 
 * @return void
 */
void Player::processKeyBoardInput(shared_ptr<Window> window, float deltaTime){
    if (window->getWindow() == nullptr){
        cerr << "ERROR::PLAYER::WINDOW_IS_NULL" << endl;
        return;
    }
    // Check to see if we are exiting the application
    if (glfwGetKey(window->getWindow(), GLFW_KEY_K) == GLFW_PRESS){
        glfwSetWindowShouldClose(window->getWindow(), true);
    }

    // Check to see if the player is sprinting by checking both shift keys
    bool sprint = false;
    sprint = glfwGetKey(window->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    sprint = sprint || glfwGetKey(window->getWindow(), GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
    // Check which keys are currently being pressed
    if (glfwGetKey(window->getWindow(), GLFW_KEY_W) == GLFW_PRESS){
        camera->processKeyboard(FORWARD, sprint, deltaTime);
    }
    if (glfwGetKey(window->getWindow(), GLFW_KEY_S) == GLFW_PRESS){
        camera->processKeyboard(BACKWARD, sprint, deltaTime);
    }
    if (glfwGetKey(window->getWindow(), GLFW_KEY_A) == GLFW_PRESS){
        camera->processKeyboard(LEFT, sprint, deltaTime);
    }
    if (glfwGetKey(window->getWindow(), GLFW_KEY_D) == GLFW_PRESS){
        camera->processKeyboard(RIGHT, sprint, deltaTime);
    }
    if (glfwGetKey(window->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS){
        camera->processKeyboard(UP, sprint, deltaTime);
    }
    if (glfwGetKey(window->getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        camera->processKeyboard(DOWN, sprint, deltaTime);
    }
    glm::vec3 newPosition = camera->getPosition();
    position = newPosition - glm::vec3(1.68f, 0.2f, 0.2f);
}

/**
 * @brief This function will return a simple axis aligned bounding box for the player
 * 
 * @return std::vector<glm::vec3> 
 * 
 */
vector<glm::vec3> Player::getBboxVertices(){
    vector<glm::vec3> vertices;
    // The player is 1.8x0.4x0.4 meters
    for (int i=0; i <8; i++){
        glm::vec3 vertex = glm::vec3(
            position.x + size.x * (i % 2),
            position.y + size.y * ((i / 2) % 2),
            position.z + size.z * ((i / 4) % 2)
        );
        vertices.push_back(vertex);
    }
    return vertices;
}