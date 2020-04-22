/*
 * Copyright (C) 2017 Marios Christoforakis
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
 * File:   vector.h
 * Author: Marios Christoforakis
 *
 * Created on July 11, 2017, 7:58 PM
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <GL/glew.h>

namespace mach
{
    class Vector2
    {
    public:
        GLfloat x, y;
        
        Vector2()
            : x(0), y(0) { }
        
        Vector2(GLfloat x, GLfloat y)
            : x(x), y(y) { }
        
        Vector2(GLfloat val)
            : Vector2(val, val) { }
    };
    
    class Vector3 : public Vector2
    {
    public:
        GLfloat z;
        
        Vector3()
            : Vector2(), z(0) { }
        
        Vector3(GLfloat x, GLfloat y, GLfloat z)
            : Vector2(x, y), z(z) { }
        
        Vector3(GLfloat val)
            : Vector3(val, val, val) { }
    };
    
    class Vector4 : public Vector3
    {
    public:
        GLfloat w;
        
        Vector4()
            : Vector3(), w(0) { }
        
        Vector4(GLfloat x, GLfloat y, GLfloat z, GLfloat w)
            : Vector3(x, y, z), w(w) { }
        
        Vector4(GLfloat val)
            : Vector4(val, val, val, val) { }
    };

    bool operator==(const Vector2 &lhs, const Vector2 &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
    bool operator==(const Vector3 &lhs, const Vector3 &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z; }
    bool operator==(const Vector4 &lhs, const Vector4 &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w; }
}

#endif /* VECTOR_H */

