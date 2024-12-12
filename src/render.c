#include "render.h"

#include "shader.h"
#include "stb_image.h"
#include "utils.h"

static SceneObject *mainFrameBufferScreenQuad = NULL;

static SceneObject *CreateScreenQuad(FrameBufferObject *frameBuffer) {
    Vertex vertices[] = {
        // Position           // Normal            // TexCoords
        {{-1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},  // Top-left
        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},   // Top-right
        {{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},  // Bottom-right
        {{-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}  // Bottom-left
    };

    // Indices for two triangles forming a quad
    GLuint indices[] = {
        0, 1, 2,  // First triangle
        0, 2, 3   // Second triangle
    };

    int vertexCount = getArraySize(vertices);
    int indexCount = getArraySize(indices);

    MeshData *meshData = (MeshData *)GetMeshCopies(vertices, vertexCount, indices, indexCount);

    SceneObject *object = (SceneObject *)NewSceneObject((SceneObject){
        .type = OBJECT_FRAMEBUFFER_QUAD,
        .tag = "ANTI-ALIAS FRAMEBUFFER QUAD",
        .shader = antiAliasShader,
        .color = (vec3s){0.0f, 1.0f, 1.0f},
        .meshData = meshData,
        .vertices = meshData->verticesCopy,
        .indices = meshData->indicesCopy,
        .vertexCount = vertexCount,
        .indexCount = indexCount});

    Transform *transforms = (Transform *)object->transforms;
    transforms[0] = (Transform){0};
    object->transforms = transforms;
    return object;
}

static void DrawFrameBufferObject(FrameBufferObject *frameBuffer) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer->frameBufferID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->intermediateFBO);
    glBlitFramebuffer(0, 0, engine->windowWidth, engine->windowHeight, 0, 0, engine->windowWidth, engine->windowHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    // 3. now render quad with scene's visuals as its texture image
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(BACKGROUND_COLOR);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    if (mainFrameBufferScreenQuad) {
        // draw Screen quad
        UseShader(*mainFrameBufferScreenQuad->shader);
        setInt(*mainFrameBufferScreenQuad->shader, "screenTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, frameBuffer->screenTexture);  // use the now resolved color attachment as the mainFrameBufferScreenQuad's texture

        mainFrameBufferScreenQuad->draw(mainFrameBufferScreenQuad);
    } else {
        printf("[DRAW ERROR] => Screen quad 'SceneObject' is NULL.\n");
    }
}

FrameBufferObject *BindFrameBuffer(FrameBufferObject frameBuffer) {
    FrameBufferObject *newFrameBuffer = (FrameBufferObject *)malloc(sizeof(FrameBufferObject));
    if (newFrameBuffer == NULL) {
        printf("[MEMORY ALLOCATION FAILURE] => #BINDFRAMEBUFFER Memory allocation failed.\n");
        return NULL;
    }

    memcpy(newFrameBuffer, &frameBuffer, sizeof(FrameBufferObject));

    glGenFramebuffers(1, &newFrameBuffer->frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuffer->frameBufferID);

    glGenTextures(1, &newFrameBuffer->texColorBufferID);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, newFrameBuffer->texColorBufferID);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, engine->windowWidth, engine->windowHeight, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, newFrameBuffer->texColorBufferID, 0);

    glGenRenderbuffers(1, &newFrameBuffer->depthStencilBufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, newFrameBuffer->depthStencilBufferID);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, engine->windowWidth, engine->windowHeight);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, newFrameBuffer->depthStencilBufferID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("[OpenGL Framebuffer Error] Depth Stencil Framebuffer is not complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &newFrameBuffer->intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, newFrameBuffer->intermediateFBO);

    // create a color attachment texture
    glGenTextures(1, &newFrameBuffer->screenTexture);
    glBindTexture(GL_TEXTURE_2D, newFrameBuffer->screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, engine->windowWidth, engine->windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, newFrameBuffer->screenTexture, 0);  // we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("[OpenGL Framebuffer Error] Screen Texture Framebuffer is not complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (mainFrameBufferScreenQuad == NULL) {
        mainFrameBufferScreenQuad = (SceneObject *)CreateScreenQuad(newFrameBuffer);
    }

    newFrameBuffer->drawBuffer = DrawFrameBufferObject;

    return newFrameBuffer;
}

