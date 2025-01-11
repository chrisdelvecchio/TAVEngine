#include "utils.h"
#include "physics.h"

bool isPointInsideElement(Element *element, vec2s cursor) {
    if (element != NULL) {
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
    }

    return GLFW_FALSE;
}

// if (object != NULL) {
//     BoundingBox *boundingBox = (object != NULL) ? object->transforms->boundingBox : model->transforms->boundingBox;

//     if (boundingBox != NULL) {
//         // Helper function to project a 3D point into 2D screen space
//         vec2s projectToScreenSpace(vec3s point) {
//             vec4 worldPos = {point.x, point.y, point.z, 1.0f};

//             // Apply view and projection matrices
//             vec4 clipSpacePos;
//             glm_mat4_mulv(camera->view.raw, worldPos, clipSpacePos);
//             glm_mat4_mulv(camera->projection.raw, clipSpacePos, clipSpacePos);

//             // Perspective divide
//             if (clipSpacePos[3] != 0.0f) {
//                 clipSpacePos[0] /= clipSpacePos[3];
//                 clipSpacePos[1] /= clipSpacePos[3];
//             }

//             // Convert NDC to screen space
//             return (vec2s){
//                 (clipSpacePos[0] + 1.0f) * 0.5f * engine->windowWidth,
//                 (1.0f - clipSpacePos[1]) * 0.5f * engine->windowHeight // Invert Y
//             };
//         }

//         // Negate min for projection if required
//         vec2s minScreen = projectToScreenSpace(boundingBox->min);
//         vec2s maxScreen = projectToScreenSpace(boundingBox->max);

