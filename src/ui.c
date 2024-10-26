#include "ui.h"

#include <string.h>

#include "utils.h"

Menu *menu;

void initUI(void) {
    printf("[Game] Initializing UI...\n");
    if (menu == NULL) {
        menu = (Menu *)malloc(sizeof(Menu));
    }

    menu->shown = GLFW_TRUE;
    menu->elements = NewList(NULL);
}

void destroyUI(void) {
    menu->shown = GLFW_FALSE;

    foreach (Element *element, menu->elements) {
        DestroyElement(element);
    }
}

void DestroyElement(Element *element) {
    if (element == NULL) return;

    element->created = GLFW_FALSE;
    element->onClick = NULL;

    element = NULL;
}

void toggleMenu(void) {
    menu->shown = !menu->shown;
}

Element *CreateElement(Element element) {
    Element *newElement = (Element *)malloc(sizeof(Element));
    if (newElement == NULL) {
        fprintf(stderr,
                "[MEMORY ERROR] Failed to allocate memory for "
                "CreateElement();\n");
        return NULL;
    }

    memcpy(newElement, &element, sizeof(Element));

    if (newElement->created == GLFW_FALSE) {
        newElement->created = GLFW_TRUE;
    }

    if (newElement->transform.position.x == 0.0f && newElement->transform.position.y == 0.0f) {
        newElement->transform = (Transform){
            .position = (vec3s){0.0f, 0.0f, 0.0f}};
    }

    newElement->textScale = (newElement->textScale > 0) ? newElement->textScale : 24.0f;

    if (newElement->color.a == 0 && newElement->color.r == 0 && newElement->color.g == 0 && newElement->color.b == 0) {
        newElement->color = nvgRGBA(255, 255, 255, 255);
    }

    if (newElement->textColor.a == 0 && newElement->textColor.r == 0 && newElement->textColor.g == 0 && newElement->textColor.b == 0) {
        newElement->textColor = nvgRGBA(255, 255, 255, 255);
    }

    if (newElement->hoverColor.a == 0 && newElement->hoverColor.r == 0 && newElement->hoverColor.g == 0 && newElement->hoverColor.b == 0) {
        newElement->hoverColor = getOppositeColor(newElement->color);
    }

    switch (newElement->type) {
        case ELEMENT_TEXTBOX:
            if (element.text != NULL) {
                strncpy(newElement->text, element.text, sizeof(newElement->text) - 1);
                newElement->text[sizeof(newElement->text) - 1] = '\0';
            } else {
                strncpy(newElement->text, "Text Box", sizeof(newElement->text) - 1);
                newElement->text[sizeof(newElement->text) - 1] = '\0';
            }
            break;
        case ELEMENT_RECT:
            newElement->text[0] = '\0';
            break;
        case ELEMENT_BUTTON:
            if (element.text != NULL) {
                strncpy(newElement->text, element.text, sizeof(newElement->text) - 1);
                newElement->text[sizeof(newElement->text) - 1] = '\0';
            } else {
                strncpy(newElement->text, "New Button", sizeof(newElement->text) - 1);
                newElement->text[sizeof(newElement->text) - 1] = '\0';
            }
            break;
        default:
            fprintf(stderr, "[Element Creation ERROR] Unsupported element type: %d\n", element.type);
            free(newElement);
            return NULL;
    }

    ListAdd(menu->elements, newElement);
    return newElement;
}

