/**
 * @file Cursor.hpp
 * @author King Attalus II
 * @brief This file contains the class definition for the Cursor class.
 * @version 1.0
 * @date 2025
 *
 */
#ifndef CURSOR_HPP
#define CURSOR_HPP

#include <vector>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/GLFW/glfw3.h"
#else
    #include <glm/glm.hpp>
    #include <GLFW/glfw3.h>
#endif

#include "Window.hpp"
#include "Settings.hpp"

/**
 * @brief This class is used to represent the cursor within the window to calculate mouse based movement.
 *
 */
class Cursor{
private:
    glm::vec2 position; // The position of the cursor on the screen
    float mouseSensitivity; // The sensitivity of the mouse
    bool firstMouse; // If this is the first mouse movement
    bool hideCursor; // If the cursor should be hidden

public:
    Cursor();
    Cursor(Settings settings);
    Cursor(glm::vec2 position, float mouseSensitivity, bool hideCursor);
    ~Cursor();

    glm::vec2 getPosition(){return position;}
    float getMouseSensitivity(){return mouseSensitivity;}
    bool getHideCursor(){return hideCursor;}

    void setPosition(glm::vec2 position){this->position = position;}
    void setStartPosition(Window *window);
    void setMouseSensitivity(float mouseSensitivity){this->mouseSensitivity = mouseSensitivity;}
    void setHideCursor(bool hideCursor){this->hideCursor = hideCursor;}

    glm::vec2 processMouseMovement(
        glm::vec2 newMousePos,
        GLFWwindow *window
    );


};
#endif // CURSOR_HPP
