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
    mat.raw[3][0] = 0.0f; \
    mat.raw[3][1] = 0.0f; \
    mat.raw[3][2] = 0.0f;

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
    char *lastSlash = strrchr(buffer, '/'); // Locate last slash
#ifdef _WIN32
    lastSlash = strrchr(buffer, '\\');
#endif

    if (lastSlash != NULL) {
        *lastSlash = '\0'; // Truncate at the last slash to remove the executable name
    }

    // Locate the `build` directory in the path and truncate it
    char *buildDir = strstr(buffer, "build");
    if (buildDir != NULL) {
        *(buildDir - 1) = '\0'; // Truncate one character before "build" to remove the slash
    }

    // Copy the resulting path to the static buffer
    strncpy(mainPath, buffer, sizeof(mainPath) - 1);
    mainPath[sizeof(mainPath) - 1] = '\0'; // Ensure null-termination

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

bool isPointInsideElement(Element *element, vec2s cursor);

#endif  // UTILS_H