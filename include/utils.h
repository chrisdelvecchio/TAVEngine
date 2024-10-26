#pragma once

#ifndef UTILS_H
#define UTILS_H

#include <stdbool.h>

// #include "nanovg.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "ui.h"

#define lambda(lambda$_ret, lambda$_args, lambda$_body) \
    ({ lambda$_ret lambda$__anon$ lambda$_args lambda$_body& lambda$__anon$; })

#define getArraySize(array) sizeof(array) / sizeof(array[0])

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

bool isPointInsideElement(Element* element, vec2s cursor);

#endif  // UTILS_H