#pragma once

#ifndef MODEL3D_H
#define MODEL3D_H

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "engine.h"
#include "render.h"
#include "shader.h"
#include "utils.h"
#include "ui.h"

Model3D *NewModel3D(Model3D builder, const char *path);

static inline bool ModelExists(Model3D *model) {
    return model && ListSize(model->meshes) > 0;
}

static inline void RemoveModels(void) {
    int counter = 0;
    foreach (Model3D *model, engine->models) {
        if (!ModelExists(model)) continue;

        UnbindBufferObj(NULL, model);
        counter++;
    }

    ListClear(engine->models);

    printf("[TAV ENGINE] %d 3D Models have been freed!\n", counter);
}

static inline void SetupMesh(Model3D *model, Mesh *mesh) {
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);
    glGenBuffers(1, &mesh->IVBO);

    int vertexCount = mesh->vertexCount;
    int indexCount = mesh->indexCount;

    // printf("SetupMesh -> Vertices size %zu\n", vertexCount);

    // printf("Vertices Unit Test Begin\n");

    // for (int i = 0; i < vertexCount; i++) {
    //     Vertex v = mesh->vertices[i];
    //     printf("Vertex %i: (%f, %f, %f)\n", i, v.position.x, v.position.y, v.position.z);
    // }

    // printf("Vertices Unit Test End\n");

    glBindVertexArray(mesh->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertexCount, mesh->vertices, GL_STATIC_DRAW);

    // printf("SetupMesh -> Indices size %zu\n", indexCount);

    // printf("Indices Unit Test Begin\n");

    // for (int i = 0; i < indexCount; i++) {
    //     GLuint index = mesh->indices[i];
    //     printf("Index %i: (%u)\n", i, index);
    // }

    // printf("Indices Unit Test End\n");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * indexCount, mesh->indices, GL_STATIC_DRAW);

    // printf("Size of Vertex: %zu\n", sizeof(Vertex));

    GLint vboSize, eboSize;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &vboSize);
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &eboSize);
    // printf("VBO Size: %d, Expected: %zu\n", vboSize, vertexCount * sizeof(Vertex));
    // printf("EBO Size: %d, Expected: %zu\n", eboSize, indexCount * sizeof(GLuint));

    // printf("SetupMesh -> Binding attributes now\n");

    // Position attribute (layout = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (layout = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(1);

    // vertex normals
    vec3s normal = mesh->vertices[0].normal;
    if (normal.x != 0 && normal.y != 0 && normal.z != 0) {
        // Normal attribute (layout = 2)
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));
    }

    // // vertex tangent
    // glEnableVertexAttribArray(3);
    // glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    // // vertex bitangent
    // glEnableVertexAttribArray(4);
    // glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    // // ids
    // glEnableVertexAttribArray(5);
    // glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));

    // // weights
    // glEnableVertexAttribArray(6);
    // glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, boneWeights));

    int instanceCount = model->instanceCount;
    // Instance matrix attribute (layout = 3)
    if (instanceCount > 1) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh->IVBO);

        // mat4s matrix_model = model->transforms->model;
        // printf("Our translation units x=%.2f y=%.2f z=%.2f\n", matrix_model.raw[3][0], matrix_model.raw[3][1], matrix_model.raw[3][2]);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4s), &model->transforms->model.raw[0], GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(mat4s), (void *)0);
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(mat4s), (void *)(sizeof(vec4s)));
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(mat4s), (void *)(2 * sizeof(vec4s)));
        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(mat4s), (void *)(3 * sizeof(vec4s)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // printf("SetupMesh -> Internal Set up mesh\n");
}

