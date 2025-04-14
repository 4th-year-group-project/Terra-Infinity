/*
    This class represents the camera (player's perspective) in the scene. It is used to calculate
    the view matrix and the projection matrix for the scene. It also processes the input from the
    user to move the camera around the scene.
*/
#ifndef CAMERA_HPP
#define CAMERA_HPP


#ifdef DEPARTMENT_BUILD
    // #include "/dcs/large/efogahlewem/.local/include/glad/glad.h"
    #include "/dcs/large/efogahlewem/.local/include/glm/glm.hpp"
    #include "/dcs/large/efogahlewem/.local/include/glm/gtc/matrix_transform.hpp"
#else
    // #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
#endif

// Defines several possible options for camera movement. Used as abstraction to stay 
// away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class Camera {
private:
    // camera Attributes
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;
    // euler Angles
    float yaw;
    float pitch;
    // camera movement options
    float movementSpeed;
    float sprintFactor;
    float mouseSensitivity;
    // Used for the perspective projection matrix
    float zoom;
    float nearPlane;
    float farPlane;
    glm::vec2 screenDimensions;

    int edgeMargin;
    float edgeStep;
    bool onLeftEdge;
    bool onRightEdge;
    bool onTopEdge;
    bool onBottomEdge;
    bool shouldUpdate;

    void updateCameraVectors();

public:
    Camera(
        glm::vec3 inPosition,
        glm::vec3 inUp,
        float inYaw,
        float inPitch,
        float inMovementSpeed,
        float inSprintFactor,
        float inMouseSensitivity,
        float inZoom,
        float inNearPlane,
        float inFarPlane,
        glm::vec2 inScreenDimensions,
        int inEdgeMargin,
        float inEdgeStep
    ):
        position(inPosition),
        worldUp(inUp),
        yaw(inYaw),
        pitch(inPitch),
        movementSpeed(inMovementSpeed),
        sprintFactor(inSprintFactor),
        mouseSensitivity(inMouseSensitivity),
        zoom(inZoom),
        nearPlane(inNearPlane),
        farPlane(inFarPlane),
        screenDimensions(inScreenDimensions),
        edgeMargin(inEdgeMargin),
        edgeStep(inEdgeStep),
        onLeftEdge(false),
        onRightEdge(false),
        onTopEdge(false),
        onBottomEdge(false),
        shouldUpdate(false)
    {
        updateCameraVectors();
    }

    // Overloaded Constructors
    Camera(); // Default constructor
    Camera(glm::vec3 inPosition, glm::vec2 inScreenDimensions, float inFarPlane);
    Camera(glm::vec3 inPosition, glm::vec3 inUp, glm::vec2 inScreenDimensions);
    Camera(glm::vec3 inPosition, glm::vec3 inUp, float inYaw, float inPitch, glm::vec2 inScreenDimensions);
    ~Camera(){}; // Default destructor

    // Getters and Setters
    void setPosition(glm::vec3 inPosition){position = inPosition;}
    void setUp(glm::vec3 inUp){up = inUp;}
    void setWorldUp(glm::vec3 inWorldUp){worldUp = inWorldUp;}
    void setYaw(float inYaw){yaw = inYaw;}
    void setPitch(float inPitch){pitch = inPitch;}
    void setMovementSpeed(float inMovementSpeed){movementSpeed = inMovementSpeed;}
    void setMouseSensitivity(float inMouseSensitivity){mouseSensitivity = inMouseSensitivity;}
    void setZoom(float inZoom){zoom = inZoom;}
    void setNearPlane(float inNearPlane){nearPlane = inNearPlane;}
    void setFarPlane(float inFarPlane){farPlane = inFarPlane;}
    void setScreenDimensions(glm::vec2 inScreenDimensions){screenDimensions = inScreenDimensions;}
    void setEdgeMargin(int inEdgeMargin){edgeMargin = inEdgeMargin;}
    void setEdgeStep(float inEdgeStep){edgeStep = inEdgeStep;}
    void setOnLeftEdge(bool inOnLeftEdge){onLeftEdge = inOnLeftEdge;}
    void setOnRightEdge(bool inOnRightEdge){onRightEdge = inOnRightEdge;}
    void setOnTopEdge(bool inOnTopEdge){onTopEdge = inOnTopEdge;}
    void setOnBottomEdge(bool inOnBottomEdge){onBottomEdge = inOnBottomEdge;}
    void setShouldUpdate(bool inShouldUpdate){shouldUpdate = inShouldUpdate;}
    glm::vec3 getPosition(){return position;}
    glm::vec3 getFront(){return front;}
    glm::vec3 getUp(){return up;}
    glm::vec3 getRight(){return right;}
    glm::vec3 getWorldUp(){return worldUp;}
    float getYaw(){return yaw;}
    float getPitch(){return pitch;}
    float getMovementSpeed(){return movementSpeed;}
    float getMouseSensitivity(){return mouseSensitivity;}
    float getZoom(){return zoom;}
    float getNearPlane(){return nearPlane;}
    float getFarPlane(){return farPlane;}
    glm::vec2 getScreenDimensions(){return screenDimensions;}
    int getEdgeMargin(){return edgeMargin;}
    float getEdgeStep(){return edgeStep;}
    bool getOnLeftEdge(){return onLeftEdge;}
    bool getOnRightEdge(){return onRightEdge;}
    bool getOnTopEdge(){return onTopEdge;}
    bool getOnBottomEdge(){return onBottomEdge;}
    bool getShouldUpdate(){return shouldUpdate;}

    // Actual Camera Functions
    glm::mat4 getViewMatrix(){return glm::lookAt(position, position + front, up);}
    glm::mat4 getProjectionMatrix(){
        return glm::perspective(
            glm::radians(zoom),
            screenDimensions.x / screenDimensions.y,
            nearPlane,
            farPlane
        );
    }

    void processKeyboard(Camera_Movement direction, bool sprint, float deltaTime);
    void processMouseMovement(
        glm::vec2 mousePos,
        glm::vec2 mouseOffset,
        int windowWidth,
        int windowHeight,
        GLboolean constrainPitch = true
    );
    void processMouseScroll(float yoffset);
    void checkCameraConstraints(GLboolean constrainPitch = true);

};

#endif // CAMERA_HPP
