#include "utils.h"

bool isPointInsideElement(Element *element, vec2s cursor) {
    float elementX = element->transform.position.x;
    float elementY = element->transform.position.y;

    if (element->type == ELEMENT_RECT || element->type == ELEMENT_BUTTON) {
        float rectWidth = element->width;
        float rectHeight = element->height;

        if (cursor.x >= elementX && cursor.x <= elementX + rectWidth &&
            cursor.y >= elementY && cursor.y <= elementY + rectHeight) {
            return true;
        }
    }

    return false;
}