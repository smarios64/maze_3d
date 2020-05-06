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
 * File:   game.h
 * Author: Marios Christoforakis
 *
 * Created on April 20, 2020, 14:51
 */

#ifndef GAME_H
#define GAME_H

class MazeCell;
class Minimap;
class Maze;

class Game
{
public:
    enum InputKey {
        KEY_UP_1,
        KEY_DOWN_1,
        KEY_LEFT_1,
        KEY_RIGHT_1,
        KEY_UP_2,
        KEY_DOWN_2,
        KEY_LEFT_2,
        KEY_RIGHT_2,
        KEY_MOVE_UP,
        KEY_MOVE_DOWN,
        KEY_RESET
    };

    enum InputKeyState {
        KEY_STATE_PRESSED,
        KEY_STATE_RELEASED
    };

    static Game *Instance();

    void update(float deltaTime);
    void draw();
    void processKeyInput(InputKey key, InputKeyState state);
    void processMouseInput(double xPos, double yPos);
    
private:
    Game();
    virtual ~Game();

    void generateMaze(MazeCell *cell);
    void resetMaze();
    void reset();

    Minimap *m_minimap;
    Maze    *m_maze;
};

#endif
