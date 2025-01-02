#include "object.h"

#include "render.h"
#include "shader.h"
#include "utils.h"

void freeMeshData(MeshData *meshData) {
    if (meshData) {
        free(meshData->verticesCopy);
        free(meshData->indicesCopy);

        if (meshData->rawVerticesCopy != NULL) {
            free(meshData->rawVerticesCopy);
        }

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
        .transforms = NewTransforms(1, (Transform[]){{.position = position}}),
        .color = (vec3s){0.0f, 1.0f, 1.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});
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

    return (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_3D,
        .tag = "PLANE",
        .transforms = NewTransforms(1, (Transform[]){
                                           {.position = position,
                                            .rotationDegrees = -90.0f,
                                            .rotation = (vec3s){1.0f, 0.0f, 0.0f},
                                            .scale = (vec3s){100.0f, 100.0f, 100.0f}}}),
        .color = (vec3s){1.0f, 1.0f, 1.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});
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
    Texture *texture = (Texture *)NewTexture(TEXTURE_TYPE_2D, "grass_block.png");

    SceneObject *object = (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_3D,
        .tag = "CUBE",
        .color = (vec3s){0.0f, 0.0f, 0.0f},
        .texture = texture,
        .transforms = NewTransforms(1, (Transform[]){{.position = position}}),
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});
    return object;
}

BoundingBox *CreateBoundingBox(BoundingBox builder) {
    BoundingBox *box = (BoundingBox *)malloc(sizeof(BoundingBox));
    if (box == NULL) {
        printf("[MEMORY ERROR] Failed to allocate memory for BoundingBox\n");
        return NULL;
    }

    if (builder.color.x == 0 && builder.color.y == 0 && builder.color.z == 0) {
        builder.color = (vec3s){0.0f, 0.0f, 0.37f};
    }

    memcpy(box, &builder, sizeof(BoundingBox));

    GLfloat vertices[] = {
        box->min.x, box->min.y, box->min.z,  // Bottom-left-front
        box->max.x, box->min.y, box->min.z,  // Bottom-right-front
        box->max.x, box->max.y, box->min.z,  // Top-right-front
        box->min.x, box->max.y, box->min.z,  // Top-left-front
        box->min.x, box->min.y, box->max.z,  // Bottom-left-back
        box->max.x, box->min.y, box->max.z,  // Bottom-right-back
        box->max.x, box->max.y, box->max.z,  // Top-right-back
        box->min.x, box->max.y, box->max.z   // Top-left-back
    };

    // Define the edges of the bounding box as line segments
    GLuint indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,  // Front face
        4, 5, 5, 6, 6, 7, 7, 4,  // Back face
        0, 4, 1, 5, 2, 6, 3, 7   // Connecting edges
    };

    int vertexCount = getArraySize(vertices);
    int indexCount = getArraySize(indices);

    // MeshData *meshData = (MeshData *)GetMeshCopiesRaw(vertices, vertexCount, indices, indexCount);

    // Create and bind VAO
    glGenVertexArrays(1, &box->VAO);
    glGenBuffers(1, &box->VBO);
    glGenBuffers(1, &box->EBO);

    glBindVertexArray(box->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, box->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexCount, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, box->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // box->meshData = meshData;

    printf("[DEBUG] Created bounding box\n");
    return box;
}