#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// #include "nanovg.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "engine.h"
#include "ui.h"

#define lambda(lambda$_ret, lambda$_args, lambda$_body) \
    ({ lambda$_ret lambda$__anon$ lambda$_args lambda$_body &lambda$__anon$; })

#define getArraySize(array) sizeof(array) / sizeof(array[0])

#define REMOVE_MAT4S_TRANSLATION(mat) \
    mat.raw[3][0] = 0.0f;             \
    mat.raw[3][1] = 0.0f;             \
    mat.raw[3][2] = 0.0f;

static inline char *textureTypetoString(TextureType type) {
    switch (type) {
        case TEXTURE_TYPE_CUBEMAP:
            return "texture_cubemap";
        case TEXTURE_TYPE_DIFFUSE:
            return "texture_diffuse";
        case TEXTURE_TYPE_SPECULAR:
            return "texture_specular";
        case TEXTURE_TYPE_NORMAL:
            return "texture_normal";
        case TEXTURE_TYPE_HEIGHT:
            return "texture_height";
        default:
            return "texture_2D";
    }
}

static inline char *getSourceCodePath(void) {
    static char mainPath[1024];
    char buffer[1024];

#ifdef _WIN32
    // Full path of .exe
    if (GetModuleFileName(NULL, buffer, sizeof(buffer)) == 0) {
        fprintf(stderr, "[INIT ERROR] -> #getMainPath(): Unable to get module file name.\n");
        return NULL;
    }
#else
    // For Unix-like systems
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1) {
        perror("readlink");
        return NULL;
    }
    buffer[len] = '\0';
#endif

    // Remove the `build` subdirectory from the path
    char *lastSlash = strrchr(buffer, '/');  // Locate last slash
#ifdef _WIN32
    lastSlash = strrchr(buffer, '\\');
#endif

    if (lastSlash != NULL) {
        *lastSlash = '\0';  // Truncate at the last slash to remove the executable name
    }

    // Locate the `build` directory in the path and truncate it
    char *buildDir = strstr(buffer, "build");
    if (buildDir != NULL) {
        *(buildDir - 1) = '\0';  // Truncate one character before "build" to remove the slash
    }

    // Copy the resulting path to the static buffer
    strncpy(mainPath, buffer, sizeof(mainPath) - 1);
    mainPath[sizeof(mainPath) - 1] = '\0';  // Ensure null-termination

    printf("[TAV ENGINE] -> Current source directory: %s\n", mainPath);
    return mainPath;
}

static inline void printMat4(const char *variableName, const float mat[4][4]) {
    printf("%s\n", variableName);
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            printf("%f ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

static inline NVGcolor getOppositeColor(NVGcolor color) {
    return nvgRGBA(255 - color.r, 255 - color.g, 255 - color.b, color.a);
}

static inline vec3s getOppositeColorV3S(vec3s color) {
    return (vec3s){255 - color.x, 255 - color.y, 255 - color.z};
}

static inline vec3s nvgColorToV3S(NVGcolor color) {
    return (vec3s){color.r, color.g, color.b};
}

static inline NVGcolor V3SToNVGColor(vec3s color) {
    // Scale the normalized values (0.0 - 1.0) to the 255 range and clamp them
    int red   = fminf(fmaxf(color.x * 255.0f, 0.0f), 255.0f);
    int green = fminf(fmaxf(color.y * 255.0f, 0.0f), 255.0f);
    int blue  = fminf(fmaxf(color.z * 255.0f, 0.0f), 255.0f);

    // Return an NVGColor with scaled values
    return nvgRGBA(red, green, blue, 255);
}

static inline vec3s ClampColor(vec3s color) {
    color.x = fminf(color.x, 1.0f);
    color.y = fminf(color.y, 1.0f);
    color.z = fminf(color.z, 1.0f);
    return color;
}

static inline vec3s SmoothHoverColor(vec3s baseColor, bool isHovered) {
    vec3s targetColor = isHovered
                            ? glms_vec3_add(baseColor, (vec3s){{0.1f, 0.1f, 0.1f}})
                            : baseColor;

    targetColor = ClampColor(targetColor);

    return glms_vec3_lerp(baseColor, targetColor, engine->deltaTime * 5.0f);
}

static inline NVGcolor nvgSmoothHoverColor(vec3s baseColor, bool isHovered) {
    vec3s targetColor = isHovered
                            ? glms_vec3_add(baseColor, (vec3s){{0.1f, 0.1f, 0.1f}})
                            : baseColor;

    targetColor = ClampColor(targetColor);

    vec3s retColor = glms_vec3_lerp(baseColor, targetColor, engine->deltaTime * 5.0f);
    return nvgRGBA(retColor.x, retColor.y, retColor.z, 255);
}

static inline Transform Mat4ToTransform(mat4s model) {
    Transform transform;

    transform.position = (vec3s){
        model.m30,
        model.m31,
        model.m32};

    transform.scale = (vec3s){
        glms_vec3_norm((vec3s){model.m00, model.m10, model.m20}),
        glms_vec3_norm((vec3s){model.m01, model.m11, model.m21}),
        glms_vec3_norm((vec3s){model.m02, model.m12, model.m22})};

    transform.rotation.y = atan2f(model.m20, model.m00);  // Yaw
    transform.rotation.x = asinf(-model.m21);             // Pitch
    transform.rotation.z = atan2f(model.m01, model.m11);  // Roll

    return transform;
}

static inline vec3s ScreenToWorld(vec2s screenPos) {
    // Convert screen coordinates to normalized device coordinates (-1 to 1 range)
    vec4s ndc = (vec4s){
        .x = (2.0f * screenPos.raw[0]) / engine->windowWidth - 1.0f,
        .y = 1.0f - (2.0f * screenPos.raw[1]) / engine->windowHeight,
        .z = 0.0f,
        .w = 1.0f
    };

    // Unproject NDC to world space
    vec4s worldPos = glms_mat4_mulv(glms_mat4_inv(glms_mat4_mul(camera->projection, camera->view)), ndc);

    // Convert back to vec3 and normalize if needed
    return glms_vec3_scale((vec3s){worldPos.x, worldPos.y, worldPos.z}, 1.0f / worldPos.w);
}


static inline vec3s CalculateDelta(vec2s currentCursor, vec2s previousCursor, vec3s selectedAxis) {
    // Convert screen-space cursor positions to world-space
    vec3s worldCurrent = ScreenToWorld(currentCursor);
    vec3s worldPrevious = ScreenToWorld(previousCursor);

    // Calculate raw delta in world-space
    vec3s rawDelta = glms_vec3_sub(worldCurrent, worldPrevious);

    // Constrain delta along the selected axis
    vec3s constrainedDelta = glms_vec3_scale(selectedAxis, glms_vec3_dot(rawDelta, selectedAxis));
    
    return constrainedDelta;
}

static inline bool IsAxisSelectionActive(void) {
    vec3s axis = engine->selectedAxis;
    return axis.x != 0 && axis.y != 0 && axis.z != 0;
}

bool isPointInsideElement(Element *element, vec2s cursor);
bool isPointInside3DObj(SceneObject *object, Model3D *model, vec2s worldCursor);

void TransformGizmoUpdateObject(SceneObject *object, Model3D *model, vec3s delta, vec2s cursor);

#endif  // UTILS_H