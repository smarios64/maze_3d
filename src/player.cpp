#include "player.h"
#include "console.h"
#include "common.h"

// Default camera values
#define SPEED         2.5f
#define SENSITIVITY   0.1f
#define ZOOM          45.0f
#define PITCH         0.0f

// Constructor with vectors
Player::Player(bool *walls) :
    Front(glm::vec3(0.0f, 0.0f, -1.0f)),
    MovementSpeed(SPEED),
    MouseSensitivity(SENSITIVITY),
    m_walls(walls)
{
    Position = glm::vec3(WALL_SIZE / 2.0f);
    if (!walls[1]) {
        Yaw = 0.0f;
    }
    else {
        Yaw = 90.0f;
    }
    
    WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
    Pitch = PITCH;
    updateViewVectors();
}

// Processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void Player::processMovement(PlayerMovement direction, float deltaTime)
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
    
    Position += validateMovement(desiredMovement);
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void Player::processRotation(float xoffset, float yoffset, GLboolean constrainPitch)
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
    updateViewVectors();
}

glm::vec3 Player::validateMovement(glm::vec3 movementOffset)
{
#ifdef DEBUG
    return movementOffset;
#else
    static const glm::vec2 normVector((WALL_SIZE * MAZE_WIDTH) / ((WALL_SIZE + WALL_THICKNESS) * MAZE_WIDTH - WALL_THICKNESS),
                                      (WALL_SIZE * MAZE_HEIGHT) / ((WALL_SIZE + WALL_THICKNESS) * MAZE_HEIGHT - WALL_THICKNESS));
                 
    glm::vec3 ret;  
    glm::vec2 curPosNorm(Position.x * normVector.x, Position.z * normVector.y);
    glm::vec2 nextPosNorm((Position.x + movementOffset.x) * normVector.x,
                      (Position.z + movementOffset.z) * normVector.y);  
    bool hasWall = false;
    int minIndex, maxIndex;
    if (nextPosNorm.x > 0.1f * normVector.x && nextPosNorm.x < MAZE_WIDTH * WALL_SIZE - 0.1f * normVector.x) {
        if (curPosNorm.x < nextPosNorm.x) {
            minIndex = MIN((curPosNorm.x + (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE, (nextPosNorm.x + (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE);
            maxIndex = MAX((curPosNorm.x + (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE, (nextPosNorm.x + (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE);
        }
        else {
            minIndex = MIN((curPosNorm.x - (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE, (nextPosNorm.x - (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE);
            maxIndex = MAX((curPosNorm.x - (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE, (nextPosNorm.x - (WALL_THICKNESS + 0.1f) * normVector.x) / WALL_SIZE);
        }
        if (minIndex != maxIndex) {
            for (int x = minIndex + 1; x <= maxIndex; ++x) {
                if (m_walls[(int)(curPosNorm.y / WALL_SIZE) * 2 * MAZE_WIDTH + x]) {
                    hasWall = true;
                    break;
                }
            }
        }
        if (!hasWall) {
            ret.x = movementOffset.x;
        }
    }
         
    if (nextPosNorm.y > 0.1f * normVector.y && nextPosNorm.y < MAZE_HEIGHT * WALL_SIZE - 0.1f * normVector.y) {           
        hasWall = false;
        if (curPosNorm.y < nextPosNorm.y) {
            minIndex = MIN((curPosNorm.y + (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE, (nextPosNorm.y + (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE);
            maxIndex = MAX((curPosNorm.y + (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE, (nextPosNorm.y + (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE);
        }
        else {
            minIndex = MIN((curPosNorm.y - (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE, (nextPosNorm.y - (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE);
            maxIndex = MAX((curPosNorm.y - (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE, (nextPosNorm.y - (WALL_THICKNESS + 0.1f) * normVector.y) / WALL_SIZE);
        }
        for (int y = minIndex; y < maxIndex; ++y) {
            if (m_walls[(y * 2 + 1) * MAZE_WIDTH + (int)(curPosNorm.x / WALL_SIZE)]) {
                hasWall = true;
                break;
            }
        }
        if (!hasWall) {
            ret.z = movementOffset.z;
        }
    }
    return ret;
#endif
}

// Calculates the front vector from the Camera's (updated) Euler Angles
void Player::updateViewVectors()
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