#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Player
{
public:
    glm::vec3 Position;

    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum PlayerMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    // Constructor with vectors
    Player(bool *walls);
    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void processMovement(PlayerMovement direction, float deltaTime);

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void processRotation(float xoffset, float yoffset, GLboolean constrainPitch = true);

private:
    bool *m_walls;

    // Camera Attributes
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // Euler Angles
    float Yaw;
    float Pitch;
    // Camera options
    float MovementSpeed;
    float MouseSensitivity;

    glm::vec3 validateMovement(glm::vec3 movementOffset);

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateViewVectors();
};
#endif