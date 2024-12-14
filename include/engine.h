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

typedef enum TextureType {
    TEXTURE_TYPE_2D,
    TEXTURE_TYPE_CUBEMAP,
    TEXTURE_TYPE_DIFFUSE,
    TEXTURE_TYPE_SPECULAR,
    TEXTURE_TYPE_NORMAL
} TextureType;

typedef struct Texture {
    TextureType type;
    GLuint textureID;

    int width, height, nrChannels;
} Texture;

typedef struct Skybox {
    List *textureNames;
    Texture *texture;
    GLuint VAO, VBO;

    /* DO NOT call this function; This is called in the main render loop; */
    void (*draw)(struct Skybox *self);

    /* Do NOT call this function; This is called @ cleanup(); */
    void (*free)(struct Skybox *self);
} Skybox;

typedef struct Engine {
    GLFWwindow *window;
    float windowWidth, windowHeight, aspectRatio, fps, deltaTime, lastX, lastY;

    NVGcontext *vgContext;
    int defaultFont;

    char *mainPath, *settingsFile, *shaderDir, *assetDir, *fontDir;
    List *sceneObjects, *shaders;

    /*
     -> Skybox struct for handling the Skybox Cubemap
     -> Usage: You can keep track of the List of texture file path's that were used, textureID, VAO & VBO.
     -> DO NOT USE THE DRAW OR FREE FUNCTION, THEIR FUNCTION POINTERS ARE ONLY FOR DEVELOPER USE ONLY.
     -> Example:
       **
        List *textureNames = (List *)NewList(NULL);
        
        ListAddMultiple(textureNames, 
        "right.png",
        "left.png",
        "top.png",
        "bottom.png",
        "front.png",
        "back.png");

        Skybox *skybox = (Skybox *)NewSkybox(textureNames);
       **
    */
    Skybox *skybox;

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

    /* Do NOT call this function; */
    void (*update)(struct Camera *self);
} Camera;

typedef struct Shader {
    GLuint programID;

    const char *vertexPath;
    const char *fragmentPath;
    const char *vShaderCode;
    const char *fShaderCode;
} Shader;

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

    /* Do NOT call this function; */
    void (*free)(struct MeshData *self);
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

    /* Do NOT call this function; */
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

    /* Do NOT call this function; */
    void (*drawBuffer)(struct FrameBufferObject *self);
} FrameBufferObject;

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