#pragma once

#ifndef OBJECT_H
#define OBJECT_H

#include "engine.h"

static float cubeVertices[] = {
    // positions
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f, 1.0f,
    1.0f, -1.0f, 1.0f};

void freeMeshData(MeshData *meshData);

static inline MeshData *GetMeshCopies(Vertex *vertices, int vertexCount, GLuint *indices, int indexCount) {
    MeshData *meshData = malloc(sizeof(MeshData));
    if (meshData == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed.\n");
        return NULL;
    }

    meshData->free = freeMeshData;

    meshData->verticesCopy = (Vertex *)malloc(sizeof(Vertex) * vertexCount);
    if (meshData->verticesCopy == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed for vertices copy.\n");
        free(meshData);
        return NULL;
    }

    memcpy(meshData->verticesCopy, vertices, sizeof(Vertex) * vertexCount);

    meshData->indicesCopy = (GLuint *)malloc(sizeof(GLuint) * indexCount);
    if (meshData->indicesCopy == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed for indices copy.\n");
        free(meshData->verticesCopy);
        free(meshData);
        return NULL;
    }

    memcpy(meshData->indicesCopy, indices, sizeof(GLuint) * indexCount);
    return meshData;
}

static inline MeshData *GetMeshCopiesRaw(float *vertices, int vertexCount, GLuint *indices, int indexCount) {
    MeshData *meshData = malloc(sizeof(MeshData));
    if (meshData == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed.\n");
        return NULL;
    }

    meshData->free = freeMeshData;

    meshData->rawVerticesCopy = (float *)malloc(sizeof(float) * vertexCount);
    if (meshData->rawVerticesCopy == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed for RAW vertices copy.\n");
        free(meshData);
        return NULL;
    }

    memcpy(meshData->rawVerticesCopy, vertices, sizeof(float) * vertexCount);

    meshData->indicesCopy = malloc(sizeof(GLuint) * indexCount);
    if (meshData->indicesCopy == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #RETURNMESHCOPIES Memory allocation failed for indices copy.\n");
        free(meshData->verticesCopy);
        free(meshData);
        return NULL;
    }

    memcpy(meshData->indicesCopy, indices, sizeof(GLuint) * indexCount);
    return meshData;
}

static inline char *objectToString(ObjectType type) {
    switch (type) {
        case OBJECT_NONE:
            return "OBJECT_NONE";
        case OBJECT_LIGHT:
            return "OBJECT_LIGHT";
        case OBJECT_3D:
            return "OBJECT_3D";
        case OBJECT_2D:
            return "OBJECT_2D";
        default:
            return "UNKNOWN";
    }
}

static inline void debugObject(SceneObject *object) {
    printf("--------------------------------------\n");
    printf("OBJECT DEBUG => %s (%s)\n", object->tag, objectToString(object->type));
    printf("Position: (%f, %f, %f)\n", object->transforms->position.x, object->transforms->position.y, object->transforms->position.z);
    printf("Rotation: (%f, %f, %f)\n", object->transforms->rotation.x, object->transforms->rotation.y, object->transforms->rotation.z);
    printf("Rotation degrees: %.2f\n", object->transforms->rotationDegrees);
    printf("Scale: (%f, %f, %f)\n", object->transforms->scale.x, object->transforms->scale.y, object->transforms->scale.z);
    printf("Color: (%f, %f, %f)\n", object->color.r, object->color.g, object->color.b);
    printf("Vertices: %d\n", object->vertexCount);
    printf("Indices: %d\n", object->indexCount);
    printf("Has texture?: %s\n", (object->texture != NULL) ? "Yes" : "No");
    printf("--------------------------------------\n");
}

SceneObject *CreateTriangle(vec3s position);
SceneObject *CreatePlane(vec3s position);
SceneObject *CreateCube(vec3s position);

#endif  // OBJECT_H