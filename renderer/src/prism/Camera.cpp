/*
    This class represents the camera (player's perspective) in the scene. It is used to calculate
    the view matrix and the projection matrix for the scene. It also processes the input from the
    user to move the camera around the scene.
*/

#include <iostream>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
#endif

#include "Camera.hpp"

using namespace std;

void Camera::updateCameraVectors(){
    // calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    // also re-calculate the right and up vector
    right = glm::normalize(glm::cross(front, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    up    = glm::normalize(glm::cross(right, front));
}

Camera::Camera(){
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 10.0f;
    sprintFactor = 2.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    nearPlane = 0.1f;
    farPlane = 1000.0f;
    screenDimensions = glm::vec2(1920, 1080);
    edgeMargin = 15;
    edgeStep = 0.8f;
    onLeftEdge = false;
    onRightEdge = false;
    onTopEdge = false;
    onBottomEdge = false;
    shouldUpdate = false;
    updateCameraVectors();
}

Camera::Camera(glm::vec3 inPosition, glm::vec2 inScreenDimensions){
    position = inPosition;
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 10.0f;
    sprintFactor = 2.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    nearPlane = 0.1f;
    farPlane = 1000.0f;
    screenDimensions = inScreenDimensions;
    edgeMargin = 15;
    edgeStep = 0.8f;
    onLeftEdge = false;
    onRightEdge = false;
    onTopEdge = false;
    onBottomEdge = false;
    shouldUpdate = false;
    updateCameraVectors();
}

Camera::Camera(glm::vec3 inPosition, glm::vec3 inUp, glm::vec2 inScreenDimensions){
    position = inPosition;
    worldUp = inUp;
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 10.0f;
    sprintFactor = 2.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    nearPlane = 0.1f;
    farPlane = 1000.0f;
    screenDimensions = inScreenDimensions;
    edgeMargin = 15;
    edgeStep = 0.8f;
    onLeftEdge = false;
    onRightEdge = false;
    onTopEdge = false;
    onBottomEdge = false;
    shouldUpdate = false;
    updateCameraVectors();
}

Camera::Camera(glm::vec3 inPosition, glm::vec3 inUp, float inYaw, float inPitch, glm::vec2 inScreenDimensions){
    position = inPosition;
    worldUp = inUp;
    yaw = inYaw;
    pitch = inPitch;
    movementSpeed = 10.0f;
    sprintFactor = 2.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    nearPlane = 0.1f;
    farPlane = 1000.0f;
    screenDimensions = inScreenDimensions;
    edgeMargin = 15;
    edgeStep = 0.8f;
    onLeftEdge = false;
    onRightEdge = false;
    onTopEdge = false;
    onBottomEdge = false;
    shouldUpdate = false;
    updateCameraVectors();
}

void Camera::processKeyboard(Camera_Movement direction, bool sprint, float deltaTime){
    float velocity;
    if (sprint) {
        velocity = movementSpeed * sprintFactor * deltaTime;
    } else {
        velocity = movementSpeed * deltaTime;
    }
    switch (direction)
    {
    case FORWARD:
        position += front * velocity;
        break;
    case BACKWARD:
        position -= front * velocity;
        break;
    case LEFT:
        position -= right * velocity;
        break;
    case RIGHT:
        position += right * velocity;
        break;
    case UP:
        position += up * velocity;
        break;
    case DOWN:
        position -= up * velocity;
        break;
    default:
        // We should never get here so log an error
        cerr << "Invalid direction provided to processKeyboard" << endl;
        break;
    }
}

void Camera::processMouseMovement(
    glm::vec2 mousePos,
    glm::vec2 mouseOffset,
    int windowWidth,
    int windowHeight,
    GLboolean constrainPitch
){
    // Reset all edge flags
    onLeftEdge = false;
    onRightEdge = false;
    onTopEdge = false;
    onBottomEdge = false;

    mouseOffset *= mouseSensitivity;
    yaw += mouseOffset.x;
    pitch += mouseOffset.y;

    // Make sure that when pitch is out of bounds, the screen doesn't get flipped
    if (constrainPitch){
        if (pitch > 89.0f){
            pitch = 89.0f;
        }
        if (pitch < -89.0f){
            pitch = -89.0f;
        }
    }

    // Boarder check
    if (mousePos.x <= edgeMargin){
        onLeftEdge = true;
    } else if (mousePos.x >= (windowWidth - edgeMargin)){
        onRightEdge = true;
    } else if (mousePos.y <= edgeMargin){
        onTopEdge = true;
    } else if (mousePos.y >= (windowHeight - edgeMargin)){
        onBottomEdge = true;
    }

    // Update using the new Euler angles
    updateCameraVectors();
}

void Camera::processMouseScroll(float yoffset){
    zoom -= yoffset;
    if (zoom < 1.0f){
        zoom = 1.0f;
    }
    // This is the max zoom in
    if (zoom > 45.0f){
        zoom = 45.0f;
    }
}

void Camera::checkCameraConstraints(GLboolean constrainPitch){
    // Reset the update flag
    shouldUpdate = false;
    // Adjust the horizontal angle if on the left or right edge
    if (onLeftEdge){
        yaw -= edgeStep;
        shouldUpdate = true;
    } else if (onRightEdge){
        yaw += edgeStep;
        shouldUpdate = true;
    } else if (onTopEdge){
        pitch += edgeStep;
        shouldUpdate = true;
    } else if (onBottomEdge){
        pitch -= edgeStep;
        shouldUpdate = true;
    }

    // Constraint the pitch
    if (constrainPitch){
        if (pitch > 89.0f){
            pitch = 89.0f;
        }
        if (pitch < -89.0f){
            pitch = -89.0f;
        }
    }

    // Apply the updates if any edge scrolling is active
    if (shouldUpdate){
        updateCameraVectors();
    }
}


