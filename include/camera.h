#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "common.h"
#include "console.h"

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
    enum Camera_Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };
    // Camera Attributes
    glm::vec3 Position;
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
    float Zoom;

    // Constructor with vectors
    Camera(bool *walls, glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
        Front(glm::vec3(0.0f, 0.0f, -1.0f)),
        MovementSpeed(SPEED),
        MouseSensitivity(SENSITIVITY),
        Zoom(ZOOM),
        m_walls(walls)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // Constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // Returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        glm::vec3 front = Front, desiredMovement;
        front.y = 0;
        front = glm::normalize(front);
        if (direction == FORWARD)
            desiredMovement = front * velocity;
        if (direction == BACKWARD)
            desiredMovement = -(front * velocity);
        if (direction == LEFT)
            desiredMovement = -(Right * velocity);
        if (direction == RIGHT)
            desiredMovement = Right * velocity;
        if (direction == UP)
            desiredMovement = WorldUp * velocity;
        if (direction == DOWN)
            desiredMovement = -(WorldUp * velocity);
        
        Position += processMovement(desiredMovement);
    }

    // Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw   += xoffset;
        Pitch += yoffset;

        // Make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

private:
    bool *m_walls;

    glm::vec3 processMovement(glm::vec3 movementOffset)
    {
        static const glm::vec2 normVector((WALL_SIZE * MAZE_WIDTH) / ((WALL_SIZE + COLUMN_SIZE) * MAZE_WIDTH - COLUMN_SIZE),
                                          (WALL_SIZE * MAZE_HEIGHT) / ((WALL_SIZE + COLUMN_SIZE) * MAZE_HEIGHT - COLUMN_SIZE));
                     
        glm::vec3 ret;        
        glm::vec2 curPos(Position.x * normVector.x, Position.z * normVector.y);
        glm::vec2 nextPos((Position.x + movementOffset.x) * normVector.x,
                          (Position.z + movementOffset.z) * normVector.y);  
        bool hasWall = false;
        int minIndex, maxIndex;

        if (nextPos.x > 0.0f && nextPos.x < MAZE_WIDTH * WALL_SIZE) {
            minIndex = MIN(curPos.x / WALL_SIZE, nextPos.x / WALL_SIZE),
            maxIndex = MAX(curPos.x / WALL_SIZE, nextPos.x / WALL_SIZE);
            if (minIndex != maxIndex) {
                for (int x = minIndex + 1; x <= maxIndex; ++x) {
                    if (m_walls[(int)(curPos.y / WALL_SIZE) * 2 * MAZE_WIDTH + x]) {
                        hasWall = true;
                        break;
                    }
                }
            }

            if (!hasWall) {
                ret.x = movementOffset.x;
            }
        }
             
        if (nextPos.y > 0.0f && nextPos.y < MAZE_HEIGHT * WALL_SIZE) {           
            hasWall = false;
            minIndex = MIN(curPos.y / WALL_SIZE, nextPos.y / WALL_SIZE);
            maxIndex = MAX(curPos.y / WALL_SIZE, nextPos.y / WALL_SIZE);

            for (int y = minIndex; y < maxIndex; ++y) {
                if (m_walls[(y * 2 + 1) * MAZE_WIDTH + (int)(curPos.x / WALL_SIZE)]) {
                    hasWall = true;
                    break;
                }
            }

            if (!hasWall) {
                ret.z = movementOffset.z;
            }
        }
        ret.y = movementOffset.y;
        return ret;
    }

    // Calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // Calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
#endif