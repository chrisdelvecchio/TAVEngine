#include "utils.h"

bool isPointInsideElement(Element *element, vec2s cursor) {
    float elementX = element->transform.position.x;
    float elementY = element->transform.position.y;

    if (element->type == ELEMENT_RECT || element->type == ELEMENT_BUTTON) {
        float rectWidth = element->width;
        float rectHeight = element->height;

        if (cursor.x >= elementX && cursor.x <= elementX + rectWidth &&
            cursor.y >= elementY && cursor.y <= elementY + rectHeight) {
            return GLFW_TRUE;
        }
    }

    return GLFW_FALSE;
}

bool isPointInside3DObj(SceneObject *object, Model3D *model, vec2s cursor) {
    BoundingBox boundingBox = (object != NULL) ? object->transforms->boundingBox : model->transforms->boundingBox;
    vec3s pos = (object != NULL) ? object->transforms->position : model->transforms->position;
    vec3s scale = (object != NULL) ? object->transforms->scale : model->transforms->scale;

    // Define constants for bounding box calculation
    float halfScale = 0.5f;     // Half the size of the model's scale
    float homogeneousW = 1.0f;  // Homogeneous coordinate w for 3D positions

    // Define the model's bounding box in world coordinates
    vec3s min = boundingBox.min;
    vec3s max = boundingBox.max;

    // Project the bounding box corners to screen space
    vec4s corners[8] = {
        {min.x, min.y, min.z, homogeneousW},
        {max.x, min.y, min.z, homogeneousW},
        {min.x, max.y, min.z, homogeneousW},
        {max.x, max.y, min.z, homogeneousW},
        {min.x, min.y, max.z, homogeneousW},
        {max.x, min.y, max.z, homogeneousW},
        {min.x, max.y, max.z, homogeneousW},
        {max.x, max.y, max.z, homogeneousW}};

    vec2s screenCorners[8];
    for (int i = 0; i < 8; ++i) {
        vec4s clipSpace = glms_mat4_mulv(camera->projection,
                                         glms_mat4_mulv(camera->view, corners[i]));
        screenCorners[i] = (vec2s){
            clipSpace.x / clipSpace.w,  // Normalize x to [-1, 1]
            clipSpace.y / clipSpace.w   // Normalize y to [-1, 1]
        };
    }

    // Find the 2D bounding box in screen space
    float minX = 1.0f, minY = 1.0f, maxX = -1.0f, maxY = -1.0f;
    for (int i = 0; i < 8; ++i) {
        if (screenCorners[i].x < minX) minX = screenCorners[i].x;
        if (screenCorners[i].y < minY) minY = screenCorners[i].y;
        if (screenCorners[i].x > maxX) maxX = screenCorners[i].x;
        if (screenCorners[i].y > maxY) maxY = screenCorners[i].y;
    }

    const float NDC_SCALE = 2.0f;  // Scales screen space to NDC
    const float NDC_SHIFT = 1.0f;  // Shifts NDC range to [-1.0, 1.0]

    // Convert cursor to normalized device coordinates (NDC)
    vec2s cursorNDC = {
        NDC_SCALE * cursor.x / engine->windowWidth - NDC_SHIFT,
        NDC_SHIFT - NDC_SCALE * cursor.y / engine->windowHeight};

    // Check if the cursor is inside the 2D bounding box
    return (cursorNDC.x >= minX && cursorNDC.x <= maxX && cursorNDC.y >= minY && cursorNDC.y <= maxY);
}
