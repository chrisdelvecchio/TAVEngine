#pragma once

#ifndef UIEVENTS_H
#define UIEVENTS_H

#include <stdio.h>
#include "game.h"
#include "ui.h"

static void onClickPlayButton(void) {
    printf("Play button clicked!\n");
    destroyUI();
}

#endif // UIEVENTS_H