void DrawElement(Element *element, void (*update)(void)) {
    if (!menu->shown || element->created == GLFW_FALSE) return;

    if (update != NULL) update();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float posX = element->transform.position.x;
    float posY = element->transform.position.y;
    float padding = 10.0f;

    switch (element->type) {
        case ELEMENT_TEXTBOX:
            nvgBeginFrame(game->vgContext, game->windowWidth, game->windowHeight, game->aspectRatio);

            nvgFontSize(game->vgContext, element->textScale);
            nvgFontFaceId(game->vgContext, game->defaultFont);
            nvgTextAlign(game->vgContext, element->alignment);
            nvgFillColor(game->vgContext, element->color);

            // float textPosX = (element->alignment & NVG_ALIGN_RIGHT) ? game->windowWidth - padding : padding;
            // float textPosY = (element->alignment & NVG_ALIGN_TOP) ? padding : game->windowHeight - padding;
            float bounds[4];
            nvgTextBounds(game->vgContext, posX, posY, element->text, NULL, bounds);

            float textWidth = bounds[2] - bounds[0];
            float textHeight = bounds[3] - bounds[1];

            float textPosX = posX, textPosY = posY;

            // DEFAULT TOP
            // ELSE RIGHT TOP
            // ELSE CENTER TOP
            // ELSE ALIGN TOP
            // RIGHT TOP
            // ELSE CENTER TOP
            // ALIGN TOP

            // if (element->alignment == 0) {
                // printf("(DEFAULT) Element '%s''s Alignment value: %d\n", element->text, element->alignment);
                // textPosX = 0.0f;
            // }

            if (element->alignment & NVG_ALIGN_RIGHT) {
                textPosX = game->windowWidth - padding;
            }

            if (element->alignment & NVG_ALIGN_CENTER) {
                textPosX = game->windowWidth / 2;
            } else {
                // printf("ELSE CENTER TOP\n");
                // textPosX = game->windowWidth - padding;
            }

            if (element->alignment & NVG_ALIGN_TOP) {
                textPosY = padding;
            } else {
                // printf("ELSE ALIGN TOP\n");
                // textPosY = game->windowHeight - padding;
            }

            if (element->alignment & NVG_ALIGN_MIDDLE) {
                // printf("MIDDLE TOP\n");
                textPosY = game->windowHeight / 2;
            }

            nvgText(game->vgContext, textPosX, textPosY, element->text, NULL);
            nvgEndFrame(game->vgContext);
            break;
        case ELEMENT_RECT:
        case ELEMENT_BUTTON:
            const float textPadding = 40.0f;

            // actual rectangle
            nvgBeginFrame(game->vgContext, game->windowWidth, game->windowHeight, game->aspectRatio);

            if (element->type == ELEMENT_BUTTON) {
                nvgFillColor(game->vgContext, (element->isHovered) ? element->hoverColor : element->color);
            } else {
                nvgFillColor(game->vgContext, element->color);
            }

            float rectPosX = 0.0f, rectPosY = 0.0f;

            if (element->alignment & NVG_ALIGN_RIGHT) {
                rectPosX = game->windowWidth - (padding + element->width);
            } else {
                rectPosX = padding;
            }

            if (element->alignment & NVG_ALIGN_CENTER) {
                rectPosX = game->windowWidth / 2 - (element->width / 2);
            }

            if (element->alignment & NVG_ALIGN_TOP) {
                rectPosY = padding;
            } else {
                rectPosY = game->windowHeight - (padding + element->height);
            }

            if (element->alignment & NVG_ALIGN_MIDDLE) {
                rectPosY = game->windowHeight / 2 - (element->height / 2);
            }

            element->transform.position = (vec3s){rectPosX, rectPosY, 0.0f};

            nvgBeginPath(game->vgContext);
            nvgRect(game->vgContext, posX, posY, element->width, element->height);
            nvgFill(game->vgContext);
            nvgClosePath(game->vgContext);

            nvgEndFrame(game->vgContext);

            // draw text inside the rectangle
            if (element->text != NULL) {
                nvgBeginFrame(game->vgContext, game->windowWidth, game->windowHeight, game->aspectRatio);

                float bounds[4];
                nvgTextBounds(game->vgContext, posX, posY, element->text, NULL, bounds);

                float textWidth = bounds[2] - bounds[0];
                float textHeight = bounds[3] - bounds[1];

                /* TODO: THIS BULLSHIT MAKE SURE TEXT IS CENTERED INSIDE THE BUTTON,
                         OR TO THE LEFT WHATEVER U GET THE POINT NIGGA */
                // printf("TextWidth = %.2f && TextHeight = %.2f\n", textWidth, textHeight);

                nvgFontSize(game->vgContext, element->textScale);
                nvgFontFaceId(game->vgContext, game->defaultFont);
                nvgFillColor(game->vgContext, element->textColor);

                nvgText(game->vgContext, posX + textPadding, posY + element->textScale, element->text, NULL);

                nvgEndFrame(game->vgContext);
            }
            break;
        default:
            fprintf(stderr, "[Element Draw ERROR] Unsupported element type: %d\n", element->type);
            return;
    }
}