#include "object.h"

#include "render.h"
#include "shader.h"
#include "utils.h"

void freeMeshData(MeshData *meshData) {
    if (meshData) {
        free(meshData->verticesCopy);
        free(meshData->indicesCopy);
        free(meshData);
    }
}

SceneObject *CreateTriangle(vec3s position) {
    Vertex vertices[] = {
        // Position         // Normal            // TexCoords
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // Top-left
        {{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // Top-right
        {{0.0f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.5f, 0.0f}}   // Bottom-left
    };

    GLuint indices[] = {
        0, 1, 2  // Triangle
    };

    int vertexCount = getArraySize(vertices);
    int indexCount = getArraySize(indices);

    MeshData *meshData = (MeshData *)GetMeshCopies(vertices, vertexCount, indices, indexCount);

    SceneObject *object = (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_2D,
        .tag = "TRIANGLE",
        .color = (vec3s){0.0f, 1.0f, 1.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});

    Transform *transforms = (Transform *)object->transforms;
    transforms[0] = (Transform){
        .position = position};
    object->transforms = transforms;
    return object;
}

SceneObject *CreatePlane(vec3s position) {
    Vertex vertices[] = {
        // Position         // Normal            // TexCoords
        {{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // Top-left
        {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},    // Top-right
        {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // Bottom-left
        {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}    // Bottom-right
    };

    GLuint indices[] = {
        0, 1, 2,  // First triangle
        1, 3, 2   // Second triangle
    };

    int vertexCount = getArraySize(vertices);
    int indexCount = getArraySize(indices);

    MeshData *meshData = (MeshData *)GetMeshCopies(vertices, vertexCount, indices, indexCount);

    SceneObject *object = (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_3D,
        .tag = "PLANE",
        .color = (vec3s){1.0f, 1.0f, 1.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});

    Transform *transforms = (Transform *)object->transforms;
    transforms[0] = (Transform){
        .position = position,
        .rotationDegrees = -90.0f,
        .rotation = (vec3s){1.0f, 0.0f, 0.0f},
        .scale = (vec3s){100.0f, 100.0f, 100.0f}};

    object->transforms = transforms;
    return object;
}
SceneObject *CreateCube(vec3s position) {
    Vertex vertices[] = {
        // Front face
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},   // Top-left
        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},    // Top-right
        {{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},  // Bottom-left
        {{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},   // Bottom-right

        // Back face
        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},    // Top-right
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},   // Top-left
        {{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},   // Bottom-right
        {{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},  // Bottom-left

        // Left face
        {{-0.5f, 0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // Top-left
        {{-0.5f, 0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},    // Top-right
        {{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-left
        {{-0.5f, -0.5f, 0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // Bottom-right

        // Right face
        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},    // Top-left
        {{0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},   // Top-right
        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},   // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},  // Bottom-right

        // Top face
        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},  // Top-left
        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},   // Top-right
        {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},   // Bottom-left
        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},    // Bottom-right

        // Bottom face
        {{-0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},   // Top-left
        {{0.5f, -0.5f, 0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},    // Top-right
        {{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},  // Bottom-left
        {{0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},   // Bottom-right
    };

    GLuint indices[] = {
        // Front face
        0, 1, 2, 1, 3, 2,
        // Back face
        4, 5, 6, 5, 7, 6,
        // Left face
        8, 9, 10, 9, 11, 10,
        // Right face
        12, 13, 14, 13, 15, 14,
        // Top face
        16, 17, 18, 17, 19, 18,
        // Bottom face
        20, 21, 22, 21, 23, 22};

    int vertexCount = getArraySize(vertices);
    int indexCount = getArraySize(indices);

    MeshData *meshData = (MeshData *)GetMeshCopies(vertices, vertexCount, indices, indexCount);

    SceneObject *object = (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_3D,
        .tag = "CUBE",
        .instanceCount = 2500,
        .color = (vec3s){1.0f, 0.0f, 0.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});

    Transform *transforms = (Transform *)object->transforms;
    transforms[0] = (Transform){
        .position = position};

    for (int i = 0; i < object->instanceCount; i++) {
        transforms[i] = (Transform){
            .position = (vec3s){
                .x = ((float)rand() / RAND_MAX) * 10 * position.x - 4.0f,
                .y = ((float)rand() / RAND_MAX) * 10 * position.y - 4.0f,
                .z = ((float)rand() / RAND_MAX) * 10 * position.z - 4.0f}};
    }

    object->transforms = transforms;
    return object;
}