#pragma once

#ifndef RENDER_H
#define RENDER_H

#include "engine.h"
#include "object.h"

FrameBufferObject *BindFrameBuffer(FrameBufferObject frameBuffer);

SceneObject *NewSceneObject(SceneObject builder);
SceneObject *NewSprite(vec3s position, float size, bool billboard, const char *path);
SceneObject *CopySceneObject(SceneObject *object);

void ExpandBoundingBox(SceneObject *object, Model3D *model, vec3s offset);
void GenerateBoundingBox(SceneObject *object, Model3D *model);
void DrawBoundingBox(SceneObject *object, Model3D *ourModel);

void GenerateTransformGizmo(SceneObject *object, Model3D *model);
void DrawTransformGizmo(SceneObject *object, Model3D *model);

void DrawLine(Line line);
void DrawTriangle(Triangle triangle);

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

void SendToShader(SceneObject *object, Model3D *model);
void UpdateInstancedBufferObj(SceneObject *object, Model3D *model, mat4s *matrices, int newCount);

static inline float CalcDistance(vec3s cameraPos, vec3s objectPos) {
    return sqrtf(powf(cameraPos.x - objectPos.x, 2) +
                 powf(cameraPos.y - objectPos.y, 2) +
                 powf(cameraPos.z - objectPos.z, 2));
}

// Extract the frustum planes from the projection * view matrix
static inline void UpdateFrustum(Camera *camera, float projViewMatrix[16]) {
    // Left Plane
    camera->frustum[0].a = projViewMatrix[3] + projViewMatrix[0];
    camera->frustum[0].b = projViewMatrix[7] + projViewMatrix[4];
    camera->frustum[0].c = projViewMatrix[11] + projViewMatrix[8];
    camera->frustum[0].d = projViewMatrix[15] + projViewMatrix[12];

    // Right Plane
    camera->frustum[1].a = projViewMatrix[3] - projViewMatrix[0];
    camera->frustum[1].b = projViewMatrix[7] - projViewMatrix[4];
    camera->frustum[1].c = projViewMatrix[11] - projViewMatrix[8];
    camera->frustum[1].d = projViewMatrix[15] - projViewMatrix[12];

    // Bottom Plane
    camera->frustum[2].a = projViewMatrix[3] + projViewMatrix[1];
    camera->frustum[2].b = projViewMatrix[7] + projViewMatrix[5];
    camera->frustum[2].c = projViewMatrix[11] + projViewMatrix[9];
    camera->frustum[2].d = projViewMatrix[15] + projViewMatrix[13];

    // Top Plane
    camera->frustum[3].a = projViewMatrix[3] - projViewMatrix[1];
    camera->frustum[3].b = projViewMatrix[7] - projViewMatrix[5];
    camera->frustum[3].c = projViewMatrix[11] - projViewMatrix[9];
    camera->frustum[3].d = projViewMatrix[15] - projViewMatrix[13];

    // Near Plane
    camera->frustum[4].a = projViewMatrix[3] + projViewMatrix[2];
    camera->frustum[4].b = projViewMatrix[7] + projViewMatrix[6];
    camera->frustum[4].c = projViewMatrix[11] + projViewMatrix[10];
    camera->frustum[4].d = projViewMatrix[15] + projViewMatrix[14];

    // Far Plane
    camera->frustum[5].a = projViewMatrix[3] - projViewMatrix[2];
    camera->frustum[5].b = projViewMatrix[7] - projViewMatrix[6];
    camera->frustum[5].c = projViewMatrix[11] - projViewMatrix[10];
    camera->frustum[5].d = projViewMatrix[15] - projViewMatrix[14];

    // Normalize the planes
    for (int i = 0; i < 6; i++) {
        float length = sqrtf(camera->frustum[i].a * camera->frustum[i].a +
                             camera->frustum[i].b * camera->frustum[i].b +
                             camera->frustum[i].c * camera->frustum[i].c);
        camera->frustum[i].a /= length;
        camera->frustum[i].b /= length;
        camera->frustum[i].c /= length;
        camera->frustum[i].d /= length;
    }
}

static inline bool ObjectInFrustum(Camera *camera, vec3s objectPosition, float radius) {
    for (int i = 0; i < 6; i++) {
        if (camera->frustum[i].a * objectPosition.x + camera->frustum[i].b * objectPosition.y + camera->frustum[i].c * objectPosition.z + camera->frustum[i].d <= -radius)
            return GLFW_FALSE;  // Outside this plane
    }
    return GLFW_TRUE;  // Inside all planes
}

static inline bool ObjectExists(SceneObject *object) {
    return object && object->VAO != 0;
}

static inline void UnbindBufferObj(SceneObject *object, Model3D *model) {
    BoundingBox *boundingBox = (object != NULL) ? object->transforms->boundingBox : model->transforms->boundingBox;

    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Delete the buffers
    if (object != NULL) {
        glDeleteVertexArrays(1, &object->VAO);
        glDeleteBuffers(1, &object->VBO);
        glDeleteBuffers(1, &object->IVBO);
        glDeleteBuffers(1, &object->EBO);

        object->VAO = 0;
        object->VBO = 0;
        object->IVBO = 0;
        object->EBO = 0;
    }

    if (model != NULL) {
        if (!isListEmpty(model->meshes)) {
            foreach (Mesh *mesh, model->meshes) {
                glDeleteVertexArrays(1, &mesh->VAO);
                glDeleteBuffers(1, &mesh->VBO);
                glDeleteBuffers(1, &mesh->IVBO);
                glDeleteBuffers(1, &mesh->EBO);

                mesh->VAO = 0;
                mesh->VBO = 0;
                mesh->IVBO = 0;
                mesh->EBO = 0;
            }
        }
    }

    if (boundingBox != NULL) {
        glDeleteVertexArrays(1, &boundingBox->VAO);
        glDeleteBuffers(1, &boundingBox->VBO);
        glDeleteBuffers(1, &boundingBox->EBO);

        boundingBox->VAO = 0;
        boundingBox->VBO = 0;
        boundingBox->EBO = 0;
    }
}

static inline void RemoveTextures(void) {
    size_t size = MapSize(engine->textures);

    if (!isMapEmpty(engine->textures)) {
        MapClear(engine->textures);
    }

    printf("[TAV ENGINE] %zu Textures have been freed!\n", size);
}

static inline void FreeupObject(SceneObject *object) {
    if (ObjectExists(object)) {
        UnbindBufferObj(object, NULL);
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

#endif  // RENDER_H