void UnbindFrameBufferObj(FrameBufferObject *frameBuffer) {
    if (frameBuffer) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDeleteFramebuffers(1, &frameBuffer->frameBufferID);
        glDeleteTextures(1, &frameBuffer->texColorBufferID);
        glDeleteRenderbuffers(1, &frameBuffer->depthStencilBufferID);

        free(frameBuffer);

        printf("[TAV ENGINE] Screen Framebuffer Object has been freed!\n");
    }
}

static void HandleShaderTransform(SceneObject *object, Shader *shader, bool isInstanced) {
    if (isInstanced) {
        mat4s instanceMatrices[object->instanceCount];

        for (int i = 0; i < object->instanceCount; i++) {
            Transform transform = object->transforms[i];
            mat4s model = glms_mat4_identity();

            model = glms_translate(model, transform.position);
            model = glms_rotate(model, glm_rad(transform.rotationDegrees), transform.rotation);

            if (transform.scale.x == 0.0f && transform.scale.y == 0.0f && transform.scale.z == 0.0f) {
                transform.scale = (vec3s)GLMS_VEC3_ONE;
            }

            model = glms_scale(model, transform.scale);
            instanceMatrices[i] = model;
        }

        UpdateInstancedBufferObj(object, instanceMatrices, object->instanceCount);
    } else {
        // Non-instanced handling
        Transform transform = object->transforms[0];
        mat4s model = glms_mat4_identity();

        if (transform.scale.x == 0.0f && transform.scale.y == 0.0f && transform.scale.z == 0.0f) {
            transform.scale = (vec3s)GLMS_VEC3_ONE;
        }

        model = glms_translate(model, transform.position);
        model = glms_rotate(model, glm_rad(transform.rotationDegrees), transform.rotation);
        model = glms_scale(model, transform.scale);

        setMat4(*shader, "model", &model);
    }
}

void SendToShader(SceneObject *object) {
    camera->update(camera);

    int instanceCount = object->instanceCount;

    if (instanceCount > 1) {
        UseShader(*instanceShader);
        setMat4(*instanceShader, "projection", &camera->projection);
        setMat4(*instanceShader, "view", &camera->view);
        setInt(*instanceShader, "instanceCount", instanceCount);

        HandleShaderTransform(object, instanceShader, GLFW_TRUE);

        if (object->texture != NULL) {
            setBool(*instanceShader, "useTexture", GLFW_TRUE);
            setInt(*instanceShader, "texture1", 0);
        } else {
            setBool(*instanceShader, "useTexture", GLFW_FALSE);
            setVec3(*instanceShader, "color", &object->color);
        }
    } else {
        UseShader(*object->shader);
        setMat4(*object->shader, "projection", &camera->projection);
        setMat4(*object->shader, "view", &camera->view);
        HandleShaderTransform(object, object->shader, GLFW_FALSE);

        if (object->texture != NULL) {
            setBool(*object->shader, "useTexture", GLFW_TRUE);
            setInt(*object->shader, "texture1", 0);
        } else {
            setBool(*object->shader, "useTexture", GLFW_FALSE);
            setVec3(*object->shader, "color", &object->color);
        }
    }
}

static void DrawSceneObject(SceneObject *object) {
    SendToShader(object);
    glBindVertexArray(object->VAO);
    UseTexture(object->texture);

    int indexCount = object->indexCount;

    if (object->indices != NULL && indexCount > 0) {
        int instanceCount = object->instanceCount;

        if (instanceCount > 1) {
            glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0, instanceCount);
        } else {
            glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
        }
    } else {
        glDrawArrays(GL_TRIANGLES, 0, object->vertexCount);
    }

    glBindVertexArray(0);
}

SceneObject *NewSceneObject(SceneObject object) {
    SceneObject *newSceneObject = (SceneObject *)malloc(sizeof(SceneObject));
    if (newSceneObject == NULL) {
        fprintf(stderr, "[MEMORY ERROR] Failed creating new Scene Object, ERROR ALLOCATING MEMORY\n");
        free(newSceneObject);
        return NULL;
    }

    memcpy(newSceneObject, &object, sizeof(SceneObject));

    if (newSceneObject->shader == NULL) {
        newSceneObject->shader = defaultShader;
    }

    if (newSceneObject->instanceCount == 0) {
        newSceneObject->instanceCount = 1;
    }

    if (newSceneObject->transforms == NULL) {
        newSceneObject->transforms = (Transform *)malloc(newSceneObject->instanceCount * sizeof(Transform));
        newSceneObject->transforms[0].model = glms_mat4_identity();
    }

    if (newSceneObject->draw == NULL) {
        newSceneObject->draw = DrawSceneObject;
    }

    BindBufferObj(newSceneObject);

    ListAdd(engine->sceneObjects, newSceneObject);
    return newSceneObject;
}

