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
#include "console.h"

#include <time.h>
#include <stdlib.h> 
#include <vector>

#define MAZE_WIDTH 16
#define MAZE_HEIGHT 12
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))

struct MazeCell
{
    int x, y;
    bool visited;
    std::vector<MazeCell*> neighbors;
};

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
    m_minimap = new Minimap(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1);
    CONSOLE_DEBUG("Game [%p] created.", this);
}

Game::~Game()
{
    delete m_minimap;
    CONSOLE_DEBUG("Game [%p] destroyed.", this);
}

void Game::draw()
{
    m_minimap->draw();
}

void Game::reset()
{
    resetMaze();
    generateMaze(&cells[rand() % MAZE_HEIGHT][rand() % MAZE_WIDTH]);
    delete m_minimap;
    m_minimap = new Minimap(walls[0], MAZE_WIDTH, MAZE_HEIGHT * 2 - 1);
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