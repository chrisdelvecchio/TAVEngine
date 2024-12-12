#include "callbacks.h"

#include <stdio.h>

#include "camera.h"
#include "shader.h"
#include "ui.h"
#include "utils.h"
#include "render.h"

void init_callbacks(Engine *engine) {
    glfwSetKeyCallback(engine->window, key_callback);
    glfwSetMouseButtonCallback(engine->window, mouse_button_callback);
    glfwSetScrollCallback(engine->window, scroll_callback);
    glfwSetFramebufferSizeCallback(engine->window, framebuffer_size_callback);
    glfwSetCursorPosCallback(engine->window, cursor_position_callback);
    glfwSetWindowCloseCallback(engine->window, window_close_callback);

    glDebugMessageCallback(debugCallback, NULL);

    printf("[TAV ENGINE] Initalized GLFW events...\n");
}

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
    fprintf(stderr, "[DEBUG CALLBACK] => OpenGL Debug: %s\n", message);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // printf("Key pressed\n");

    processKeyboard(camera);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_RELEASE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    } else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
        reloadShaders();
    } else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_RELEASE) {
        int mode = glfwGetInputMode(engine->window, GLFW_CURSOR);

        glfwSetInputMode(engine->window, GLFW_CURSOR, (mode == GLFW_CURSOR_DISABLED) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    } else if (key == GLFW_KEY_W && action == GLFW_RELEASE && (mods & GLFW_MOD_CONTROL)) {
        engine->wireframeMode = !engine->wireframeMode;
    }
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
    // printf("Cursor at (%f, %f)\n", xpos, ypos);

    processMouse(camera, xpos, ypos);

    foreach (Element *element, menu->elements) {
        if (element == NULL || element->type == ELEMENT_TEXTBOX) continue;
        element->isHovered = isPointInsideElement(element, (vec2s){xpos, ypos});
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        // press
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        // release
        foreach (Element *element, menu->elements) {
            if (element == NULL || element->type != ELEMENT_BUTTON) continue;

            if (element->isHovered && element->onClick) {
                element->onClick(element);
            }
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    // printf("Scrolled %f units horizontally and %f units vertically\n", xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    printf("Framebuffer callback => Resized to %dx%d\n", width, height);
    glViewport(0, 0, width, height);

    engine->windowWidth = (float)width;
    engine->windowHeight = (float)height;
    engine->aspectRatio = (float)width / (float)height;

    if (engine->antiAliasing) {
        UnbindFrameBufferObj(antiAlias); // Clean up the old framebuffer
        antiAlias = BindFrameBuffer((FrameBufferObject){
            .bufferWidth = width,
            .bufferHeight = height
        });  // Recreate framebuffer with new dimensions
    }
}

void window_close_callback(GLFWwindow *window) {
    printf("Window closed\n");
}