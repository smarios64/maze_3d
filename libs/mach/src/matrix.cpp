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
 * File:   matrix.cpp
 * Author: Marios Christoforakis
 *
 * Created on July 9, 2017, 4:26 PM
 */

#include "matrix.h"
#include <cassert>

using namespace mach;

template <std::size_t columns, std::size_t rows>
Matrix<columns, rows>::Matrix()
{
    std::fill(&m_p[0][0], &m_p[0][0] + rows * columns, 0.0);
    if (rows == columns) {
        for (std::size_t i = 0; i < rows; ++i) {
            m_p[i][i] = 1.0;
        }
    }
}

template <std::size_t columns, std::size_t rows>
Matrix<columns, rows>::~Matrix() { }

template <std::size_t columns, std::size_t rows>
std::size_t Matrix<columns, rows>::numRows() const { return rows; }

template <std::size_t columns, std::size_t rows>
std::size_t Matrix<columns, rows>::numColumns() const { return columns; }

template <std::size_t columns, std::size_t rows>
GLfloat* Matrix<columns, rows>::operator[](std::size_t idx) { return m_p[idx]; }

template <std::size_t columns, std::size_t rows>
Matrix<columns, rows> &Matrix<columns, rows>::operator*=(const Matrix<columns, rows> &other) 
{
    assert(rows == columns);
    GLfloat p[columns][rows];
    for (std::size_t i = 0; i < columns; ++i) {
        for (std::size_t j = 0; j < rows; ++j) {
            p[i][j] = 0.0;
            for (std::size_t k = 0; k < rows; ++k) {
                p[i][j] += m_p[i][k] * other.m_p[k][j];
            }
        }
    }
    
    std::copy(&p[0][0], &p[0][0] + rows * columns, &m_p[0][0]);
    return *this;
}

template <std::size_t columns, std::size_t rows>
Matrix<columns, rows> &Matrix<columns, rows>::operator*=(const GLfloat &scalar) 
{
    for (std::size_t i = 0; i < columns; ++i) {
        for (std::size_t j = 0; j < rows; ++j) {
            m_p[i][j] *= scalar;
        }
    }
    return *this;
}

template<std::size_t columns, std::size_t rows, std::size_t other_columns>
Matrix<other_columns, rows> operator*(const Matrix<columns, rows> &m1, const Matrix<other_columns, columns> &m2)
{ 
    Matrix<other_columns, rows> m;
    for (std::size_t i = 0; i < other_columns; ++i) {
        for (std::size_t j = 0; j < rows; ++j) {
            m.m_p[i][j] = 0.0;
            for (std::size_t k = 0; k < columns; ++k) {
                m.m_p[i][j] += m1.m_p[k][j] * m2.m_p[i][k];
            }
        }
    }
    return m;
}

template<std::size_t columns, std::size_t rows>
Matrix<columns, rows> operator*(const Matrix<columns, rows> &m, const GLfloat &scalar)
{
    Matrix<columns, rows> matrix = m;
    matrix *= scalar;
    return matrix;
}

template<std::size_t columns, std::size_t rows>
Matrix<columns, rows> operator*(const GLfloat &scalar, const Matrix<columns, rows> &m)
{
    return m * scalar;
}

//////////// Define matrix types /////////////

template class Matrix<3, 3>;
template class Matrix<4, 4>;

template Matrix<3, 3> operator*(const Matrix<3, 3> &m1, const Matrix<3, 3> &m2);
template Matrix<4, 4> operator*(const Matrix<4, 4> &m1, const Matrix<4, 4> &m2);
    
template Matrix<3, 3> operator*(const Matrix<3, 3> &m, const GLfloat &scalar);
template Matrix<4, 4> operator*(const Matrix<4, 4> &m, const GLfloat &scalar);
    
template Matrix<3, 3> operator*(const GLfloat &scalar, const Matrix<3, 3> &m);
template Matrix<4, 4> operator*(const GLfloat &scalar, const Matrix<4, 4> &m);