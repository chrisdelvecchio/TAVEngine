#include "physics.h"

PhysicsManager *physicsManager;

void initPhysics(void) {
    PhysicsManager *physicsManager = (PhysicsManager *)malloc(sizeof(PhysicsManager));
    physicsManager->active = GLFW_TRUE;
    physicsManager->gravity = 9.81f;

}
void togglePhysics(void) {
    physicsManager->active = !physicsManager->active;
}
void step(void) {
    if (physicsManager->active) {
        // simulate physics
    }
}