#include "ui.h"

#include <string.h>

#include "utils.h"

Menu *menu;

void initUI(void) {
    printf("[TAV ENGINE] Initializing UI...\n");
    if (menu == NULL) {
        menu = (Menu *)malloc(sizeof(Menu));
    }

    menu->shown = GLFW_TRUE;
    menu->elements = NewList(NULL);
}

void destroyUI(void) {
    menu->shown = GLFW_FALSE;

    int counter = 0;
    foreach (Element *element, menu->elements) {
        if (!ElementExists(element)) continue;
        FreeupElement(element);
        counter++;
    }

    if (!isListEmpty(menu->elements)) {
        ListClear(menu->elements);
    }

    printf("[TAV ENGINE] %d UI Elements have been freed!\n", counter);
}

void RemoveElement(Element *element) {
    if (!ElementExists(element)) return;

    if (ListContains(menu->elements, element)) {
        ListRemove(menu->elements, element);
    }

    FreeupElement(element);
    printf("[UI ELEMENT] Destroyed successfully!\n");
}

void toggleMenu(void) {
    menu->shown = !menu->shown;
    printf("[UI] Toggled Menu\n");
}

Element *NewUIElement(Element element) {
    Element *newElement = (Element *)malloc(sizeof(Element));
    if (newElement == NULL) {
        fprintf(stderr,
                "[MEMORY ERROR] Failed to allocate memory for "
                "NewUIElement();\n");
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

    if (!newElement->clickable.onClick && newElement->clickable.hoverColor.x == 0 && newElement->clickable.hoverColor.y == 0 && newElement->clickable.hoverColor.z == 0) {
        newElement->hoverColor = (NVGcolor)nvgSmoothHoverColor(nvgColorToV3S(newElement->color), GLFW_FALSE);

        newElement->clickable = (Clickable){
            .onClick = NULL,
            .hoverColor = nvgColorToV3S(newElement->hoverColor)
        };
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
    if (!menu->shown || !ElementExists(element)) return;

    if (update != NULL) update();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    float posX = element->transform.position.x;
    float posY = element->transform.position.y;
    float padding = 10.0f;

    switch (element->type) {
        case ELEMENT_TEXTBOX:
            nvgBeginFrame(engine->vgContext, engine->windowWidth, engine->windowHeight, engine->aspectRatio);

            nvgFontSize(engine->vgContext, element->textScale);
            nvgFontFaceId(engine->vgContext, engine->defaultFont);
            nvgTextAlign(engine->vgContext, element->alignment);
            nvgFillColor(engine->vgContext, element->color);

            // float textPosX = (element->alignment & NVG_ALIGN_RIGHT) ? engine->windowWidth - padding : padding;
            // float textPosY = (element->alignment & NVG_ALIGN_TOP) ? padding : engine->windowHeight - padding;
            float bounds[4];
            nvgTextBounds(engine->vgContext, posX, posY, element->text, NULL, bounds);

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
                textPosX = engine->windowWidth - padding;
            }

            if (element->alignment & NVG_ALIGN_CENTER) {
                textPosX = engine->windowWidth / 2;
            } else {
                // printf("ELSE CENTER TOP\n");
                // textPosX = engine->windowWidth - padding;
            }

            if (element->alignment & NVG_ALIGN_TOP) {
                textPosY = padding;
            } else {
                // printf("ELSE ALIGN TOP\n");
                // textPosY = engine->windowHeight - padding;
            }

            if (element->alignment & NVG_ALIGN_MIDDLE) {
                // printf("MIDDLE TOP\n");
                textPosY = engine->windowHeight / 2;
            }

            nvgText(engine->vgContext, textPosX, textPosY, element->text, NULL);
            nvgEndFrame(engine->vgContext);
            break;
        case ELEMENT_RECT:
        case ELEMENT_BUTTON:
            const float textPadding = 40.0f;

            // actual rectangle
            nvgBeginFrame(engine->vgContext, engine->windowWidth, engine->windowHeight, engine->aspectRatio);

            if (element->type == ELEMENT_BUTTON) {
                nvgFillColor(engine->vgContext, (element->clickable.isHovered) ? element->hoverColor : element->color);
            } else {
                nvgFillColor(engine->vgContext, element->color);
            }

            float rectPosX = 0.0f, rectPosY = 0.0f;

            if (element->alignment & NVG_ALIGN_RIGHT) {
                rectPosX = engine->windowWidth - (padding + element->width);
            } else {
                rectPosX = padding;
            }

            if (element->alignment & NVG_ALIGN_CENTER) {
                rectPosX = engine->windowWidth / 2 - (element->width / 2);
            }

            if (element->alignment & NVG_ALIGN_TOP) {
                rectPosY = padding;
            } else {
                rectPosY = engine->windowHeight - (padding + element->height);
            }

            if (element->alignment & NVG_ALIGN_MIDDLE) {
                rectPosY = engine->windowHeight / 2 - (element->height / 2);
            }

            element->transform.position = (vec3s){rectPosX, rectPosY, 0.0f};

            nvgBeginPath(engine->vgContext);
            nvgRect(engine->vgContext, posX, posY, element->width, element->height);
            nvgFill(engine->vgContext);
            nvgClosePath(engine->vgContext);

            nvgEndFrame(engine->vgContext);

            // draw text inside the rectangle
            if (element->text != NULL) {
                nvgBeginFrame(engine->vgContext, engine->windowWidth, engine->windowHeight, engine->aspectRatio);

                float bounds[4];
                nvgTextBounds(engine->vgContext, posX, posY, element->text, NULL, bounds);

                float textWidth = bounds[2] - bounds[0];
                float textHeight = bounds[3] - bounds[1];

                /* TODO: THIS BULLSHIT MAKE SURE TEXT IS CENTERED INSIDE THE BUTTON,
                         OR TO THE LEFT WHATEVER U GET THE POINT NIGGA */
                // printf("TextWidth = %.2f && TextHeight = %.2f\n", textWidth, textHeight);

                nvgFontSize(engine->vgContext, element->textScale);
                nvgFontFaceId(engine->vgContext, engine->defaultFont);
                nvgFillColor(engine->vgContext, element->textColor);

                nvgText(engine->vgContext, posX + textPadding, posY + element->textScale, element->text, NULL);

                nvgEndFrame(engine->vgContext);
            }
            break;
        default:
            fprintf(stderr, "[Element Draw ERROR] Unsupported element type: %d\n", element->type);
            return;
    }
}