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
 * File:   minimap.cpp
 * Author: Marios Christoforakis
 *
 * Created on April 17, 2020, 10:47
 */

#include "minimap.h"
#include "console.h"
#include "shader.h"

#include <GL/glew.h>
#include <vector>
#include <algorithm>

namespace mach {
    bool operator==(const Vector2 &lhs, const Vector2 &rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }
}

static Shader minimapShader;

static void insertVertex(std::vector<mach::Vector2> &vertices, std::vector<GLuint> &indices, const mach::Vector2 &point);

Minimap::Minimap(bool *walls, const unsigned int width, const unsigned int height)
{
    if (minimapShader.id() == -1) {
        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 0.75f, 0.2f, 1.0f);\n"
            "}\0";
        minimapShader.compile(vertexShaderSource, fragmentShaderSource);
    }
    // inner walls
    std::vector<mach::Vector2> vertices;
    std::vector<GLuint> indices;
    const GLfloat h = 1.5 / (height + 1);
    const GLfloat w = 1.5 / width;
    
    // vertical walls
    for (int x = 1; x < width; ++x) {
        int startY = -1;
        int endY = -1;
        for (int y = 0; y < height + 2; y += 2) {
            if (y < height && walls[y * width + x]) {
                if (startY == -1) {
                    startY = y;
                }
                endY = y;
            }
            else if (startY != -1) {
                insertVertex(vertices, indices, { w * x - 0.75f, (h * startY - 0.75f) * -1 });
                insertVertex(vertices, indices, { w * x - 0.75f, (h * endY - 0.75f + h * 2) * -1 });
                numPoints += 2;
                endY = startY = -1;
            }
        }
    }
    
    // horizontal walls
    for (int y = 1; y < height; y += 2) {
        int startX = -1;
        int endX = -1;
        for (int x = 0; x <= width; ++x) {
            if (x < width && walls[y * width + x]) {
                if (startX == -1) {
                    startX = x;
                }
                endX = x;
            }
            else if (startX != -1) {
                insertVertex(vertices, indices, { w * startX - 0.75f, (h * y - 0.75f + h) * -1 });
                insertVertex(vertices, indices, { w * endX - 0.75f + w, (h * y - 0.75f + h) * -1 });
                numPoints += 2;
                endX = startX = -1;
            }
        }
    }

    // outer walls
    vertices.push_back({-0.75f, 0.75f});  // top left
    vertices.push_back({0.75f, 0.75f});   // top right
    vertices.push_back({0.75f, -0.75f});  // bottom right
    vertices.push_back({-0.75f, -0.75f}); // bottom left
    for (int i = 8; i > 1; --i) {
        indices.push_back(vertices.size() - i / 2);
    }
    indices.push_back(vertices.size() - 4);
    numPoints += 8;
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mach::Vector2) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(mach::Vector2), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 

    CONSOLE_DEBUG("Minimap [%p] created.", this);
}

Minimap::~Minimap()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    CONSOLE_DEBUG("Minimap [%p] destroyed.", this);
}

void Minimap::draw()
{
    minimapShader.use();
    glBindVertexArray(VAO);
    glLineWidth(2);
    glPolygonMode(GL_FRONT, GL_LINE);
    glDrawElements(GL_LINES, numPoints, GL_UNSIGNED_INT, 0);
    //glDrawArrays(GL_LINES, 0, numPoints);
    glBindVertexArray(0);
}

static void insertVertex(std::vector<mach::Vector2> &vertices, std::vector<GLuint> &indices, const mach::Vector2 &point)
{
    GLuint index;
    std::vector<mach::Vector2>::const_iterator itr = std::find(vertices.cbegin(), vertices.cend(), point);

    if (itr != vertices.cend()) {
        index = std::distance(vertices.cbegin(), itr);
    }
    else {
        index = vertices.size();
        vertices.push_back(point);
    }
    indices.push_back(index);
}