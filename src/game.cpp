/*
 * Copyright (C) 2020 Marios Christoforakis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * File:   game.cpp
 * Author: Marios Christoforakis
 *
 * Created on April 20, 2020, 14:51
 */

#include "game.h"
#include "minimap.h"
#include "maze.h"
#include "console.h"
#include "common.h"

#include <time.h>
#include <stdlib.h> 
#include <vector>
#include <map>

struct MazeCell
{
    int x, y;
    bool visited;
    std::vector<MazeCell*> neighbors;
};

static float lastX;
static float lastY;
static bool firstMouse = true;
static std::map<Game::InputKey, bool> keyStates;

// use the even y indexes for the walls that separate the cells horizontally
// and the odd y indexes for the walls that separate the cells vertically
static bool walls[MAZE_HEIGHT * 2 - 1][MAZE_WIDTH];
static MazeCell cells[MAZE_HEIGHT][MAZE_WIDTH];

Game *Game::Instance()
{
    static Game instance;
    return &instance;
}

Game::Game()
{
    srand(time(0));
    // initialize maze cells
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            cells[y][x].x = x;
            cells[y][x].y = y;
            if (y > 0) {
                cells[y - 1][x].neighbors.push_back(&cells[y][x]);
                cells[y][x].neighbors.push_back(&cells[y - 1][x]);
            }
            if (y < MAZE_HEIGHT - 1) {
                cells[y + 1][x].neighbors.push_back(&cells[y][x]);
                cells[y][x].neighbors.push_back(&cells[y + 1][x]);
            }
            if (x > 0) {
                cells[y][x - 1].neighbors.push_back(&cells[y][x]);
                cells[y][x].neighbors.push_back(&cells[y][x - 1]);
            }
            if (x < MAZE_WIDTH - 1) {
                cells[y][x + 1].neighbors.push_back(&cells[y][x]);
                cells[y][x].neighbors.push_back(&cells[y][x + 1]);
            }
        }
    }
    resetMaze();
    generateMaze(&cells[rand() % MAZE_HEIGHT][rand() % MAZE_WIDTH]);
    m_minimap = new Minimap(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1, &camera);
    m_maze = new Maze(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1, &camera);

    camera.Position = glm::vec3(WALL_SIZE / 2.0f);
    if (!walls[0][1]) {
        camera.Yaw = 0.0f;
    }
    else {
        camera.Yaw = 90.0f;
    }
    // trigger an update in camera vectors
    camera.ProcessMouseMovement(0, 0);

    CONSOLE_DEBUG("Game [%p] created.", this);
}

Game::~Game()
{
    delete m_minimap;
    CONSOLE_DEBUG("Game [%p] destroyed.", this);
}

void Game::draw()
{
    m_maze->draw();
    if (m_showMinimap) {
        m_minimap->draw();
    }
}

void Game::reset()
{
    resetMaze();
    generateMaze(&cells[rand() % MAZE_HEIGHT][rand() % MAZE_WIDTH]);
    delete m_minimap;
    delete m_maze;
    m_minimap = new Minimap(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1, &camera);
    m_maze = new Maze(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1, &camera);

    camera.Position = glm::vec3(WALL_SIZE / 2.0f);
    camera.Pitch = 0.0f;
    if (!walls[0][1]) {
        camera.Yaw = 0.0f;
    }
    else {
        camera.Yaw = 90.0f;
    }
    // trigger an update in camera vectors
    camera.ProcessMouseMovement(0, 0);

    CONSOLE_DEBUG("Game [%p] was resetted.", this);
}

void Game::generateMaze(MazeCell *cell)
{
    std::vector<MazeCell*> neighbors = cell->neighbors;
    cell->visited = true;
    while (neighbors.size() > 0) {
        int i = rand() % neighbors.size();
        if (!neighbors[i]->visited) {
            // For the y index of the wall, get the minimum y index between the examined cells,
            // times 2 and add the difference of their indexes. That way, if they are on
            // the same y index, their difference will be 0, so wall that will be removed
            // will be on the even indexes, otherwise it would be on the odd ones.
            
            // For the x index of the wall, just get the maximum x index between the examined cells.
            // Keep in mind when processing the walls, the 0 x index should be ignored as the
            // implementation is now. That is because the walls that separate the cells horizontally
            // are one less than the walls that separate the cells vertically.
            walls[MIN(cell->y, neighbors[i]->y) * 2 + ABS(cell->y - neighbors[i]->y)][MAX(cell->x, neighbors[i]->x)] = false;
            generateMaze(neighbors[i]);
        }
        neighbors.erase(neighbors.begin() + i);
    }
}

void Game::resetMaze()
{
    // reset maze cells
    for (int y = 0; y < MAZE_HEIGHT; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            cells[y][x].visited = false;
        }
    }
    // reset maze walls
    for (int y = 0; y < MAZE_HEIGHT * 2 - 1; ++y) {
        for (int x = 0; x < MAZE_WIDTH; ++x) {
            walls[y][x] = true;
        }
    }
}

void Game::update(float deltaTime)
{
    if (keyStates[KEY_UP_1]) {
        camera.ProcessKeyboard(Camera::FORWARD, deltaTime);
    }
    if (keyStates[KEY_DOWN_1]) {
        camera.ProcessKeyboard(Camera::BACKWARD, deltaTime);
    }
    if (keyStates[KEY_LEFT_1]) {
        camera.ProcessKeyboard(Camera::LEFT, deltaTime);
    }
    if (keyStates[KEY_RIGHT_1]) {
        camera.ProcessKeyboard(Camera::RIGHT, deltaTime);
    }
    if (keyStates[KEY_MOVE_UP]) {
        camera.ProcessKeyboard(Camera::UP, deltaTime);
    }
    if (keyStates[KEY_MOVE_DOWN]) {
        camera.ProcessKeyboard(Camera::DOWN, deltaTime);
    }
    if (keyStates[KEY_UP_2]) {
        camera.ProcessMouseMovement(0, 1000 * deltaTime);
    }
    if (keyStates[KEY_DOWN_2]) {
        camera.ProcessMouseMovement(0, -1000 * deltaTime);
    }
    if (keyStates[KEY_LEFT_2]) {
        camera.ProcessMouseMovement(-1000 * deltaTime, 0);
    }
    if (keyStates[KEY_RIGHT_2]) {
        camera.ProcessMouseMovement(1000 * deltaTime, 0);
    }
    m_minimap->update();
}

void Game::processKeyInput(InputKey key, InputKeyState state)
{
    // conditional to handle undesired continuous key events
    if (!keyStates[key] && state == KEY_STATE_PRESSED) {
        switch (key) {
            case KEY_MINIMAP:
                m_showMinimap = !m_showMinimap;
                break;
            case KEY_RESET:
                reset();
                break;
            default:
                break;
        }
    }
    keyStates[key] = state == KEY_STATE_PRESSED;
}

void Game::processMouseInput(double xPos, double yPos)
{
    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xoffset = xPos - lastX;
    float yoffset = lastY - yPos; // reversed since y-coordinates go from bottom to top

    lastX = xPos;
    lastY = yPos;

    camera.ProcessMouseMovement(xoffset * 3, yoffset);
}