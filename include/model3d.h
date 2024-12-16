#pragma once

#ifndef MODEL3D_H
#define MODEL3D_H

#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "engine.h"
#include "shader.h"
#include "utils.h"

Mesh *NewMesh(List *vertices, List *indices, List *textures);
Model3D *NewModel3D(const char *path, bool gammaCorrection);

static inline void SetupMesh(Mesh *mesh) {
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

    glBufferData(GL_ARRAY_BUFFER, ListSize(mesh->vertices) * sizeof(Vertex), &mesh->vertices->data[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ListSize(mesh->indices) * sizeof(unsigned int),
                 &mesh->indices->data[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void *)offsetof(Vertex, boneIDs));

    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, boneWeights));

    glBindVertexArray(0);
}

static inline void DrawMesh(Mesh *mesh, Shader *shader) {
    // bind appropriate textures
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    GLuint normalNr = 1;
    GLuint heightNr = 1;

    int counter = 0;
    foreach (Texture *texture, mesh->textures) {
        counter++;
        // active proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + counter);

        // retrieve texture number (the N in texture_diffuseN)
        char number[16];
        char combined[128];
        char *name = textureTypetoString(texture->type);

        if (strcmp(name, "texture_diffuse") == 0)
            sprintf(number, "%u", diffuseNr++);
        else if (strcmp(name, "texture_specular") == 0)
            sprintf(number, "%u", specularNr++);
        else if (strcmp(name, "texture_normal") == 0)
            sprintf(number, "%u", normalNr++);
        else if (strcmp(name, "texture_height") == 0)
            sprintf(number, "%u", heightNr++);

        sprintf(combined, "%s%s", name, number);

        // now set the sampler to the correct texture unit
        setInt(*shader, combined, counter);

        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, texture->textureID);
    }

    // Draw Mesh
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, ListSize(mesh->indices), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // reset to default texture unit
    glActiveTexture(GL_TEXTURE0);
}

static inline void DrawModel(Model3D *model, Shader *shader) {
    foreach (Mesh *mesh, model->meshes) {
        mesh->draw(mesh, shader);
    }
}

/* Checks all material textures of a given type and loads the textures if they're not loaded yet. */
List *loadMaterialTextures(Model3D *model, CSTRUCT aiMaterial *mat, CSTRUCT aiTextureType type, char *typeName) {
    List *textures = (List *)NewList(NULL);

    for (GLuint i = 0; i < mat->GetTextureCount(type); i++) {
        CSTRUCT aiString str;
        mat->GetTexture(type, i, &str);

        /* Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture */
        bool skip = false;

        for (GLuint j = 0; j < ListSize(model->texturesLoaded); j++) {
            Texture *tex = (Texture *)ListGet(model->texturesLoaded, j);

            if (tex != NULL) {
                if (strcmp(tex->path, str) == 0) {
                    ListAdd(textures, tex);
                    skip = true; /* A 'Texture' with the same filepath has already been loaded, continue to next one. (optimization) */
                    break;
                }
            }
        }

        if (!skip) { /* If texture hasn't been loaded already, load it */
            TextureType type;

            switch (typeName) {
                case "texture_diffuse":
                    type = TEXTURE_TYPE_DIFFUSE;
                    break;
                case "texture_specular":
                    type = TEXTURE_TYPE_SPECULAR;
                    break;
                case "texture_normal":
                    type = TEXTURE_TYPE_NORMAL;
                    break;
                case "texture_height":
                    type = TEXTURE_TYPE_HEIGHT;
                    break;
                default:
                    break;
            }

            Texture *texture = (Texture *)NewTexture(type, str);
            ListAdd(textures, texture);
            ListAdd(model->texturesLoaded, texture);
        }
    }

    return textures;
}

Mesh *ProcessOurMesh(Model3D *model, CSTRUCT aiMesh *mesh, const CSTRUCT aiScene *scene) {
    /* Data to fill */
    List *vertices = (List *)NewList(NULL); /* <Vertex *> */
    List *indices = (List *)NewList(NULL);  /* <GLuint *> */
    List *textures = (List *)NewList(NULL); /* <Texture *> */

    /* Loop through each of the mesh's vertices */
    for (GLuint i = 0; i < mesh->mNumVertices; i++) {
        Vertex *vertex = (Vertex *)malloc(sizeof(Vertex));
        vec3s vector;

        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;

        vertex->position = vector;

        /* Normals */
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;

            vertex->normal = vector;
        }

        /* Tex Coords */
        if (mesh->mTextureCoords[0]) {
            vec2s vec;

            /* Vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
                use models where a Vertex can have multiple texture coordinates so we always take the first set (0).
            */
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex->texCoords = vec;

            /* Tangent */
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex->tangent = vector;

            /* Bitangent */
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex->bitangent = vector;
        } else {
            vertex->texCoords = (vec2s){0.0f, 0.0f};
        }

        ListAdd(vertices, vertex);
    }

    /* Now walk through each of the Mesh's faces and retrieve the corresponding vertex indices. */
    for (GLuint i = 0; i < mesh->mNumFaces; i++) {
        CSTRUCT aiFace face = (aiFace)mesh->mFaces[i];

        /* Retrieve all indices of the face and store them in the indices list */
        for (GLuint j = 0; j < face.mNumIndices; j++) {
            GLuint *indicesCopy = (GLuint *)malloc(sizeof(GLuint));
            *indicesCopy = face.mIndices[j];

            ListAdd(indices, indicesCopy);
        }
    }

    /* Process materials */
    CSTRUCT aiMaterial *material = (aiMaterial *)scene->mMaterials[mesh->mMaterialIndex];

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
    ListAddAll(textures, diffuseMaps);

    /* 2. specular maps */
    List *specularMaps = (List *)loadMaterialTextures(model, material, aiTextureType_SPECULAR, "texture_specular");
    ListAddAll(textures, specularMaps);

    /* 3. normal maps */
    List *normalMaps = (List *)loadMaterialTextures(model, material, aiTextureType_HEIGHT, "texture_normal");
    ListAddAll(textures, normalMaps);

    /* 4. height maps */
    List *heightMaps = (List *)loadMaterialTextures(model, material, aiTextureType_AMBIENT, "texture_height");
    ListAddAll(textures, heightMaps);

    return (Mesh *)NewMesh(vertices, indices, textures);
}

/*
    Processes a node in a recursive fashion.
    Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
*/
static inline void ProcessRootNode(Model3D *model, CSTRUCT aiNode *node, const CSTRUCT aiScene *scene) {
    /* Process each 'Mesh' located at the current node */
    for (GLuint i = 0; i < node->mNumMeshes; i++) {
        /* The node object only contains indices to index the actual objects in the scene.
           the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        */
        CSTRUCT aiMesh *mesh = (aiMesh *)scene->mMeshes[node->mMeshes[i]];
        ListAdd(model->meshes, (Mesh *)ProcessOurMesh(model, mesh, scene));
    }

    /* After we've processed all of the Meshes (if any) we then recursively process each of the children nodes */
    for (GLuint i = 0; i < node->mNumChildren; i++) {
        ProcessRootNode(model, node->mChildren[i], scene);
    }
}

#endif  // MODEL3D_H