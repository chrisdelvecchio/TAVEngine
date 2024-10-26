#pragma once

#ifndef SHADER_H
#define SHADER_H

#include "game.h"
#include <stdbool.h>

void reloadShaders(void);
Shader *NewShader(const char *vertexPath, const char *fragmentPath);
void UseShader(Shader shader);

void setBool(Shader shader, const char *name, bool value);
void setInt(Shader shader, const char *name, int value);
void setFloat(Shader shader, const char *name, float value);
void setVec2(Shader shader, const char *name, const vec2s *value);
void setVec2F(Shader shader, const char *name, float x, float y);
void setVec3(Shader shader, const char *name, const vec3s *value);
void setVec3F(Shader shader, const char *name, float x, float y, float z);
void setVec4(Shader shader, const char *name, const vec4s *value);
void setVec4F(Shader shader, const char *name, float x, float y, float z, float w);
void setMat2(Shader shader, const char *name, const mat2s *mat);
void setMat3(Shader shader, const char *name, const mat3s *mat);
void setMat4(Shader shader, const char *name, const mat4s *mat);

static char *getShaderPath(const char *path) {
    return (char *)CreatePath(game->shaderDir, path);
}

static void freeShaders(void) {
    if (isListEmpty(game->shaders)) {
        printf("[Game] No shaders to free.\n");
        return;
    }

    int counter = 0;
    foreach (Shader *shader, game->shaders) {
        glDeleteProgram(shader->programID);
        counter++;
    }

    ListClear(game->shaders);
    printf("[Game] %d Shaders have been freed!\n", counter);
}

static void checkCompileErrors(GLuint shader, char *type) {
    int success;
    char infoLog[1024];

    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            printf("[SHADER ERROR] -> SHADER_COMPILATION_ERROR of type: %s\n[LOG]: %s\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("[SHADER ERROR] -> PROGRAM_LINKING_ERROR of type: %s\n[LOG]: %s\n", type, infoLog);
        }
    }
}

static void ReadContents(Shader *shader) {
    char *fullVertexPath = getShaderPath(shader->vertexPath);
    char *fullFragmentPath = getShaderPath(shader->fragmentPath);
        if (!fullVertexPath || !fullFragmentPath) {
        printf("[Shader] => Error constructing shader paths using #getShaderPath(const char* path)\n");
        free(shader);
        return;
    }

    const char *vShaderCode = ReadAll(fullVertexPath);
    const char *fShaderCode = ReadAll(fullFragmentPath);

    if (vShaderCode == NULL) {
        printf("[Shader] => NULLPOINTEREXCEPTION: Could not read Vertex Shader %s\n", fullVertexPath);
        free(fullVertexPath);
        free(fullFragmentPath);
        free(shader);
        return;
    }

    if (fShaderCode == NULL) {
        printf("[Shader] => NULLPOINTEREXCEPTION: Could not read Fragment Shader %s\n", fullFragmentPath);
        free((void *)vShaderCode);
        free(fullVertexPath);
        free(fullFragmentPath);
        free(shader);
        return;
    }

    shader->vShaderCode = vShaderCode;
    shader->fShaderCode = fShaderCode;

    free(fullVertexPath);
    free(fullFragmentPath);
}

static void CompileShader(Shader *shader) {
    GLuint vertex, fragment;

    ReadContents(shader);

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &shader->vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &shader->fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    shader->programID = glCreateProgram();
    glAttachShader(shader->programID, vertex);
    glAttachShader(shader->programID, fragment);
    glLinkProgram(shader->programID);
    checkCompileErrors(shader->programID, "PROGRAM");

    // Delete the shaders as they're linked into the program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

#endif  // SHADER_H