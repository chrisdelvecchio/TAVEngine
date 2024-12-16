#include "shader.h"

#include <string.h>

void reloadShaders(void) {
    if (isListEmpty(engine->shaders)) {
        printf("[TAV ENGINE] => No shaders to reload.\n");
        return;
    }

    int counter = 0;

    foreach (Shader *shader, engine->shaders) {
        glDeleteProgram(shader->programID);

        CompileShader(shader);
        counter++;
    }

    printf("[TAV ENGINE] => DONE.. Reloaded %d shaders successfully!\n", counter);
}

Shader *NewShader(const char *vertexPath, const char *fragmentPath) {
    Shader *shader = malloc(sizeof(Shader));
    if (shader == NULL) {
        printf("[Shader] => Memory allocation failed.\n");
        return NULL;
    }

    shader->programID = 0;
    shader->vertexPath = vertexPath;
    shader->fragmentPath = fragmentPath;

    // 1. Get the Full File Path's
    char *fullVertexPath = getShaderPath(vertexPath);
    char *fullFragmentPath = getShaderPath(fragmentPath);
    if (!fullVertexPath || !fullFragmentPath) {
        printf("[Shader] => Error constructing shader paths using #getShaderPath(const char* path)\n");
        free(shader);
        return NULL;
    }

    // 2. Read Contents from files && Compile Shaders
    CompileShader(shader);

    // 3. Free shader paths and codes
    free(fullVertexPath);
    free(fullFragmentPath);

    // 4. Add the shader to the engine's shader list
    ListAdd(engine->shaders, shader);
    printf("[Shader] '%s' & '%s'\n", vertexPath, fragmentPath);

    return shader;
}

void UseShader(Shader shader) {
    glUseProgram(shader.programID);
}

void setBool(Shader shader, const char *name, bool value) {
    glUniform1i(glGetUniformLocation(shader.programID, name), (int)value);
}
void setInt(Shader shader, const char *name, int value) {
    glUniform1i(glGetUniformLocation(shader.programID, name), value);
}
void setFloat(Shader shader, const char *name, float value) {
    glUniform1f(glGetUniformLocation(shader.programID, name), value);
}
void setVec2(Shader shader, const char *name, const vec2s *value) {
    glUniform2fv(glGetUniformLocation(shader.programID, name), 1, &value->raw[0]);
}
void setVec2F(Shader shader, const char *name, float x, float y) {
    glUniform2f(glGetUniformLocation(shader.programID, name), x, y);
}
void setVec3(Shader shader, const char *name, const vec3s *value) {
    glUniform3fv(glGetUniformLocation(shader.programID, name), 1, &value->raw[0]);
}
void setVec3F(Shader shader, const char *name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader.programID, name), x, y, z);
}
void setVec4(Shader shader, const char *name, const vec4s *value) {
    glUniform4fv(glGetUniformLocation(shader.programID, name), 1, &value->raw[0]);
}
void setVec4F(Shader shader, const char *name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader.programID, name), x, y, z, w);
}
void setMat2(Shader shader, const char *name, const mat2s *mat) {
    glUniformMatrix2fv(glGetUniformLocation(shader.programID, name), 1, GL_FALSE, &mat->raw[0][0]);
}
void setMat3(Shader shader, const char *name, const mat3s *mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader.programID, name), 1, GL_FALSE, &mat->raw[0][0]);
}
void setMat4(Shader shader, const char *name, const mat4s *mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader.programID, name), 1, GL_FALSE, &mat->raw[0][0]);
}