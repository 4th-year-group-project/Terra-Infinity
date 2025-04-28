/**
 * @file Camera.cpp
 * @author King Attalus II
 * @brief This file contains the implementation of the Camera class, which is used to represent the camera in the scene.
 * @version 1.0
 * @date 2025
 *
 */

#include <iostream>
#include <memory>

#ifdef DEPARTMENT_BUILD
    #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
#else
    #include <glad/glad.h>
    #include <glm/glm.hpp>
#endif

#include "Camera.hpp"
#include "Settings.hpp"

/**
 * @brief This function updates the camera vectors based on the current yaw and pitch angles within the class
 *
 * @return void
 *
 */
void Camera::updateCameraVectors(){
    // calculate the new front vector
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(newFront);
    // Recalculate the right and up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in
    // slower movement.
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

/**
 * @brief Construct a new Camera object
 *
 * @details This constructor will create a camera at the origin with the default values for the camera.
 * The default values are:
 * - position: (0.0f, 0.0f, 0.0f)
 * - worldUp: (0.0f, 1.0f, 0.0f)
 * - yaw: -90.0f
 * - pitch: 0.0f
 * - movementSpeed: 10.0f
 * - sprintFactor: 2.0f
 * - mouseSensitivity: 0.05f
 * - zoom: 45.0f
 * - nearPlane: 0.1f
 * - farPlane: 1000.0f
 * - screenDimensions: (1920, 1080)
 * - edgeMargin: 15
 * - edgeStep: 0.8f
 *
 */
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

/**
 * @brief Construct a new Camera object
 *
 * @details This constructor will create a camera at the given position for a given screen size and far plane.
 * The other values are set to the default values.
 *
 * @param inPosition [in] glm::vec3 The position of the camera
 * @param inScreenDimensions [in] glm::vec2 The screen dimensions of the camera
 * @param inFarPlane [in] float The far plane of the camera
 *
 */
Camera::Camera(glm::vec3 inPosition, glm::vec2 inScreenDimensions, float inFarPlane){
    position = inPosition;
    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    yaw = -90.0f;
    pitch = 0.0f;
    movementSpeed = 10.0f;
    sprintFactor = 2.0f;
    mouseSensitivity = 0.05f;
    zoom = 45.0f;
    nearPlane = 0.1f;
    farPlane = inFarPlane;
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

/**
 * @brief Construct a new Camera object
 *
 * @details This constructor will create a camera at the given position and up vector for a given screen size.
 * The other values are set to the default values.
 *
 * @param inPosition [in] glm::vec3 The position of the camera
 * @param inUp [in] glm::vec3 The up vector of the camera
 * @param inScreenDimensions [in] glm::vec2 The screen dimensions of the camera
 *
 */
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

/**
 * @brief Construct a new Camera object
 *
 * @details This constructor will create a camera at the given position and up vector with the given yaw and pitch.
 * The other values are set to the default values.
 *
 * @param inPosition [in] glm::vec3 The position of the camera
 * @param inUp [in] glm::vec3 The up vector of the camera
 * @param inYaw [in] float The yaw of the camera
 * @param inPitch [in] float The pitch of the camera
 * @param inScreenDimensions [in] glm::vec2 The screen dimensions of the camera
 *
 */
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

/**
 * @brief Process the keyboard input for the camera
 *
 * @details This function will process the keyboard input for the camera and move the camera in the
 * direction specified by the direction parameter. The sprint parameter will determine if the camera
 * should move faster or not.
 *
 * @param direction [in] Camera_Movement The direction to move the camera
 * @param sprint [in] bool Whether to sprint or not
 * @param deltaTime [in] float The delta time since the last frame
 *
 * @return void
 *
 */
void Camera::processKeyboard(Camera_Movement direction, bool sprint, float deltaTime){
    float velocity;
    if (sprint) {
        velocity = movementSpeed * sprintFactor * deltaTime;
    } else {
        velocity = movementSpeed * deltaTime;
    }
    // glm::vec3 oldPosition = position;
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

/**
 * @brief Process the mouse movement input for the camera
 *
 * @details This function will process the mouse movement input for the camera and update the camera
 * position and orientation based on the mouse position and offset. The constrainPitch parameter will
 * determine if the pitch should be constrained or not.
 *
 * @param mousePos [in] glm::vec2 The current mouse position
 * @param mouseOffset [in] glm::vec2 The offset of the mouse position
 * @param windowWidth [in] int The width of the window
 * @param windowHeight [in] int The height of the window
 * @param constrainPitch [in] bool Whether to constrain the pitch or not
 *
 * @return void
 *
 */
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

    // Update using the new pitch and yaw values
    updateCameraVectors();
}

/**
 * @brief Process the mouse scroll input for the camera
 *
 * @details This function will process the mouse scroll input for the camera and update the camera
 * zoom based on the scroll offset.
 *
 * @param yoffset [in] float The scroll offset
 *
 * @return void
 *
 */
void Camera::processMouseScroll(float yoffset){
    zoom -= yoffset;
    // We only allow a minimum zoom of 1.0f and a maximum zoom of 45.0f
    if (zoom < 1.0f){
        zoom = 1.0f;
    }
    if (zoom > 45.0f){
        zoom = 45.0f;
    }
}

/**
 * @brief Check the camera constraints
 *
 * @details This function will check the camera constraints and update the camera position and orientation
 * based on the edge scrolling. The constrainPitch parameter will determine if the pitch should be constrained or not.
 *
 * @param constrainPitch [in] GLboolean Whether to constrain the pitch or not
 *
 * @return void
 *
 */
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

/**
 * @brief Inverts the camera
 *
 * @details This function will invert the camera and move it to the opposite side of the water and negating the pitch.
 *
 * @param settings [in] std::shared_ptr<Settings> The settings object
 *
 * @return void
 *
 */
void Camera::setInverted(std::shared_ptr<Settings> settings){
    // Invert the camera
    pitch = -pitch;
    // Move the camera as far below the water as it is above the water
    float waterLevel = settings->getSeaLevel() * settings->getMaximumHeight();
    float distanceFromWater = position.y - waterLevel;
    position.y = waterLevel - distanceFromWater;
    // Update the camera vectors
    updateCameraVectors();
}

/**
 * @brief Restores the camera to its normal position and orientation
 *
 * @details This function will restore the camera to its normal position and negate the pitch.
 *
 * @param settings [in] std::shared_ptr<Settings> The settings object
 *
 * @return void
 *
 */
void Camera::setNormal(std::shared_ptr<Settings> settings){
    // Restore the pitch to normal
    pitch = -pitch;
    // Move the camera as far above the water as it is below the water
    // This will restore the camera to the original position
    float waterLevel = settings->getSeaLevel() * settings->getMaximumHeight();
    float distanceBelowWater = waterLevel - position.y;
    position.y = waterLevel + distanceBelowWater;
    // Update the camera vectors
    updateCameraVectors();
}