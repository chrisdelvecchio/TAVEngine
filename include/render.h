#pragma once

#ifndef RENDER_H
#define RENDER_H

#include "game.h"

FrameBufferObject *BindFrameBuffer(FrameBufferObject frameBuffer);
void UnbindFrameBufferObj(FrameBufferObject *frameBuffer);


void SendToShader(SceneObject *object);

SceneObject *NewSceneObject(SceneObject object);
SceneObject *CopySceneObject(SceneObject *object);

Texture *NewTexture(const char *path);
void UseTexture(Texture *texture);

void UpdateInstancedBufferObj(SceneObject *object, mat4s *matrices, int newCount);
void BindBufferObj(SceneObject *object);
void UnbindBufferObj(SceneObject *object);

void RemoveSceneObject(SceneObject *object);

static char *getAssetPath(const char *path) {
    return (char *)CreatePath(game->assetDir, path);
}

#endif // RENDER_H