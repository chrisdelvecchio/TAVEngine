#include <stdio.h>
#include "game.h"

static void debug(void) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        fprintf(stderr, "[FATAL ERROR] => OpenGL error: %d\n", err);
    }
}

static void loop(void) {
    debug();
    
    tick();
    render();
}

int main(void) {
    game = init();

    while (!glfwWindowShouldClose(game->window)) {
        loop();
    }

    return cleanup();
}