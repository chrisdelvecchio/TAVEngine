#pragma once

#ifndef MODEL3D_H
#define MODEL3D_H

#include "engine.h"
#include "utils.h"
#include "shader.h"

Mesh *NewMesh(List *vertices, List *indices, List *textures);

static void SetupMesh(Mesh *mesh) {
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

static void DrawMesh(Mesh *mesh, Shader *shader) {
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

#endif  // MODEL3D_H