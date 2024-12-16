#include "model3d.h"

Mesh *NewMesh(List *vertices, List *indices, List *textures) {
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    if (mesh == NULL) {
        fprintf(stderr, "[MEMORY ERROR] Failed creating NewMesh();, ERROR ALLOCATING MEMORY\n");
        free(mesh);
        return NULL;
    }

    mesh->vertices = vertices;
    mesh->indices = indices;
    mesh->textures = textures;
    mesh->draw = DrawMesh;
    
    SetupMesh(mesh);
    return mesh;
}