static inline void DrawMesh(Model3D *model, Mesh *mesh) {
    // printf("[!IMPORTANT] DRAWMESH -> TOP of Draw Mesh\n");

    UseShader(*model->shader);
    SendToShader(NULL, model);
    // printf("Sent to shader\n");
    // bind appropriate textures
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint normalNr = 1;
    GLuint heightNr = 1;

    int counter = 0;

    // if (!isListEmpty(mesh->textures)) {
    //     foreach (Texture *texture, mesh->textures) {
    //         if (texture != NULL) {
    //             counter++;
    //             // active proper texture unit before binding
    //             glActiveTexture(GL_TEXTURE0 + counter);

    //             // retrieve texture number (the N in texture_diffuseN)
    //             char number[16];
    //             char combined[128];
    //             char *name = textureTypetoString(texture->type);

    //             if (strcmp(name, "texture_diffuse") == 0)
    //                 sprintf(number, "%u", diffuseNr++);
    //             else if (strcmp(name, "texture_specular") == 0)
    //                 sprintf(number, "%u", specularNr++);
    //             else if (strcmp(name, "texture_normal") == 0)
    //                 sprintf(number, "%u", normalNr++);
    //             else if (strcmp(name, "texture_height") == 0)
    //                 sprintf(number, "%u", heightNr++);

    //             sprintf(combined, "%s%s", name, number);

    //             // now set the sampler to the correct texture unit
    //             setInt(*model->shader, combined, counter);

    //             // and finally bind the texture
    //             glBindTexture(GL_TEXTURE_2D, texture->textureID);

    //             printf("DrawMesh -> Looped texture\n");
    //         } else {
    //             printf("DrawMesh -> Something is wrong with mesh->textures\n");
    //         }
    //     }
    // }

    // printf("DrawMesh-> MIDDLE\n");

    // printf("Before bind -> VAO: %d, indexCount: %d\n", mesh->VAO, mesh->indexCount);
    glBindVertexArray(mesh->VAO);
    // printf("After bind -> VAO: %d, indexCount: %d\n", mesh->VAO, mesh->indexCount);
    UseTexture(model->texture);

    int indexCount = mesh->indexCount;
    int vertexCount = mesh->vertexCount;
    int instanceCount = model->instanceCount;
    // printf("DrawMesh -> Index count: %d\n", indexCount);

    if (mesh->indices != NULL && indexCount > 0) {
        if (instanceCount > 1) {
            glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
        } else {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }
    } else if (mesh->vertices != NULL && vertexCount > 0) {
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }

    // reset to default texture unit
    // if (!isListEmpty(mesh->textures)) {
    //     glActiveTexture(GL_TEXTURE0);
    // }

    glBindVertexArray(0);
}

static inline Mesh *NewMesh(Mesh builder, Model3D *model) {
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    if (mesh == NULL) {
        fprintf(stderr, "[MEMORY ERROR] Failed creating NewMesh();, ERROR ALLOCATING MEMORY\n");
        free(mesh);
        return NULL;
    }

    memcpy(mesh, &builder, sizeof(Mesh));

    mesh->draw = DrawMesh;

    // printf("[Mesh] VertexCount = %d | IndexCount = %d\n", mesh->vertexCount, mesh->indexCount);

    SetupMesh(model, mesh);
    // printf("NEWMESH -> Set up mesh\n");
    return mesh;
}

static inline void DrawModel(Model3D *model) {
    // printf("[!IMPORTANT] DRAWMODEL -> Top of Draw Model\n");

    foreach (Mesh *mesh, model->meshes) {
        if (mesh == NULL) continue;

        mesh->draw(model, mesh);
        DrawBoundingBox(NULL, model);
        // printf("DrawModel -> Drawing mesh\n");
    }
}

