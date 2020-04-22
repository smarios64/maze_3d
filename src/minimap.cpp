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
#include "vector.h"

#include <GL/glew.h>
#include <vector>
#include <algorithm>

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

static int shaderProgram = -1;

static void insertVertex(std::vector<mach::Vector2> &vertices, std::vector<GLuint> &indices, const mach::Vector2 &point);

Minimap::Minimap(bool *walls, const unsigned int width, const unsigned int height)
{
    if (shaderProgram == -1) {
        // build and compile our shader program
        // ------------------------------------
        // vertex shader
        int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            CONSOLE_ERROR("SHADER::VERTEX::COMPILATION_FAILED -> %s", infoLog);
        }
        // fragment shader
        int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            CONSOLE_ERROR("SHADER::FRAGMENT::COMPILATION_FAILED -> %s", infoLog);
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            CONSOLE_ERROR("SHADER::PROGRAM::LINKING_FAILED -> %s", infoLog);
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // inner walls
    std::vector<mach::Vector2> vertices;
    std::vector<GLuint> indices;
    const GLfloat h = 1.5 / (height + 1);
    const GLfloat w = 1.5 / width;
    for (int y = 0; y < height; ++y) {
        for (int x = (y % 2 == 0 ? 1 : 0); x < width; ++x) {
            if (walls[y * width + x]) {
                insertVertex(vertices, indices, { w * x - 0.75f, h * y - 0.75f + (y % 2 == 0 ? 0 : h) });
                insertVertex(vertices, indices, { w * x - 0.75f + (y % 2 == 0 ? 0 : w), h * y - 0.75f + (y % 2 == 0 ? h * 2 : h) });
                numPoints += 2;
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
    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    CONSOLE_DEBUG("Minimap [%p] destroyed.", this);
}

void Minimap::draw()
{
    glUseProgram(shaderProgram);
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