//         // Check if the cursor is inside the projected 2D bounding box
//         return (cursor.x >= fmin(minScreen.x, maxScreen.x) && cursor.x <= fmax(minScreen.x, maxScreen.x)) &&
//                (cursor.y >= fmin(minScreen.y, maxScreen.y) && cursor.y <= fmax(minScreen.y, maxScreen.y));
//     }
// }
bool isPointInside3DObj(SceneObject *object, Model3D *model, vec2s cursor) {
    if (object != NULL) {
        BoundingBox *boundingBox = (object != NULL) ? object->transforms->boundingBox : model->transforms->boundingBox;

        // vec3s unprojectedPosition = glms_unproject(object->transforms->position, object->transforms->model, glms_mat4_mul(camera->projection, camera->view));

        if (boundingBox != NULL) {
            // Helper function to project a 3D point into 2D screen space
            vec2s projectToScreenSpace(vec3s point) {
                vec4 worldPos = {point.x, point.y, point.z, 1.0f};

                // Apply view and projection matrices
                vec4 clipSpacePos;
                glm_mat4_mulv(camera->view.raw, worldPos, clipSpacePos);
                glm_mat4_mulv(camera->projection.raw, clipSpacePos, clipSpacePos);

                // Perspective divide
                if (clipSpacePos[3] != 0.0f) {
                    clipSpacePos[0] /= clipSpacePos[3];
                    clipSpacePos[1] /= clipSpacePos[3];
                }

                // Convert NDC to screen space
                return (vec2s){
                    (clipSpacePos[0] + 1.0f) * 0.5f * engine->windowWidth,
                    (1.0f - clipSpacePos[1]) * 0.5f * engine->windowHeight  // Invert Y
                };
            }

            // Negate min for projection if required
            vec2s minScreen = projectToScreenSpace(boundingBox->min);
            vec2s maxScreen = projectToScreenSpace(boundingBox->max);

            // Check if the cursor is inside the projected 2D bounding box
            return (cursor.x >= fmin(minScreen.x, maxScreen.x) && cursor.x <= fmax(minScreen.x, maxScreen.x)) &&
                   (cursor.y >= fmin(minScreen.y, maxScreen.y) && cursor.y <= fmax(minScreen.y, maxScreen.y));
        }
    }

    // TODO: isPointInside3DObj

    return GLFW_FALSE;
}
static inline void DetectSelectedAxis(SceneObject *object, Model3D *model, vec2s cursor) {
    Transform *transforms = (object != NULL) ? object->transforms : model->transforms;
    TransformGizmo gizmo = (object != NULL) ? object->gizmo : model->gizmo;
    Axis *axes = (Axis *)gizmo.axes;

    vec3s position = transforms->position;
    vec3s selectedAxis = (vec3s){0.0f, 0.0f, 0.0f};

    float closestDistance = INFINITY;
    float distance;

    Ray ray = (Ray)GenerateRay(camera, cursor);

    // X-axis
    Axis x_axis = (Axis)axes[0];
    if (RayIntersectsAxis(ray, position.raw, x_axis.position.raw, closestDistance, &distance)) {
        printf("X_AXIS true\n");
        selectedAxis = (vec3s){x_axis.rotation.x, x_axis.rotation.y, x_axis.rotation.z};
        gizmo.lines[0].color = (vec3s){ENGINE_SELECTED_COLOR};
        gizmo.triangles[0].color = (vec3s){ENGINE_SELECTED_COLOR};
    }

    // Y-axis
    Axis y_axis = (Axis)axes[1];
    if (RayIntersectsAxis(ray, position.raw, y_axis.position.raw, closestDistance, &distance)) {
        printf("Y_AXIS true\n");
        selectedAxis = (vec3s){y_axis.rotation.x, y_axis.rotation.y, y_axis.rotation.z};
        gizmo.lines[1].color = (vec3s){ENGINE_SELECTED_COLOR};
        gizmo.triangles[1].color = (vec3s){ENGINE_SELECTED_COLOR};
    }

    Axis z_axis = (Axis)axes[2];
    // Z-axis
    if (RayIntersectsAxis(ray, position.raw, z_axis.position.raw, closestDistance, &distance)) {
        printf("Z_AXIS true\n");
        selectedAxis = (vec3s){z_axis.rotation.x, z_axis.rotation.y, z_axis.rotation.z};
        gizmo.lines[2].color = (vec3s){ENGINE_SELECTED_COLOR};
        gizmo.triangles[2].color = (vec3s){ENGINE_SELECTED_COLOR};
    }

    if (object != NULL) {
        object->gizmo = gizmo;
    } else if (model != NULL) {
        model->gizmo = gizmo;
    }

    engine->selectedAxis = selectedAxis;
}

void TransformGizmoUpdateObject(SceneObject *object, Model3D *model, vec3s delta, vec2s cursor) {
    Transform *transforms = (object != NULL) ? object->transforms : model->transforms;
    TransformGizmo gizmo = (object != NULL) ? object->gizmo : model->gizmo;
    Axis *axes = (Axis *)gizmo.axes;

    vec3s position = transforms->position;
    vec3s selectedAxis = engine->selectedAxis;

    DetectSelectedAxis(object, model, cursor);

    if (axes[0].type == AXIS_X) {
        printf("X_AXIS update true\n");
        transforms->position.raw[0] += delta.raw[0];
    } else if (axes[1].type == AXIS_Y) {
        printf("Y_AXIS update true\n");
        transforms->position.raw[1] += delta.raw[1];
    } else if (axes[2].type == AXIS_Z) {
        printf("Z_AXIS update true\n");
        transforms->position.raw[2] += delta.raw[2];
    }

    // if (selectedAxis.x == 1.0f) {
    //     printf("X_AXIS update true\n");
    //     transforms->position.raw[0] += delta.raw[0];
    // } else if (selectedAxis.y == 1.0f) {
    //     printf("Y_AXIS update true\n");
    //     transforms->position.raw[1] += delta.raw[1];
    // } else if (selectedAxis.z == 1.0f) {
    //     printf("Z_AXIS update true\n");
    //     transforms->position.raw[2] += delta.raw[2];
    // }
}