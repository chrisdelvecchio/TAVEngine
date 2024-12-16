#pragma once

#ifndef RENDER_H
#define RENDER_H

#include "engine.h"
#include "object.h"

FrameBufferObject *BindFrameBuffer(FrameBufferObject frameBuffer);

SceneObject *NewSceneObject(SceneObject builder);
SceneObject *NewSprite(vec3s position, float size, bool billboard, const char *path);

SceneObject *CopySceneObject(SceneObject *object);

Transform *NewTransforms(int instanceCount, Transform *transforms);

Texture *NewTexture(TextureType type, const char *path);

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

static inline bool ObjectExists(SceneObject *object) {
    return object && object->VAO != 0;
}

static inline void UnbindBufferObj(SceneObject *object) {
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Delete the buffers
    glDeleteVertexArrays(1, &object->VAO);
    glDeleteBuffers(1, &object->VBO);
    glDeleteBuffers(1, &object->IVBO);
    glDeleteBuffers(1, &object->EBO);

    object->VAO = 0;
    object->VBO = 0;
    object->IVBO = 0;
    object->EBO = 0;
}

static inline void RemoveTextures(void) {
    int counter = 0;

    List *temp = (List *)NewList(NULL);
    ListAddArray(temp, engine->textures->values->entries);

    foreach (Texture *texture, temp) {
        if (texture == NULL) continue;
        counter++;
    }

    if (!isListEmpty(temp)) {
        ListClear(temp);
        ListFreeMemory(temp);
    }

    if (!isMapEmpty(engine->textures)) {
        MapClear(engine->textures);
    }

    printf("[TAV ENGINE] %d Textures have been freed!\n", counter);
}

static inline void FreeupObject(SceneObject *object) {
    if (ObjectExists(object)) {
        UnbindBufferObj(object);

        // freeMeshData(object->meshData);

        // if (object->transforms != NULL) {
        //     free(object->transforms);
        // }

        // free(object);
    }
}

static inline void UnbindFrameBufferObj(FrameBufferObject *frameBuffer) {
    if (frameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &frameBuffer->frameBufferID);
        glDeleteTextures(1, &frameBuffer->texColorBufferID);
        glDeleteRenderbuffers(1, &frameBuffer->depthStencilBufferID);

        free(frameBuffer);

        printf("[TAV ENGINE] Screen Framebuffer Object has been freed!\n");
    }
}

static inline void RemoveSceneObjects(void) {
    int counter = 0;
    foreach (SceneObject *object, engine->sceneObjects) {
        if (!ObjectExists(object)) continue;
        FreeupObject(object);
        counter++;
    }

    if (!isListEmpty(engine->sceneObjects)) {
        ListClear(engine->sceneObjects);
    }

    UnbindFrameBufferObj(antiAlias);

    printf("[TAV ENGINE] %d Scene Objects have been freed!\n", counter);
}

void RemoveSceneObject(SceneObject *object);

static inline char *getAssetPath(const char *path) {
    return (char *)CreatePath(engine->assetDir, path);
}

#endif // RENDER_H