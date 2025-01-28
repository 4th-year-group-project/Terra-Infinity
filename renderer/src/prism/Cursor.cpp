/*
    This is a simple class for a cursor in the scene. It is used to calculate the position of the
    cursor in the scene and to determine if the cursor is on the edge of the screen.
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

using namespace std;

Cursor::Cursor(){
    position = glm::vec2(0.0f, 0.0f);
    mouseSensitivity = 0.05f;
    firstMouse = true;
    hideCursor = false;
}

Cursor::Cursor(Window *window){
    glm::vec2 windowSize = glm::vec2(window->getWidth(), window->getHeight());
    position = windowSize / 2.0f;
    mouseSensitivity = 0.05f;
    firstMouse = true;
    hideCursor = false;
}

Cursor::Cursor(glm::vec2 position, float mouseSensitivity, bool hideCursor){
    this->position = position;
    this->mouseSensitivity = mouseSensitivity;
    this->hideCursor = hideCursor;
    firstMouse = true;
}

Cursor::~Cursor(){
    // Nothing to do here
}

void Cursor::setStartPosition(Window *window){
    // We will use GLFW to set the cursor position
    glfwSetCursorPos(window->getWindow(), position.x, position.y);
}

glm::vec2 Cursor::processMouseMovement(
    glm::vec2 newMousePos,
    GLFWwindow *window
){
    if (firstMouse){
        firstMouse = false;
        position = newMousePos;
        return position;
    }
    // Ensure that the new position is within the window and if not clamp it
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glm::vec2 windowSize = glm::vec2(width, height);
    if (newMousePos.x < 0){
        newMousePos.x = 0;
    } else if (newMousePos.x > windowSize.x){
        newMousePos.x = windowSize.x;
    }
    if (newMousePos.y < 0){
        newMousePos.y = 0;
    } else if (newMousePos.y > windowSize.y){
        newMousePos.y = windowSize.y;
    }
    // Compute the offset
    glm::vec2 mouseOffset = glm::vec2(newMousePos.x - position.x, position.y - newMousePos.y);
    position = newMousePos;
    return mouseOffset;
}
