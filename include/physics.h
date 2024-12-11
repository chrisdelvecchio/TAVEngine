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

extern PhysicsManager *physicsManager;

#endif // PHYSICS_H