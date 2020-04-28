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
 * File:   shader.h
 * Author: Marios Christoforakis
 *
 * Created on June 10, 2017, 5:45 PM
 */

#ifndef SHADER_H
#define SHADER_H
#include <string>

#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/vector_relational.hpp>

class Shader
{    
public:
    // Constructor
    Shader() : ID(-1) { }
    
    ~Shader();
    // Sets the current shader as active
    void use();
    GLuint id() const;
    
    // Compiles the shader from given source code
    void compile(const GLchar *vertexSource, const GLchar *fragmentSource);
    
    // Utility functions
    void setFloat    (const GLchar *name, GLfloat value, GLboolean useShader = GL_FALSE);
    void setInteger  (const GLchar *name, GLint value, GLboolean useShader = GL_FALSE);
    void setVector2f (const GLchar *name, GLfloat x, GLfloat y, GLboolean useShader = GL_FALSE);
    void setVector2f (const GLchar *name, const glm::vec2 &value, GLboolean useShader = GL_FALSE);
    void setVector3f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLboolean useShader = GL_FALSE);
    void setVector3f (const GLchar *name, const glm::vec3 &value, GLboolean useShader = GL_FALSE);
    void setVector4f (const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w, GLboolean useShader = GL_FALSE);
    void setVector4f (const GLchar *name, const glm::vec4 &value, GLboolean useShader = GL_FALSE);
    void setMatrix4  (const GLchar *name, const glm::mat4 &matrix, GLboolean useShader = GL_FALSE);
    
private:
    // State
    GLuint ID; 
    
    // Checks if compilation or linking failed and if so, print the error logs
    void checkCompileErrors(GLuint object, std::string type); 
    
};

#endif /* SHADER_H */

