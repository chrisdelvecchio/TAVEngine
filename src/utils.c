#include "utils.h"

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

bool isPointInside3DObj(SceneObject *object, Model3D *model, vec2s cursor) {
    if (object != NULL) {
        BoundingBox *boundingBox = (object != NULL) ? object->transforms->boundingBox : model->transforms->boundingBox;

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
                    (1.0f - clipSpacePos[1]) * 0.5f * engine->windowHeight // Invert Y
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

    return GLFW_FALSE;
}
