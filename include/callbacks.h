#pragma once

#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "game.h"

void init_callbacks(Game *game);

void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void window_close_callback(GLFWwindow *window);

#endif  // CALLBACKS_H