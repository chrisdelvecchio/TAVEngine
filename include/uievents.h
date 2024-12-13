#pragma once

#ifndef UIEVENTS_H
#define UIEVENTS_H

#include <stdio.h>
#include "engine.h"
#include "ui.h"

static void onClickPlayButton(Element *element) {
    printf("Play button clicked!\n");
    RemoveElement(element);
}

#endif // UIEVENTS_H