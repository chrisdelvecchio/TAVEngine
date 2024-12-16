#include "model3d.h"
#include "render.h"

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

Model3D *NewModel3D(const char *path, bool gammaCorrection) {
    Model3D *model = (Model3D *)malloc(sizeof(Model3D));
    const C_STRUCT aiScene *scene = (aiScene *)aiImportFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        printf("[ASSIMP ERROR] %s\n", aiGetErrorString());
        return NULL;
    }

    char *dir = malloc(strlen(path) * sizeof(char));
    dir = getAssetPath(path);

    model->path = path;
    model->gammaCorrection = gammaCorrection;
    model->meshes = (List *)NewList(NULL);
    model->texturesLoaded = (List *)NewList(NULL);
    model->draw = DrawModel;

    // process ASSIMP's root node recursively
    ProcessRootNode(model, scene->mRootNode, scene);
    return model;
}