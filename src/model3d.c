#include "model3d.h"

#include <pthread.h>

void *LoadAsync(void *arg) {
    const char *path = (const char *)arg;

    C_STRUCT aiScene *scene = (C_STRUCT aiScene *)aiImportFile(getAssetPath(path),
                                                               aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        printf("[Model3D] ERROR from 'ASSIMP' library -> %s\n", (char *)aiGetErrorString());
    }

    return scene;
}

Model3D *NewModel3D(Model3D builder, const char *path) {
    Model3D *model = (Model3D *)malloc(sizeof(Model3D));
    if (model == NULL) {
        printf("[MEMORY ERROR] Failed to allocate memory for Model3D.");
        return NULL;
    }

    memcpy(model, &builder, sizeof(Model3D));

    /* TODO COMPLETE (ASYNC LOAD)*/
    pthread_t thread;
    if (pthread_create(&thread, NULL, LoadAsync, (void *)path) != 0) {
        printf("[THREAD ERROR] Failed to create thread #NewModel3D.\n");
        return NULL;
    } else {
        printf("[THREAD] 'asset_thread' Creation Success\n");
    }

    C_STRUCT aiScene *scene = NULL;
    if (pthread_join(thread, (void **)&scene) != 0) {
        printf("[THREAD ERROR] Failed to join thread #NewModel3D.\n");
        return NULL;
    } else {
        printf("[THREAD] 'asset_thread' Join Success\n");
    }

    /* Check if the scene was loaded successfully */
    if (!scene) {
        printf("[Model3D] ERROR: Failed to load the model 'scene' from thread: %s\n", path);
        return NULL;
    }

    if (!model->clickable.onClick && model->clickable.hoverColor.x == 0 && model->clickable.hoverColor.y == 0 && model->clickable.hoverColor.z == 0) {
        model->hoverColor = getOppositeColorV3S(model->color);

        model->clickable = (Clickable){
            .onClick = NULL,
            .hoverColor = model->hoverColor};
    }

    if (model->meshes == NULL) {
        model->meshes = (List *)NewList(NULL);
    }

    if (model->texturesLoaded == NULL) {
        model->texturesLoaded = (List *)NewList(NULL);
    }

    if (model->instanceCount == 0) {
        model->instanceCount = 1;
    }

    if (model->transforms == NULL) {
        model->transforms = (Transform *)malloc(model->instanceCount * sizeof(Transform));
        model->transforms[0].model = glms_mat4_identity();
    }

    if (model->draw == NULL) {
        model->draw = DrawModel;
    }

    if (model->shader == NULL) {
        model->shader = defaultShader;
    }

    if (model->color.x == 0 && model->color.y == 0 && model->color.z == 0) {
        model->color = (vec3s){1.0f, 1.0f, 1.0f};
    }

    // process ASSIMP's root node recursively
    ProcessRootNode(model, scene->mRootNode, scene);
    printf("[Model3D] '%s' loaded.\n", path);

    ListAdd(engine->models, model);
    return model;
}