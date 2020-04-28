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
 * File:   shader.cpp
 * Author: Marios Christoforakis
 * 
 * Created on June 10, 2017, 5:45 PM
 */

#include <cassert>

#include "shader.h"
#include "console.h"

#define USE_SHADER(useShader) if (useShader) this->use()

Shader::~Shader()
{
    glDeleteProgram(ID);
    CONSOLE_INFO("Deleted Shader with ID [%d]", ID);
}

void Shader::use()
{
    glUseProgram(this->ID);
}

void Shader::compile(const GLchar* vertexSource, const GLchar* fragmentSource)
{
    GLuint sVertex, sFragment;
    // Vertex Shader
    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertexSource, NULL);
    glCompileShader(sVertex);
    checkCompileErrors(sVertex, "VERTEX");
    // Fragment Shader
    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragmentSource, NULL);
    glCompileShader(sFragment);
    checkCompileErrors(sFragment, "FRAGMENT");
    
    // Shader Program
    this->ID = glCreateProgram();
    CONSOLE_INFO("Created Shader with ID [%d]", ID);
    glAttachShader(this->ID, sVertex);
    glAttachShader(this->ID, sFragment);
    glLinkProgram(this->ID);
    checkCompileErrors(this->ID, "PROGRAM");
    // Delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

void Shader::setFloat(const GLchar *name, GLfloat value, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::setInteger(const GLchar *name, GLint value, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::setVector2f(const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::setVector2f(const GLchar *name, const glm::vec2 &value, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::setVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::setVector3f(const GLchar *name, const glm::vec3 &value, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::setVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::setVector4f(const GLchar *name, const glm::vec4 &value, GLboolean useShader)
{
    USE_SHADER(useShader);
    glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}

void Shader::checkCompileErrors(GLuint object, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(object, 1024, NULL, infoLog);
            CONSOLE_ERROR("SHADER: Compile-time error: Type: %s -> %s" , type.c_str(), infoLog);
        }
    }
    else
    {
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(object, 1024, NULL, infoLog);
            CONSOLE_ERROR("SHADER: Link-time error: Type: %s -> %s" , type.c_str(), infoLog);
        }
    }
    assert(success);
}

void Shader::setMatrix4(const GLchar *name, const glm::mat4 &matrix, GLboolean useShader)
{
    USE_SHADER(useShader);    
    glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, &matrix[0][0]);
}

GLuint Shader::id() const
{
    return ID;
}