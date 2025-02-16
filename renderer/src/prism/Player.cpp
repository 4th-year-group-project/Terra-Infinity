/*
    This class represents the player in the scene. It stores the player's camera along with their
    position and size which will be used to calculate the bounding box of the player, preventing
    them from moving through the terrain.
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

Player::Player(){
    glm::vec3 tempPosition(0.0f, 0.0f, 0.0f);
    camera = make_shared<Camera>(
        tempPosition + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(1920, 1080)
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>();
    position = tempPosition;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
    cout << "Player created" << endl;
}

Player::Player(Settings settings){
    glm::vec3 tempPosition(0.0f, 0.0f, 0.0f);
    camera = make_shared<Camera>(
        tempPosition + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(settings.getWindowWidth(), settings.getWindowHeight())
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>(settings);
    position = tempPosition;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
}

Player::Player(Settings settings, glm::vec3 position){
    camera = make_shared<Camera>(
        position + glm::vec3(1.68f, 0.2f, 0.2f),
        glm::vec2(settings.getWindowWidth(), settings.getWindowHeight())
    ); // Passing in the initial camera position
    cursor = make_shared<Cursor>(settings);
    this->position = position;
    size = glm::vec3(1.8f, 0.4f, 0.4f);
    mode = 0;
}

Player::~Player(){
    // Nothing to do here
}

void Player::processKeyBoardInput(shared_ptr<Window> window, float deltaTime){
    if (window->getWindow() == nullptr){
        cerr << "ERROR::PLAYER::WINDOW_IS_NULL" << endl;
        return;
    }
    // Check to see if we are exiting the application
    if (glfwGetKey(window->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS){
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
    // for (int x=0; x < 2; x++){
    //     for (int y=0; y < 2; y++){
    //         for (int z=0; z < 2; z++){
    //             glm::vec3 vertex = glm::vec3(
    //                 position.x + size.x * x,
    //                 position.y + size.y * y,
    //                 position.z + size.z * z
    //             );
    //             vertices.push_back(vertex);
    //         }
    //     }
    // }
    return vertices;
}