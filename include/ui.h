#pragma once

#ifndef UI_H
#define UI_H

#include <stdbool.h>

#include "engine.h"

typedef enum ElementType {
    ELEMENT_TEXTBOX = 0,
    ELEMENT_RECT,
    ELEMENT_BUTTON
} ElementType;

typedef struct Element {
    ElementType type;
    Transform transform;
    
    void (*onClick)(struct Element *element);

    NVGcolor color;
    NVGcolor textColor;
    NVGcolor hoverColor;

    bool isHovered;
    bool created;
    char text[256];
    float width, height, textScale;
    int alignment;
} Element;

typedef struct Menu {
    bool shown;
    List *elements;  // List <Element *>
} Menu;

extern Menu *menu;

void initUI(void);
void destroyUI(void);
void toggleMenu(void);

Element *CreateElement(Element element);
void DestroyElement(Element *element);

void DrawElement(Element *element, void (*update)(void));

static char *elementTypetoString(ElementType type) {
    switch (type) {
        case ELEMENT_TEXTBOX:
            return "TextBox";
        case ELEMENT_RECT:
            return "Rectangle";
        case ELEMENT_BUTTON:
            return "Button";
        default:
            return "Unknown";
    }
}

#endif  // UI_H