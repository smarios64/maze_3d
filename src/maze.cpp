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
 * File:   maze.cpp
 * Author: Marios Christoforakis
 *
 * Created on April 23, 2020, 09:58
 */

#include "maze.h"
#include "common.h"
#include "shader.h"
#include "console.h"
#include "camera.h"

#define STB_IMAGE_IMPLEMENTATION // nessesary to use stb_image.h
#include "stb_image.h"

#include <GL/glew.h>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

struct VertexData
{
    glm::vec3 position;
    glm::vec2 texCoords;
};

bool operator==(const VertexData &lhs, const VertexData &rhs)
{
    return lhs.position == rhs.position && lhs.texCoords == rhs.texCoords;
}

static void insertVertex(std::vector<VertexData> &vertices, std::vector<GLuint> &indices, const VertexData &point);
static unsigned int makeTexture(const char *texturePath);
static Shader mazeShader;

static unsigned int wallTexture, floorTexture;

Maze::Maze(bool *walls, const unsigned int width, const unsigned int height, Camera *camera)
    : numPoints(0), camera(camera)
{
    const unsigned int m_height = (height + 1) / 2.0f;
    if (mazeShader.id() == -1) {
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(60.0f), SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);

        const char *vertexShaderSource = "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec2 aTexCoord;\n"
            "out vec2 TexCoord;\n"
            "uniform mat4 view;\n"
            "uniform mat4 projection;\n"
            "void main()\n"
            "{\n"
            "    gl_Position = projection * view * vec4(aPos, 1.0);\n"
            "    TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
            "}\0";
        const char *fragmentShaderSource = "#version 330 core\n"
            "out vec4 FragColor;\n"
            "in vec2 TexCoord;\n"
            "uniform sampler2D texture1;\n"
            "void main()\n"
            "{\n"
            "    FragColor = texture(texture1, TexCoord);\n"
            "}\0";
        mazeShader.compile(vertexShaderSource, fragmentShaderSource);
        mazeShader.setMatrix4("projection", projection, GL_TRUE);

        wallTexture = makeTexture("./resources/textures/wall_diffuse.jpg");
        floorTexture = makeTexture("./resources/textures/ground_diffuse.jpg");
    }

    // inner walls
    std::vector<VertexData> vertices;
    std::vector<GLuint> indices;
    VertexData topLeft, topRight, bottomLeft, bottomRight;

    const float columnTextureSize = 1.0f * COLUMN_SIZE / WALL_SIZE;

    // floor
    vertices.push_back( { { 0.0f, 0.0f, 0.0f }, { 0.0f, m_height } } );
    vertices.push_back( { { 0.0f, 0.0f, (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE }, { 0.0f, 0.0f } } );
    vertices.push_back( { { (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE, 0.0f, 0.0f }, { width, m_height } } );
    vertices.push_back( { { (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE, 0.0f, (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE }, { width, 0.0f } } );
    
    // vertical walls
    for (int x = 1; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            bool flag = true;
            if (y % 2 == 0) {
                flag = walls[y * width + x];
                if (flag) {
                    topLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, WALL_SIZE, (WALL_SIZE + COLUMN_SIZE) * (y / 2) }, { (y / 2) * (1.0f + columnTextureSize), 1.0f } };
                    topRight = { { (WALL_SIZE + COLUMN_SIZE) * x, WALL_SIZE, (WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE }, { (y / 2) * (1.0f + columnTextureSize) + 1.0f, 1.0f } };
                    bottomLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, 0, (WALL_SIZE + COLUMN_SIZE) * (y / 2) }, { (y / 2) * (1.0f + columnTextureSize), 0.0f } };
                    bottomRight = { { (WALL_SIZE + COLUMN_SIZE) * x, 0, (WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE }, { (y / 2) * (1.0f + columnTextureSize) + 1.0f, 0.0f } };
                }
            }
            else {
                // vertical columns
                topLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, WALL_SIZE, (WALL_SIZE + COLUMN_SIZE) * ((y + 1) / 2) - COLUMN_SIZE }, { (y / 2) * (1.0f + columnTextureSize) + 1.0f, 1.0f } };
                topRight = { { (WALL_SIZE + COLUMN_SIZE) * x, WALL_SIZE, (WALL_SIZE + COLUMN_SIZE) * ((y + 1) / 2) }, { (y / 2) * (1.0f + columnTextureSize) + columnTextureSize + 1.0f, 1.0f } };
                bottomLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, 0, (WALL_SIZE + COLUMN_SIZE) * ((y + 1) / 2) - COLUMN_SIZE }, { (y / 2) * (1.0f + columnTextureSize) + 1.0f, 0.0f } };
                bottomRight = { { (WALL_SIZE + COLUMN_SIZE) * x, 0, (WALL_SIZE + COLUMN_SIZE) * ((y + 1) / 2) }, { (y / 2) * (1.0f + columnTextureSize) + columnTextureSize + 1.0f, 0.0f } };
            }
            if (flag) {
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, topRight );
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, bottomLeft);
                insertVertex(vertices, indices, topLeft);
                
                topLeft.position.x -= COLUMN_SIZE;
                topRight.position.x -= COLUMN_SIZE;
                bottomLeft.position.x -= COLUMN_SIZE;
                bottomRight.position.x -= COLUMN_SIZE;
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, topRight );
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, bottomLeft);
                numPoints += 12;
            }
        }
    }
    
    // horizontal walls
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            bool flag = true;
            if (y % 2 == 1) {
                flag = walls[y * width + x];
                if (flag) {
                    topLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, WALL_SIZE, ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize), 1.0f } };
                    topRight = { { (WALL_SIZE + COLUMN_SIZE) * x + WALL_SIZE, WALL_SIZE, ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + 1.0f, 1.0f } };
                    bottomLeft = { { (WALL_SIZE + COLUMN_SIZE) * x, 0, ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize), 0.0f } };
                    bottomRight = { { (WALL_SIZE + COLUMN_SIZE) * x + WALL_SIZE, 0, ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + 1.0f, 0.0f } };
                }
            }
            else {
                if (x == width - 1 || y == height - 1) {
                    continue;
                }
                // horizontal columns
                topLeft = { { (WALL_SIZE + COLUMN_SIZE) * (x + 1) - COLUMN_SIZE, WALL_SIZE,  ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + 1.0f, 1.0f } };
                topRight = { { (WALL_SIZE + COLUMN_SIZE) * (x + 1), WALL_SIZE,  ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + columnTextureSize + 1.0f, 1.0f } };
                bottomLeft = { { (WALL_SIZE + COLUMN_SIZE) * (x + 1) - COLUMN_SIZE, 0,  ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + 1.0f, 0.0f } };
                bottomRight = { { (WALL_SIZE + COLUMN_SIZE) * (x + 1), 0,  ((WALL_SIZE + COLUMN_SIZE) * (y / 2) + WALL_SIZE) }, { x * (1.0f + columnTextureSize) + columnTextureSize + 1.0f, 0.0f } };
            }
            if (flag) {
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, topRight );
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, bottomLeft);
                insertVertex(vertices, indices, topLeft);
                
                topLeft.position.z += COLUMN_SIZE;
                topRight.position.z += COLUMN_SIZE;
                bottomLeft.position.z += COLUMN_SIZE;
                bottomRight.position.z += COLUMN_SIZE;
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, topRight );
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, bottomRight);
                insertVertex(vertices, indices, topLeft);
                insertVertex(vertices, indices, bottomLeft);
                numPoints += 12;
            }
        }
    }

    // top outer wall
    topLeft = { { 0.0f, WALL_SIZE, 0.0f }, { 0.0f, 1.0f } };
    topRight = { { (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE, WALL_SIZE, 0.0f }, { width, 1.0f } };
    bottomLeft = { { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f } };
    bottomRight = { { (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE, 0.0f, 0.0f }, { width, 0.0f } };
    insertVertex(vertices, indices, topLeft);
    insertVertex(vertices, indices, bottomLeft);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, topRight );
    insertVertex(vertices, indices, topLeft);

    // bottom outer wall
    topLeft.position.z = (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE;
    topRight.position.z = (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE;
    bottomLeft.position.z = (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE;
    bottomRight.position.z = (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE;
    insertVertex(vertices, indices, topLeft);
    insertVertex(vertices, indices, topRight );
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomLeft);
    insertVertex(vertices, indices, topLeft);

    // left outer wall
    topLeft = { { 0.0f, WALL_SIZE, (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE }, { 0.0f, 1.0f } };
    topRight = { { 0.0f, WALL_SIZE, 0.0f }, { m_height, 1.0f } };
    bottomLeft = { { 0.0f, 0.0f, (WALL_SIZE + COLUMN_SIZE) * m_height - COLUMN_SIZE }, { 0.0f, 0.0f } };
    bottomRight = { { 0.0f, 0.0f, 0.0f }, { m_height, 0.0f } };
    insertVertex(vertices, indices, topLeft);
    insertVertex(vertices, indices, bottomLeft);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, topRight );
    insertVertex(vertices, indices, topLeft);

    // right outer wall
    topLeft.position.x = (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE;
    topRight.position.x = (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE;
    bottomLeft.position.x = (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE;
    bottomRight.position.x = (WALL_SIZE + COLUMN_SIZE) * width - COLUMN_SIZE;
    insertVertex(vertices, indices, topLeft);
    insertVertex(vertices, indices, topRight );
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomRight);
    insertVertex(vertices, indices, bottomLeft);
    insertVertex(vertices, indices, topLeft);

    numPoints += 24;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexData) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);


    
    CONSOLE_DEBUG("Maze [%p] created.", this);
}

Maze::~Maze()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    CONSOLE_DEBUG("Maze [%p] destroyed.", this);
}

void Maze::draw()
{

    // render container
    mazeShader.use();

    // create transformations
    glm::mat4 view = camera->GetViewMatrix();
    mazeShader.setMatrix4("view", view);

    glBindVertexArray(VAO);
    // bind Texture
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    // bind Texture
    glBindTexture(GL_TEXTURE_2D, wallTexture);
    glDrawElements(GL_TRIANGLES, numPoints, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

static void insertVertex(std::vector<VertexData> &vertices, std::vector<GLuint> &indices, const VertexData &point)
{
    GLuint index;
    std::vector<VertexData>::const_iterator itr = std::find(vertices.cbegin(), vertices.cend(), point);

    if (itr != vertices.cend()) {
        index = std::distance(vertices.cbegin(), itr);
    }
    else {
        index = vertices.size();
        vertices.push_back(point);
    }
    indices.push_back(index);
}

static unsigned int makeTexture(const char *texturePath)
{
    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int w, h, nrChannels;
    
    unsigned char *data = stbi_load(texturePath, &w, &h, &nrChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        CONSOLE_ERROR("Failed to load texture");
    }
    stbi_image_free(data);
    return texture;
}