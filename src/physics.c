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

Ray GenerateRay(Camera *camera, vec2s cursor) {
    Ray ray;

    mat4s projection = camera->projection;
    mat4s view = camera->view;

    float windowWidth = engine->windowWidth;
    float windowHeight = engine->windowHeight;
    float cursorX = cursor.x;
    float cursorY = cursor.y;

    // Normalize mouse coordinates to the range [-1, 1]
    float normalizedX = (2.0f * cursorX) / windowWidth - 1.0f;
    float normalizedY = 1.0f - (2.0f * cursorY) / windowHeight;  // Invert Y for OpenGL

    // Create clip space coordinates (Z = -1 for near plane, Z = 1 for far plane)
    vec4s clipCoords = (vec4s){normalizedX, normalizedY, -1.0f, 1.0f};

    // Unproject to eye space
    vec4s eyeCoords;
    glms_mat4_inv(projection);  // Invert projection matrix
    glm_mat4_mulv(projection.raw, clipCoords.raw, eyeCoords.raw);

    eyeCoords.raw[2] = -1.0f;  // Set forward direction in eye space
    eyeCoords.raw[3] = 0.0f;   // Make it a direction vector

    // Unproject to world space
    vec4s worldCoords;
    glms_mat4_inv(view);  // Invert view matrix
    glm_mat4_mulv(view.raw, eyeCoords.raw, worldCoords.raw);

    // Normalize the ray direction
    glm_vec3_copy(worldCoords.raw, ray.direction.raw);
    glm_vec3_normalize(ray.direction.raw);

    // Ray origin is the camera's position
    // glm_mat4_pick3(view.raw, ray.origin.raw);
    ray.origin.raw[0] = view.raw[3][0];
    ray.origin.raw[1] = view.raw[3][1];
    ray.origin.raw[2] = view.raw[3][2];
    return ray;
}

bool RayIntersectsAxis(Ray ray, vec3 axis_start, vec3 axis_end, float threshold, float *t_out) {
    // Axis vector
    vec3 axis_dir;
    glm_vec3_sub(axis_end, axis_start, axis_dir);

    // Normalize the axis direction
    glm_vec3_normalize(axis_dir);

    // Vector from ray origin to axis start
    vec3 origin_to_axis;
    glm_vec3_sub(axis_start, ray.origin.raw, origin_to_axis);

    // Project the vector onto the ray direction
    float t = glm_vec3_dot(origin_to_axis, ray.direction.raw);

    // Closest point on the ray
    vec3 closest_point;
    glm_vec3_scale(ray.direction.raw, t, closest_point);
    glm_vec3_add(ray.origin.raw, closest_point, closest_point);

    // Distance from closest point to the axis
    vec3 axis_closest;
    glm_vec3_sub(closest_point, axis_start, axis_closest);

    float distance = glm_vec3_norm(axis_closest);

    // Check if the distance is within the threshold
    if (distance < threshold) {
        if (t_out) *t_out = t;
        return GLFW_TRUE;
    }

    return GLFW_FALSE;
}