#include "game.h"

#include <stdio.h>
#include <stdlib.h>

#define NANOVG_GL3_IMPLEMENTATION
#include "callbacks.h"
#include "camera.h"
#include "nanovg_gl.h"
#include "object.h"
#include "render.h"
#include "shader.h"
#include "ui.h"
#include "uievents.h"
#include "utils.h"

Game *game = NULL;
Shader *defaultShader, *instanceShader, *antiAliasShader;
Camera *camera;
Element *button, *fpstextBox, *coordinatestextBox;
SceneObject *plane, *cube;
FrameBufferObject *antiAlias;

Game *init(void) {
    game = malloc(sizeof(Game));
    if (!game) {
        printf("[Game] => Failed to allocate memory for game\n");
        return NULL;
    }

    printf("[Game] => Loading game...\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;

    if (!glfwInit()) {
        printf("[Game] => Failed to initialize GLFW\n");
        return NULL;
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Prototype Game", NULL, NULL);
    if (!window) {
        printf("[Game] => Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    game->windowWidth = (float)SCREEN_WIDTH;
    game->windowHeight = (float)SCREEN_HEIGHT;
    game->aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    game->firstMouse = GLFW_TRUE;
    game->lastX = SCREEN_WIDTH / 2.0f;
    game->lastY = SCREEN_HEIGHT / 2.0f;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "[GLAD] Failed to initialize OpenGL context\n");
        return NULL;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    char *mainPath = "C:/Users/Chris/Desktop/Chris/Coding/Game";
    char *settingsFile = CreatePath(mainPath, "build/settings.txt");
    char *shadersDir = CreatePath(mainPath, ReadValue(settingsFile, "shaderDir"));
    char *assetsDir = CreatePath(mainPath, ReadValue(settingsFile, "assetDir"));
    char *fontsDir = CreatePath(mainPath, ReadValue(settingsFile, "fontDir"));

    NVGcontext *vg = (NVGcontext *)nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);

    if (vg == NULL) {
        printf("[NanoVG] => Could not init NanoVG.\n");
        return NULL;
    }

    char *defaultFontFile = CreatePath(fontsDir, ReadValue(settingsFile, "defaultFont"));
    int font = nvgCreateFont(vg, "Roboto", defaultFontFile);
    if (font == -1) {
        printf("Could not load the default font.\n");
        return NULL;
    }

    game->window = (GLFWwindow *)window;
    game->vgContext = (NVGcontext *)vg;
    game->defaultFont = (int)font;
    game->mainPath = (char *)mainPath;
    game->settingsFile = (char *)settingsFile;
    game->shaderDir = (char *)shadersDir;
    game->assetDir = (char *)assetsDir;
    game->fontDir = (char *)fontsDir;
    game->fps = (float)0.0f;
    game->deltaTime = (float)0.0f;
    game->shaders = (List *)NewList(NULL);
    game->sceneObjects = (List *)NewList(NULL);
    game->antiAliasing = GLFW_TRUE;
    game->vSync = GLFW_TRUE;
    game->wireframeMode = GLFW_FALSE;

    glEnable(GL_DEBUG_OUTPUT);

    initUI();
    init_callbacks(game);
    InitTimerManager();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    defaultShader = (Shader *)NewShader("shader.vert", "shader.frag");
    instanceShader = (Shader *)NewShader("instance_shader.vert", "shader.frag");
    camera = (Camera *)NewCamera((vec3s){1.0f, 1.0f, 1.0f}, 45.0f);

    if (game->antiAliasing) {
        antiAliasShader = (Shader *)NewShader("aa_post.vert", "aa_post.frag");
        antiAlias = (FrameBufferObject *)BindFrameBuffer((FrameBufferObject){
            .bufferWidth = SCREEN_WIDTH,
            .bufferHeight = SCREEN_HEIGHT});
    }

    button = (Element *)CreateElement((Element){
        .type = ELEMENT_BUTTON,
        .text = "Play!",
        .width = 200.0f,
        .height = 30.0f,
        .color = nvgRGBA(169, 169, 169, 255),
        .textColor = nvgRGBA(100, 0, 0, 255),
        .transform = (Transform){0},
        .onClick = onClickPlayButton});

    fpstextBox = (Element *)CreateElement((Element){
        .type = ELEMENT_TEXTBOX,
        .color = nvgRGBA(255, 255, 0, 255),
        .transform = (Transform){
            .position = (vec3s){10.0f, game->windowHeight - 50.0f, 0.0f}}});

    coordinatestextBox = (Element *)CreateElement((Element){
        .type = ELEMENT_TEXTBOX,
        .alignment = NVG_ALIGN_TOP | NVG_ALIGN_RIGHT});

    plane = (SceneObject *)CreatePlane((vec3s){0.0f, 0.0f, 0.0f});
    cube = (SceneObject *)CreateCube((vec3s){10.0f, -10.0f, 10.0f});
    cube->transforms->scale = (vec3s){7.0f, 7.0f, 7.0f};
    return game;
}

static void RemoveSceneObjects(void) {
    int counter = 0;
    foreach (SceneObject *object, game->sceneObjects) {
        RemoveSceneObject(object);
        counter++;
    }

    if (!isListEmpty(game->sceneObjects)) {
        ListClear(game->sceneObjects);
    }

    UnbindFrameBufferObj(antiAlias);

    printf("[Game] %d Scene Objects have been freed!\n", counter);
}

int cleanup(void) {
    printf("\n");
    nvgDeleteGL3(game->vgContext);

    destroyUI();
    freeShaders();
    RemoveSceneObjects();

    ListFreeMemory(game->sceneObjects);
    ListFreeMemory(game->shaders);

    ListFreeMemory(menu->elements);
    free(menu);

    if (timerManager->totalTimers > 0) {
        for (int i = 0; i < timerManager->totalTimers; i++) {
            free(timerManager->timers[i]);
        }
    }

    free(timerManager->timers);
    free(timerManager);

    glfwDestroyWindow(game->window);
    glfwTerminate();

    free(game);
    printf("[EXIT] Cleaned up successfully.");
    return EXIT_SUCCESS;
}

void update(void) {
    static int frameCount = 0;
    static double lastTime = 0.0;
    double currentTime = current_time();

    if (lastTime == 0.0) {
        lastTime = currentTime;
    }

    frameCount++;
    double difference = currentTime - lastTime;

    if (difference >= 1.0f) {
        game->fps = frameCount;

        frameCount = 0;
        lastTime = currentTime;
    }
}

void render(void) {
    if (!game->vSync) {
        glfwSwapInterval(0);
    }

    glClearColor(BACKGROUND_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (game->antiAliasing) {
        glBindFramebuffer(GL_FRAMEBUFFER, antiAlias->frameBufferID);
        glClearColor(BACKGROUND_COLOR);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

    if (game->wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    foreach (SceneObject *object, game->sceneObjects) {
        if (object->type == OBJECT_FRAMEBUFFER_QUAD) continue;
        object->draw(object);
    }

    if (menu != NULL) {
        DrawElement(button, NULL);

        DrawElement(fpstextBox, lambda(void, (void), {
                        sprintf(fpstextBox->text, "%.1f", game->fps);
                    }));

        DrawElement(coordinatestextBox, lambda(void, (void), {
                        sprintf(coordinatestextBox->text, "X: %.2f, Y: %.2f, Z: %.2f", camera->position.x, camera->position.y, camera->position.z);
                    }));
    }

    if (game->antiAliasing) {
        antiAlias->drawBuffer(antiAlias);
    }

    glfwSwapBuffers(game->window);
    glfwPollEvents();
}