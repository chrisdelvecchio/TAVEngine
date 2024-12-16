#include "camera.h"
#include "render.h"

static inline void UpdateCameraVectors(Camera *camera) {
    camera->projection = glms_perspective(glm_rad(camera->fov), engine->aspectRatio, 0.1f, camera->renderDistance);
    camera->view = glms_lookat(camera->position, glms_vec3_add(camera->position, camera->front), camera->up);

    vec3s front;
    front.x = cos(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));
    front.y = sin(glm_rad(camera->pitch));
    front.z = sin(glm_rad(camera->yaw)) * cos(glm_rad(camera->pitch));

    camera->front = glms_vec3_normalize(front);
    camera->right = glms_vec3_normalize(glms_vec3_cross(camera->front, camera->worldUp));
    camera->up = glms_vec3_normalize(glms_vec3_cross(camera->right, camera->front));
}

Camera *NewCamera(vec3s position, float fov) {
    Camera *camera = (Camera *)malloc(sizeof(Camera));
    camera->position = position;
    camera->fov = fov;
    camera->movementSpeed = 60.0f;
    camera->mouseSensitivity = 0.2f;
    camera->renderDistance = 1000.0f;
    camera->yaw = -90.0f;
    camera->pitch = 0.0f;
    camera->up = (vec3s){0.0f, 1.0f, 0.0f};
    camera->front = (vec3s){0.0f, 0.0f, -1.0f};
    camera->worldUp = camera->up;
    camera->velocity = GLMS_VEC3_ZERO;
    camera->maxVelocity = 10.0f;
    camera->update = UpdateCameraVectors;

    UpdateCameraVectors(camera);

    camera->object = (SceneObject *)NewSprite(position, 10.0f, GLFW_TRUE, "camera_texture.png");
    camera->object->color = (vec3s){0.3f, 0.0f, 0.3f};
    camera->object->type |= OBJECT_CAMERA;

    ListAdd(engine->cameras, camera);
    return camera;
}

void processKeyboard(Camera *camera) {
    float cameraSpeed = camera->movementSpeed * engine->deltaTime;

    // Move forward (W key)
    if (glfwGetKey(engine->window, GLFW_KEY_W) == GLFW_PRESS)
        camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->front, cameraSpeed));

    // Move backward (S key)
    if (glfwGetKey(engine->window, GLFW_KEY_S) == GLFW_PRESS)
        camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->front, cameraSpeed));

    // Strafe left (A key)
    if (glfwGetKey(engine->window, GLFW_KEY_A) == GLFW_PRESS)
        camera->position = glms_vec3_sub(camera->position, glms_normalize(
                                                               glms_vec3_scale(glms_cross(camera->front, camera->up), cameraSpeed)));

    // Strafe right (D key)
    if (glfwGetKey(engine->window, GLFW_KEY_D) == GLFW_PRESS)
        camera->position = glms_vec3_add(camera->position, glms_normalize(
                                                               glms_vec3_scale(glms_cross(camera->front, camera->up), cameraSpeed)));

    // Fly up (Space bar)
    if (glfwGetKey(engine->window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera->position = glms_vec3_add(camera->position, glms_vec3_scale(camera->up, cameraSpeed));

    // Fly down (Left Shift key)
    if (glfwGetKey(engine->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera->position = glms_vec3_sub(camera->position, glms_vec3_scale(camera->up, cameraSpeed));
}

void processMouse(Camera *camera, double xposIn, double yposIn) {
    float xpos = (float)xposIn;
    float ypos = (float)yposIn;

    if (engine->firstMouse) {
        engine->lastX = xpos;
        engine->lastY = ypos;
        engine->firstMouse = GLFW_FALSE;
    }

    float xoffset = xpos - engine->lastX;
    float yoffset = engine->lastY - ypos;  // reversed since y-coordinates go from bottom to top
    engine->lastX = xpos;
    engine->lastY = ypos;

    float sensitivity = camera->mouseSensitivity;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->yaw += xoffset;
    camera->pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (camera->pitch > 89.0f)
        camera->pitch = 89.0f;
    if (camera->pitch < -89.0f)
        camera->pitch = -89.0f;

    camera->update(camera);
}