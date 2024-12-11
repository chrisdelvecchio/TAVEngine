#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include "engine.h"

Camera *NewCamera(vec3s position, float fov);

void processKeyboard(Camera *camera);
void processMouse(Camera *camera, double xposIn, double yposIn);

#endif // CAMERA_H