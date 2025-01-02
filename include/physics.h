#pragma once

#ifndef PHYSICS_H
#define PHYSICS_H

#include "engine.h"
#include <stdbool.h>

typedef struct PhysicsManager {
    bool active;
    float gravity;
} PhysicsManager;

void initPhysics(void);
void togglePhysics(void);
void step(void);

Ray GenerateRay(Camera *camera, vec2s cursor);
bool RayIntersectsAxis(Ray ray, vec3 axis_start, vec3 axis_end, float threshold, float *t_out);

extern PhysicsManager *physicsManager;

#endif // PHYSICS_H