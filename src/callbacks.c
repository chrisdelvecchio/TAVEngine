#include "callbacks.h"

#include <stdio.h>

#include "camera.h"
#include "model3d.h"
#include "render.h"
#include "shader.h"
#include "ui.h"
#include "utils.h"

static vec2s cursor, previousCursor;

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
    float xposF = (float)xpos;
    float yposF = (float)ypos;

    // printf("Cursor at (%f, %f)\n", xpos, ypos);
    cursor = (vec2s){xposF, yposF};

    bool isDragging = engine->mouseDragging;

    if (isDragging) {
        processMouse(camera, xposF, yposF);
    }

    vec3s delta = CalculateDelta(cursor, previousCursor, engine->selectedAxis);

    foreach (Element *element, menu->elements) {
        if (element == NULL || element->type == ELEMENT_TEXTBOX) continue;

        element->hoverColor = (NVGcolor)nvgSmoothHoverColor(nvgColorToV3S(element->color), element->clickable.isHovered);
        element->clickable.isHovered = isPointInsideElement(element, cursor);
    }

    foreach (Model3D *model, engine->models) {
        if (!ModelExists(model)) continue;

        if (isDragging && IsAxisSelectionActive()) {
            TransformGizmoUpdateObject(NULL, model, delta, cursor);
        }

        model->hoverColor = (vec3s)SmoothHoverColor(model->color, model->clickable.isHovered);
        model->clickable.isHovered = isPointInside3DObj(NULL, model, cursor);
    }

    foreach (SceneObject *object, engine->sceneObjects) {
        if (!ObjectExists(object)) continue;

        if (isDragging && IsAxisSelectionActive()) {
            TransformGizmoUpdateObject(object, NULL, delta, cursor);
        }

        object->hoverColor = (vec3s)SmoothHoverColor(object->color, object->clickable.isHovered);
        object->clickable.isHovered = isPointInside3DObj(object, NULL, cursor);
    }

    if (engine->mouseDragging) {
        previousCursor = cursor;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            engine->mouseDragging = GLFW_TRUE;
            engine->firstMouse = GLFW_TRUE;  // Reset first mouse flag

            previousCursor = cursor;  // Initialize previousCursor
        } else if (action == GLFW_RELEASE) {
            engine->mouseDragging = GLFW_FALSE;
        }
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        foreach (Element *element, menu->elements) {
            if (element == NULL || element->type != ELEMENT_BUTTON) continue;

            if (element->clickable.isHovered && element->clickable.onClick) {
                element->clickable.onClick(element);
            }
        }

        foreach (Model3D *model, engine->models) {
            if (!ModelExists(model)) continue;

            if (model->clickable.isHovered && model->clickable.onClick) {
                model->clickable.onClick(model);
            }
        }

        foreach (SceneObject *object, engine->sceneObjects) {
            if (!ObjectExists(object)) continue;

            if (object->clickable.isHovered && object->clickable.onClick) {
                object->clickable.onClick(object);
            }
        }
    }
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    // printf("Scrolled %f units horizontally and %f units vertically\n", xoffset, yoffset);
    static float initalFov = 0.0f;

    if (initalFov == 0.0f) {
        initalFov = camera->fov;
    }

    camera->fov -= (float)yoffset;
    if (camera->fov < 1.0f)
        camera->fov = 1.0f;
    if (camera->fov > initalFov)
        camera->fov = initalFov;
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    printf("Framebuffer callback => Resized to %dx%d\n", width, height);
    glViewport(0, 0, width, height);

    engine->windowWidth = (float)width;
    engine->windowHeight = (float)height;
    engine->aspectRatio = (float)width / (float)height;

    if (engine->antiAliasing) {
        UnbindFrameBufferObj(antiAlias);  // Clean up the old framebuffer

        antiAlias = BindFrameBuffer((FrameBufferObject){
            .bufferWidth = width,
            .bufferHeight = height});  // Recreate framebuffer with new dimensions
    }
}

void window_close_callback(GLFWwindow *window) {
    printf("Window closed\n");
}