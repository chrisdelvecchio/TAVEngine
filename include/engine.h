#pragma once

#ifndef ENGINE_H
#define ENGINE_H

#include <cglm/struct.h>
#include <glad/glad.h>
#include <glfw3.h>

#include <stdbool.h>

#include "liblist.h"
#include "libio.h"
#include "libmap.h"
#include "nanovg.h"
#include "timings.h"

#define BACKGROUND_COLOR 0.53f, 0.81f, 0.98f, 1.0f
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define UPDATE_INTERVAL 1.0f / 60.0f

typedef struct Engine {
    GLFWwindow *window;
    float windowWidth, windowHeight, aspectRatio, fps, deltaTime, lastX, lastY;

    NVGcontext *vgContext;
    int defaultFont;

    char *mainPath, *settingsFile, *shaderDir, *assetDir, *fontDir;
    List *sceneObjects, *shaders;

    bool vSync, antiAliasing, wireframeMode, firstMouse;
} Engine;

typedef struct Transform {
    mat4s model;
    vec3s position, rotation, scale;
    float rotationDegrees;
} Transform;

typedef struct Camera {
    mat4s projection, view;

    vec3s position;
    vec3s front;
    vec3s up;
    vec3s right;
    vec3s worldUp;
    vec3s velocity;

    float yaw;
    float pitch;

    float renderDistance;
    float movementSpeed;
    float maxVelocity;
    float mouseSensitivity;
    float fov;

    void (*update)(struct Camera *self);
} Camera;

typedef struct Shader {
    GLuint programID;

    const char *vertexPath;
    const char *fragmentPath;
    const char *vShaderCode;
    const char *fShaderCode;
} Shader;

typedef struct Texture {
    GLuint textureID;

    int width, height, nrChannels;
} Texture;

typedef struct Sprite {
    Texture texture;
    float x, y, width, height;
} Sprite;

typedef enum ObjectType {
    OBJECT_NONE,
    OBJECT_LIGHT,
    OBJECT_3D,
    OBJECT_2D,
    OBJECT_FLOOR,
    OBJECT_FRAMEBUFFER_QUAD
} ObjectType;

typedef struct Vertex {
    vec3s position;
    vec3s normal;
    vec2s texCoords;
} Vertex;

typedef struct MeshData {
    Vertex *verticesCopy;
    GLuint *indicesCopy;
} MeshData;

typedef struct SceneObject {
    char *tag;

    ObjectType type;
    Transform *transforms;

    Shader *shader;
    Sprite *sprite;
    Texture *texture;

    MeshData *meshData;
    GLuint VAO, VBO, EBO, IVBO;
    Vertex *vertices;
    GLuint *indices;

    int indexCount, vertexCount, instanceCount;

    vec3s color;

    void (*draw)(struct SceneObject *self);
} SceneObject;

typedef struct FrameBufferObject {
    GLuint frameBufferID;
    GLuint texColorBufferID;
    GLuint depthStencilBufferID;
    GLuint intermediateFBO;
    GLuint screenTexture;

    int bufferWidth;
    int bufferHeight;

    SceneObject *quad;
    void (*drawBuffer)(struct FrameBufferObject *self);
} FrameBufferObject;

typedef struct Skybox {
    List *textureNames;
    GLuint textureID;
    GLuint VAO, VBO;

    void (*draw)(struct Skybox *self);
    void (*free)(struct Skybox *self);
} Skybox;

typedef struct Entity {
    long uniqueID;

    SceneObject *object;
} Entity;

extern Engine *engine;
extern FrameBufferObject *antiAlias;
extern Shader *defaultShader, *instanceShader, *antiAliasShader, *skyboxShader;
extern Camera *camera;

Engine *init(void);
int cleanup(void);

void update(void);
void render(void);

static void tick(void) {
    static double nextUpdateTime = 0.0;

    if (nextUpdateTime == 0.0) {
        nextUpdateTime = current_time();
    }

    double currentTime = current_time();

    if (currentTime >= nextUpdateTime) {
        update();

        nextUpdateTime += UPDATE_INTERVAL;

        if (currentTime > nextUpdateTime) {
            nextUpdateTime = currentTime;
        }

        engine->deltaTime = (float)(nextUpdateTime - currentTime);
    }
}

#endif // ENGINE_H