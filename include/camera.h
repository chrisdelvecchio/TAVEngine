#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"

Camera *NewCamera(vec3s position, float fov);

void processKeyboard(Camera *camera);
void processMouse(Camera *camera, double xposIn, double yposIn);

static inline bool CameraExists(Camera *camera) {
    return camera != NULL;
}

static inline void RemoveCameras(void) {
    int counter = 0;
    foreach (Camera *cam, engine->cameras) {
        if (!CameraExists(cam)) continue;
        counter++;
    }

    if (!isListEmpty(engine->cameras)) {
        ListClear(engine->cameras);
    }

    printf("[TAV ENGINE] %d Cameras have been freed!\n", counter);
}

#endif // CAMERA_H