SceneObject *CopySceneObject(SceneObject *object) {
    if (object != NULL) {
        return (SceneObject *)NewSceneObject(*object);
    }
}

Transform *NewTransforms(int instanceCount, Transform *transforms) {
    if (instanceCount < 1) instanceCount = 1;

    Transform *newTransforms = (Transform *)malloc(instanceCount * sizeof(Transform));
    if (newTransforms == NULL) {
        fprintf(stderr, "[MEMORY ERROR] Failed creating new Transform, ERROR ALLOCATING MEMORY\n");
        free(newTransforms);
        return NULL;
    }

    memcpy(newTransforms, transforms, sizeof(Transform));
    newTransforms[0].model = glms_mat4_identity();

    return newTransforms;
}

void UseTexture(Texture *texture) {
    if (texture != NULL) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture->textureID);
    }
}

Texture *NewTexture(const char *path) {
    Texture *texture = (Texture *)malloc(sizeof(Texture));
    texture->textureID = 0;
    texture->width = 0;
    texture->height = 0;
    texture->nrChannels = 0;

    // Load texture
    glGenTextures(1, &texture->textureID);
    glBindTexture(GL_TEXTURE_2D, texture->textureID);

    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(getAssetPath(path), &texture->width, &texture->height, &texture->nrChannels, 0);
    if (data) {
        GLenum format;
        if (texture->nrChannels == 1)
            format = GL_RED;
        else if (texture->nrChannels == 3)
            format = GL_RGB;
        else if (texture->nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, texture->width, texture->height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        printf("[TEXTURE ERROR] Failed to load texture at path: %s\n", path);
    }

    stbi_image_free(data);
    return texture;
}

void UpdateInstancedBufferObj(SceneObject *object, mat4s *instanceMatrices, int instanceCount) {
    glBindBuffer(GL_ARRAY_BUFFER, object->IVBO);
    glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(mat4s), instanceMatrices, GL_DYNAMIC_DRAW);
}

void BindBufferObj(SceneObject *object) {
    glGenVertexArrays(1, &object->VAO);
    glGenBuffers(1, &object->VBO);
    glGenBuffers(1, &object->EBO);
    glGenBuffers(1, &object->IVBO);

    glBindVertexArray(object->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, object->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * object->vertexCount, object->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, object->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * object->indexCount, object->indices, GL_STATIC_DRAW);

    // Position attribute (layout = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute (layout = 1)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, texCoords)));
    glEnableVertexAttribArray(1);

    vec3s normal = object->vertices[0].normal;
    if (normal.x != 0 && normal.y != 0 && normal.z != 0) {
        // Normal attribute (layout = 2)
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(2);
    }

    // Instance matrix attribute (layout = 3)
    if (object->instanceCount > 1) {
        glBindBuffer(GL_ARRAY_BUFFER, object->IVBO);

        glBufferData(GL_ARRAY_BUFFER, object->instanceCount * sizeof(mat4s), &object->transforms->model.raw[0], GL_DYNAMIC_DRAW);

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
}

void UnbindBufferObj(SceneObject *object) {
    // Unbind VAO, VBO, and EBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Delete the buffers
    glDeleteVertexArrays(1, &object->VAO);
    glDeleteBuffers(1, &object->VBO);
    glDeleteBuffers(1, &object->IVBO);
    glDeleteBuffers(1, &object->EBO);

    object->VAO = 0;
    object->VBO = 0;
    object->IVBO = 0;
    object->EBO = 0;
}

void RemoveSceneObject(SceneObject *object) {
    if (ObjectExists(object)) {
        if (ListContains(engine->sceneObjects, object)) {
            ListRemove(engine->sceneObjects, object);
        }

        FreeupObject(object);
        printf("[SCENE OBJECT] Destroyed successfully!\n");
    }
}