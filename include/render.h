#pragma once

#ifndef RENDER_H
#define RENDER_H

#include "engine.h"
#include "object.h"

FrameBufferObject *BindFrameBuffer(FrameBufferObject frameBuffer);
void UnbindFrameBufferObj(FrameBufferObject *frameBuffer);
void SendToShader(SceneObject *object);

SceneObject *NewSceneObject(SceneObject object);
SceneObject *CopySceneObject(SceneObject *object);

Transform *NewTransforms(int instanceCount, Transform *transforms);

Texture *NewTexture(const char *path);

/*
Order of operation:
    +X (right)
    -X (left)
    +Y (top)
    -Y (bottom)
    +Z (front) 
    -Z (back)
*/
Skybox *NewSkybox(List *textureNames);

void UseTexture(Texture *texture);

void UpdateInstancedBufferObj(SceneObject *object, mat4s *matrices, int newCount);
void BindBufferObj(SceneObject *object);
void UnbindBufferObj(SceneObject *object);

static inline bool ObjectExists(SceneObject *object) {
    return object && object->VAO != 0;
}

static inline void FreeupObject(SceneObject *object) {
    if (object != NULL) {
        UnbindBufferObj(object);

        freeMeshData(object->meshData);

        if (object->sprite != NULL) {
            free(object->sprite);
        }

        if (object->texture != NULL) {
            free(object->texture);
        }

        if (object->transforms != NULL) {
            free(object->transforms);
        }

        free(object);
    }
}

void RemoveSceneObject(SceneObject *object);

static inline char *getAssetPath(const char *path) {
    return (char *)CreatePath(engine->assetDir, path);
}

#endif // RENDER_H