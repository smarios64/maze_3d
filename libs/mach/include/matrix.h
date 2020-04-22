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
 * File:   matrix.h
 * Author: Marios Christoforakis
 *
 * Created on July 9, 2017, 4:26 PM
 */

#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <GL/glew.h>

namespace mach
{
    template <std::size_t columns, std::size_t rows>
    class Matrix 
    {
        public:
            GLfloat m_p[columns][rows];
        
        public:
            Matrix();
            
            virtual ~Matrix();
            
            std::size_t numRows() const;
            std::size_t numColumns() const;
            
            GLfloat* operator[](std::size_t idx);
            
            Matrix<columns, rows> &operator*=(const Matrix<columns, rows> &other);
            Matrix<columns, rows> &operator*=(const GLfloat &scalar);
    };
}
    
template<std::size_t columns, std::size_t rows, std::size_t other_columns>
mach::Matrix<other_columns, rows> operator*(const mach::Matrix<columns, rows> &m1, const mach::Matrix<other_columns, columns> &m2);
    
template<std::size_t columns, std::size_t rows>
mach::Matrix<columns, rows> operator*(const mach::Matrix<columns, rows> &m, const GLfloat &scalar);
    
template<std::size_t columns, std::size_t rows>
mach::Matrix<columns, rows> operator*(const GLfloat &scalar, const mach::Matrix<columns, rows> &m);

#endif /* MATRIX_H */