/* Checks all material textures of a given type and loads the textures if they're not loaded yet. */
static inline List *loadMaterialTextures(Model3D *model, C_STRUCT aiMaterial *mat, C_ENUM aiTextureType type, char *typeName) {
    List *textures = (List *)NewList(NULL);

    for (GLuint i = 0; i < aiGetMaterialTextureCount(mat, type); i++) {
        C_STRUCT aiString str;
        aiGetMaterialTexture(mat, type, i, &str, NULL, NULL, NULL, NULL, NULL, NULL);

        /* Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture */
        bool skip = false;

        for (GLuint j = 0; j < ListSize(model->texturesLoaded); j++) {
            Texture *tex = (Texture *)ListGet(model->texturesLoaded, j);

            if (tex != NULL) {
                // printf("Found tex in loadMaterialTexs()\n");
                if (strcmp(tex->path, str.data) == 0) {
                    ListAdd(textures, tex);
                    skip = true; /* A 'Texture' with the same filepath has already been loaded, continue to next one. (optimization) */

                    // printf("LOADMATERIALTEXTURES -> ADD TO FUNCTION LIST CHECK\n");
                    break;
                }
            }
        }

        TextureType ourTextureType = TEXTURE_TYPE_NONE;

        /* If texture hasn't been loaded already, load it */
        if (!skip) {
            if (strcmp(typeName, "texture_diffuse") == 0) {
                ourTextureType = TEXTURE_TYPE_DIFFUSE;
            } else if (strcmp(typeName, "texture_specular") == 0) {
                ourTextureType = TEXTURE_TYPE_SPECULAR;
            } else if (strcmp(typeName, "texture_normal") == 0) {
                ourTextureType = TEXTURE_TYPE_NORMAL;
            } else if (strcmp(typeName, "texture_height") == 0) {
                ourTextureType = TEXTURE_TYPE_HEIGHT;
            }
        }

        Texture *texture = (Texture *)NewTexture(ourTextureType, str.data);

        if (texture != NULL) {
            ListAdd(textures, texture);
            ListAdd(model->texturesLoaded, texture);
        }

        // printf("LOADMATERIALTEXTURES -> Loading new texture for Model3D %s w/ type: (%s)\n", str.data, typeName);
    }

    // printf("Returning material textures list with size %zu\n", ListSize(textures));

    return textures;
}

