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
 * File:   minimap.h
 * Author: Marios Christoforakis
 *
 * Created on April 17, 2020, 10:47
 */

#ifndef MINIMAP_H
#define MINIMAP_H

class Minimap
{
public: 
    Minimap(bool *walls, const unsigned int width, const unsigned int height);
    virtual ~Minimap();

    void draw();
    
private:
    unsigned int VBO, VAO, EBO;
    unsigned int numPoints;
};

#endif
