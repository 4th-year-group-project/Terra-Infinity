/**
 * @file Cursor.cpp
 * @author King Attalus II
 * @brief Implementation of the Cursor class.
 * @version 1.0
 * @date 2025
 */

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

#include "Cursor.hpp"
#include "Window.hpp"
#include "Settings.hpp"

using namespace std;

/**
 * @brief Construct a new Cursor object with default values
 * 
 * @details This constructor will create a cursor at the bottom left of the window with the default
 * values for the cursor.
 * The default values are:
 * - position: The position of the cursor (0, 0)
 * - mouseSensitivity: The sensitivity of the mouse (0.05)
 * - firstMouse: Whether this is the first time the mouse is moved (true)
 * - hideCursor: Whether to hide the cursor (true)
 * 
 */
Cursor::Cursor(){
    position = glm::vec2(0.0f, 0.0f);
    mouseSensitivity = 0.05f;
    firstMouse = true;
    hideCursor = true;
}

/**
 * @brief Construct a new Cursor object with the given settings
 * 
 * @details This constructor will create a cursor at the center of the window with the default
 * values for the cursor.
 * The default values are:
 * - position: The position of the cursor (windowWidth / 2, windowHeight / 2)
 * - mouseSensitivity: The sensitivity of the mouse (0.05)
 * - firstMouse: Whether this is the first time the mouse is moved (true)
 * - hideCursor: Whether to hide the cursor (true)
 * 
 * @param settings [in] std::shared_ptr<Settings> The settings object
 */
Cursor::Cursor(Settings settings){
    position = glm::vec2(settings.getWindowWidth(), settings.getWindowHeight()) / 2.0f;
    mouseSensitivity = 0.05f;
    firstMouse = true;
    hideCursor = true;
}

/**
 * @brief Construct a new Cursor object with the given position and mouse sensitivity
 * 
 * @details This constructor will create a cursor at the given position with the given mouse
 * sensitivity.
 * The default values are:
 * - firstMouse: Whether this is the first time the mouse is moved (true)
 * 
 * @param position [in] glm::vec2 The position of the cursor
 * @param mouseSensitivity [in] float The sensitivity of the mouse
 * @param hideCursor [in] bool Whether to hide the cursor
 * 
 */
Cursor::Cursor(glm::vec2 position, float mouseSensitivity, bool hideCursor){
    this->position = position;
    this->mouseSensitivity = mouseSensitivity;
    this->hideCursor = hideCursor;
    firstMouse = true;
}

/**
 * @brief Destroy the Cursor object
 * 
 * @details This destructor will destroy the cursor object. This allows default distruction
 * 
 */
Cursor::~Cursor(){
    // Nothing to do here
}

/**
 * @brief Sets the position of the cursor
 * 
 * @details This method will set the position of the cursor to the given position. This is used to
 * set the position of the cursor when it is first created.
 * 
 * @param window [in] Window* The window object
 * 
 * @return void
 */
void Cursor::setStartPosition(Window *window){
    // We will use GLFW to set the cursor position
    glfwSetCursorPos(window->getWindow(), position.x, position.y);
}

/**
 * @brief Processes the mouse movement
 * 
 * @details This method will process the mouse movement and return the offset of the mouse
 * movement. This is used to move the camera when the mouse is moved.
 * 
 * @param newMousePos [in] glm::vec2 The new position of the mouse
 * @param window [in] GLFWwindow* The window object
 * 
 * @return glm::vec2 The offset of the mouse movement
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
glm::vec2 Cursor::processMouseMovement(
    glm::vec2 newMousePos,
    GLFWwindow *window
){
    if (this->firstMouse){
        this->firstMouse = false;
        position = newMousePos;
    }
    // Compute the offset
    glm::vec2 mouseOffset = glm::vec2(newMousePos.x - position.x, position.y - newMousePos.y);
    position = newMousePos;
    return mouseOffset;
}
#pragma GCC diagnostic pop