static inline Mesh *ProcessOurMesh(Model3D *model, C_STRUCT aiMesh *mesh, const C_STRUCT aiScene *scene) {
    Mesh ourMesh;
    ourMesh.vertexCount = mesh->mNumVertices;

    /* Data to fill */
    List *textures = (List *)NewList(NULL); /* <Texture *> */
    ourMesh.vertices = (Vertex *)malloc(ourMesh.vertexCount * sizeof(Vertex));

    /* Loop through each of the mesh's vertices */
    for (GLuint i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vec3s vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        // printf("OBJ Vertex Parsed: %f, %f, %f\n", vector.x, vector.y, vector.z);

        vertex.position = vector;

        // printf("PROCESSOURMESH -> VERTEXES CHECK\n");

        /* Normals */
        if (mesh->mNormals != NULL) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;

            vertex.normal = vector;
            // printf("PROCESSOURMESH -> NORMALS CHECK\n");
        }

        /* Tex Coords */
        if (mesh->mTextureCoords[0]) {
            vec2s vec;

            /* Vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                use models where a Vertex can have multiple texture coordinates so we always take the first set (0).
            */
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = vec;

            // /* Tangent */
            // vector.x = mesh->mTangents[i].x;
            // vector.y = mesh->mTangents[i].y;
            // vector.z = mesh->mTangents[i].z;
            // vertex.tangent = vector;

            // /* Bitangent */
            // vector.x = mesh->mBitangents[i].x;
            // vector.y = mesh->mBitangents[i].y;
            // vector.z = mesh->mBitangents[i].z;
            // vertex.bitangent = vector;

            // printf("PROCESSOURMESH -> TEXCOORDS | TANGENT | BITANGENT CHECK\n");
        } else {
            vertex.texCoords = (vec2s){0.0f, 0.0f};
            // printf("PROCESSOURMESH -> SORRY TEXCOORDS DEFAULTED\n");
        }

        // printf("Adding Vertex to mesh vertices array %i: (%f, %f, %f)\n", i, vertex.position.x, vertex.position.y, vertex.position.z);
        if (i < ourMesh.vertexCount) {
            ourMesh.vertices[i] = vertex;
        } else {
            printf("[INDEX ARRAY OUT OF BOUNDS ERROR]  Vertex counter out of bounds when Processing our Mesh: %d\n", i);
        }

        // printf("PROCESSOURMESH -> ONE FULL LOOP\n");
    }

    /* Now walk through each of the Mesh's faces and retrieve the corresponding vertex indices. */
    GLuint indexCounter = 0;
    GLuint totalIndices = 0;
    for (GLuint i = 0; i < mesh->mNumFaces; i++) {
        totalIndices += mesh->mFaces[i].mNumIndices;
    }

    ourMesh.indexCount = totalIndices;

    if (ourMesh.indices == NULL) {
        ourMesh.indices = (GLuint *)malloc(ourMesh.indexCount * sizeof(GLuint));
    }

    for (GLuint i = 0; i < mesh->mNumFaces; i++) {
        C_STRUCT aiFace face = (C_STRUCT aiFace)mesh->mFaces[i];

        /* Retrieve all indices of the face and store them in the indices list */
        for (GLuint j = 0; j < face.mNumIndices; j++) {
            if (indexCounter < ourMesh.indexCount) {
                ourMesh.indices[indexCounter++] = face.mIndices[j];
            } else {
                printf("[INDEX ARRAY OUT OF BOUNDS ERROR] Index counter out of bounds when Processing our Mesh: %d\n", indexCounter);
            }
            // printf("PROCESSOURMESH -> INDICES LOOP & CHECK\n");
        }
    }

    /* Process materials */
    C_STRUCT aiMaterial *material = (C_STRUCT aiMaterial *)scene->mMaterials[mesh->mMaterialIndex];

    /*
        Each diffuse texture should be named as 'texture_diffuseN',
        where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER.

         Same applies to other texture as the following list summarizes:
         diffuse: texture_diffuseN
         specular: texture_specularN
         normal: texture_normalN
    */

    /* 1. diffuse maps */
    List *diffuseMaps = (List *)loadMaterialTextures(model, material, aiTextureType_DIFFUSE, "texture_diffuse");

    if (!isListEmpty(diffuseMaps)) {
        ListAddAll(textures, diffuseMaps);
    }

    /* 2. specular maps */
    List *specularMaps = (List *)loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");

    if (!isListEmpty(specularMaps)) {
        ListAddAll(textures, specularMaps);
    }

    /* 3. normal maps */
    List *normalMaps = (List *)loadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");

    if (!isListEmpty(normalMaps)) {
        ListAddAll(textures, normalMaps);
    }

    /* 4. height maps */
    List *heightMaps = (List *)loadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");

    if (!isListEmpty(heightMaps)) {
        ListAddAll(textures, heightMaps);
    }

    // printf("PROCESSOURMESH -> ENTIRE FUNCTION\n");
    return (Mesh *)NewMesh(ourMesh, model);
}

/*
    Processes a node in a recursive fashion.
    Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
*/
static inline void ProcessRootNode(Model3D *model, C_STRUCT aiNode *node, const C_STRUCT aiScene *scene) {
    if (scene != NULL) {
        /* Process each 'Mesh' located at the current node */
        for (GLuint i = 0; i < node->mNumMeshes; i++) {
            /* The node object only contains indices to index the actual objects in the scene.
               the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
            */
            C_STRUCT aiMesh *mesh = (C_STRUCT aiMesh *)scene->mMeshes[node->mMeshes[i]];
            ListAdd(model->meshes, (Mesh *)ProcessOurMesh(model, mesh, scene));
            // printf("PROCESSROOTNODE -> MESHES LOOP & CHECK\n");
        }

        /* After we've processed all of the Meshes (if any) we then recursively process each of the children nodes */
        for (GLuint i = 0; i < node->mNumChildren; i++) {
            ProcessRootNode(model, node->mChildren[i], scene);
            // printf("PROCESSROOTNODE -> CHILDREN LOOP & CHECK\n");
        }
    }
}

#endif  // MODEL3D_H