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
#include "player.h"
#include "shader.h"
#include "common.h"

#include <GL/glew.h>
#include <vector>
#include <algorithm>

struct VertexData2D
{
    glm::vec2 position;
    glm::vec3 color;
};

bool operator==(const VertexData2D &lhs, const VertexData2D &rhs)
{
    return lhs.position == rhs.position && lhs.color == rhs.color;
}

static Shader minimapShader;
static VertexData2D playerData = { { 0.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } };

static void insertVertex(std::vector<VertexData2D> &vertices, std::vector<GLuint> &indices, const VertexData2D &point);

Minimap::Minimap(bool *walls, glm::vec3 *playerPos)
    : numPoints(0), playerPos(playerPos)
{
    if (minimapShader.id() == -1) {
        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "layout (location = 1) in vec3 color;\n"
            "out vec3 outColor;\n"
            "void main()\n"
            "{\n"
            "  outColor = color;\n"
            "  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec3 outColor;\n"
            "void main()\n"
            "{\n"
            "  FragColor = vec4(outColor, 0.7f);\n"
            "}\0";
        minimapShader.compile(vertexShaderSource, fragmentShaderSource);
    }
    // inner walls
    std::vector<VertexData2D> vertices;
    std::vector<GLuint> indices;
    const GLfloat h = MINIMAP_HEIGHT / (MAZE_HEIGHT * 2);
    const GLfloat w = MINIMAP_WIDTH / MAZE_WIDTH;
    const glm::vec3 wallsColor(1.0f, 1.0f, 0.0f);

    // background
    vertices.push_back({ {MINIMAP_X, MINIMAP_Y}, { 0.0f, 0.0f, 0.0f } });  // top left
    vertices.push_back({ {MINIMAP_X, MINIMAP_Y - MINIMAP_HEIGHT}, { 0.0f, 0.0f, 0.0f } }); // bottom left
    vertices.push_back({ {MINIMAP_X + MINIMAP_WIDTH, MINIMAP_Y}, { 0.0f, 0.0f, 0.0f } });   // top right
    vertices.push_back({ {MINIMAP_X + MINIMAP_WIDTH, MINIMAP_Y - MINIMAP_HEIGHT}, { 0.0f, 0.0f, 0.0f } });  // bottom right
    
    // vertical walls
    for (int x = 1; x < MAZE_WIDTH; ++x) {
        int startY = -1;
        int endY = -1;
        for (int y = 0; y <= MAZE_HEIGHT * 2; y += 2) {
            if (y < MAZE_HEIGHT * 2 - 1 && walls[y * MAZE_WIDTH + x]) {
                if (startY == -1) {
                    startY = y;
                }
                endY = y;
            }
            else if (startY != -1) {
                insertVertex(vertices, indices, { { w * x + MINIMAP_X, (h * startY - MINIMAP_Y) * -1 }, wallsColor });
                insertVertex(vertices, indices, { { w * x + MINIMAP_X, (h * endY - MINIMAP_Y + h * 2) * -1 }, wallsColor });
                numPoints += 2;
                endY = startY = -1;
            }
        }
    }
    
    // horizontal walls
    for (int y = 1; y < MAZE_HEIGHT * 2 - 1; y += 2) {
        int startX = -1;
        int endX = -1;
        for (int x = 0; x <= MAZE_WIDTH; ++x) {
            if (x < MAZE_WIDTH && walls[y * MAZE_WIDTH + x]) {
                if (startX == -1) {
                    startX = x;
                }
                endX = x;
            }
            else if (startX != -1) {
                insertVertex(vertices, indices, { { w * startX + MINIMAP_X, (h * y -  MINIMAP_Y + h) * -1 }, wallsColor });
                insertVertex(vertices, indices, { { w * endX + MINIMAP_X + w, (h * y - MINIMAP_Y + h) * -1 }, wallsColor });
                numPoints += 2;
                endX = startX = -1;
            }
        }
    }

    // outer walls
    vertices.push_back({ vertices[3].position, wallsColor });  // bottom right
    vertices.push_back({ vertices[2].position, wallsColor });   // top right
    vertices.push_back({ vertices[0].position, wallsColor });  // top left
    vertices.push_back({ vertices[1].position, wallsColor }); // bottom left
    for (int i = 8; i > 1; --i) {
        indices.push_back(vertices.size() - i / 2);
    }
    indices.push_back(vertices.size() - 4);
    numPoints += 8;
    
    CONSOLE_DEBUG("Vertices count: %d", vertices.size());
    CONSOLE_DEBUG("Points count: %d", numPoints);
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData2D) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData2D), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData2D), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);
    
    glGenVertexArrays(1, &player_VAO);
    glGenBuffers(1, &player_VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(player_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, player_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(playerData), &playerData, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(playerData), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(playerData), (void*)(sizeof(glm::vec2)));
    glEnableVertexAttribArray(1);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0); 
    glLineWidth(2);
    glPointSize(7);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    CONSOLE_DEBUG("Minimap [%p] created.", this);
}

Minimap::~Minimap()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &player_VAO);
    glDeleteBuffers(1, &player_VBO);
    glDeleteBuffers(1, &EBO);

    CONSOLE_DEBUG("Minimap [%p] destroyed.", this);
}

void Minimap::update()
{
    playerData.position.x = playerPos->x / ((WALL_SIZE + WALL_THICKNESS) * MAZE_WIDTH - WALL_THICKNESS) * MINIMAP_WIDTH + MINIMAP_X;
    playerData.position.y = -(playerPos->z / ((WALL_SIZE + WALL_THICKNESS) * MAZE_HEIGHT - WALL_THICKNESS) * MINIMAP_HEIGHT - MINIMAP_Y);
}

void Minimap::draw()
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    minimapShader.use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawElements(GL_LINES, numPoints, GL_UNSIGNED_INT, 0);
    glBindBuffer(GL_ARRAY_BUFFER, player_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(playerData.position), &playerData.position);
    glBindVertexArray(player_VAO);
    glDrawArrays(GL_POINTS, 0, 1);
    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

static void insertVertex(std::vector<VertexData2D> &vertices, std::vector<GLuint> &indices, const VertexData2D &point)
{
    GLuint index;
    std::vector<VertexData2D>::const_iterator itr = std::find(vertices.cbegin(), vertices.cend(), point);

    if (itr != vertices.cend()) {
        index = std::distance(vertices.cbegin(), itr);
    }
    else {
        index = vertices.size();
        vertices.push_back(point);
    }
    indices.push_back(index);
}