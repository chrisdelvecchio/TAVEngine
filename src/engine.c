#include "engine.h"

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

Engine *engine = NULL;
Shader *defaultShader, *instanceShader, *antiAliasShader, *skyboxShader;
Camera *camera;
FrameBufferObject *antiAlias;

/* DEBUG STUFF FOR TESTING & TROUBLESHOOTING ENGINE */
static Element *button, *fpstextBox, *coordinatestextBox;
static SceneObject *plane, *cube;
static Camera *cam2;
// static Timer *timer;

Engine *init(void) {
    engine = malloc(sizeof(Engine));
    if (!engine) {
        printf("[TAV ENGINE] => Failed to allocate memory for Engine\n");
        return NULL;
    }

    printf("[TAV ENGINE] => Loading engine...\n");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window;

    if (!glfwInit()) {
        printf("[TAV ENGINE] => Failed to initialize GLFW\n");
        return NULL;
    }

    window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Prototype Engine", NULL, NULL);
    if (!window) {
        printf("[TAV ENGINE] => Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    engine->windowWidth = (float)SCREEN_WIDTH;
    engine->windowHeight = (float)SCREEN_HEIGHT;
    engine->aspectRatio = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;

    engine->firstMouse = GLFW_TRUE;
    engine->lastX = SCREEN_WIDTH / 2.0f;
    engine->lastY = SCREEN_HEIGHT / 2.0f;

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "[GLAD] Failed to initialize OpenGL context\n");
        return NULL;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    char *mainPath = getSourceCodePath();
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

    engine->window = (GLFWwindow *)window;
    engine->vgContext = (NVGcontext *)vg;
    engine->defaultFont = (int)font;
    engine->mainPath = (char *)mainPath;
    engine->settingsFile = (char *)settingsFile;
    engine->shaderDir = (char *)shadersDir;
    engine->assetDir = (char *)assetsDir;
    engine->fontDir = (char *)fontsDir;
    engine->fps = (float)0.0f;
    engine->deltaTime = (float)0.0f;
    engine->shaders = (List *)NewList(NULL);
    engine->sceneObjects = (List *)NewList(NULL);
    engine->cameras = (List *)NewList(NULL);
    engine->textures = (Map *)NewMap(NULL);
    engine->antiAliasing = GLFW_TRUE;
    engine->vSync = GLFW_TRUE;
    engine->wireframeMode = GLFW_FALSE;
    engine->skybox = (Skybox *)NULL;

    glEnable(GL_DEBUG_OUTPUT);

    initUI();
    init_callbacks(engine);
    InitTimerManager();

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    defaultShader = (Shader *)NewShader("shader.vert", "shader.frag");
    instanceShader = (Shader *)NewShader("instance_shader.vert", "shader.frag");
    skyboxShader = (Shader *)NewShader("skybox.vert", "skybox.frag");

    camera = (Camera *)NewCamera((vec3s){1.0f, 1.0f, 1.0f}, 45.0f);
    cam2 = (Camera *)NewCamera((vec3s){15.0f, -10.0f, 10.0f}, 45.0f);

    if (engine->antiAliasing) {
        antiAliasShader = (Shader *)NewShader("aa_post.vert", "aa_post.frag");
        antiAlias = (FrameBufferObject *)BindFrameBuffer((FrameBufferObject){
            .bufferWidth = SCREEN_WIDTH,
            .bufferHeight = SCREEN_HEIGHT});
    }

    button = (Element *)NewUIElement((Element){
        .type = ELEMENT_BUTTON,
        .text = "Play!",
        .width = 200.0f,
        .height = 30.0f,
        .color = nvgRGBA(169, 169, 169, 255),
        .textColor = nvgRGBA(100, 0, 0, 255),
        .transform = (Transform){0},
        .onClick = onClickPlayButton});

    fpstextBox = (Element *)NewUIElement((Element){
        .type = ELEMENT_TEXTBOX,
        .color = nvgRGBA(255, 255, 0, 255),
        .transform = (Transform){
            .position = (vec3s){10.0f, engine->windowHeight - 50.0f, 0.0f}}});

    coordinatestextBox = (Element *)NewUIElement((Element){
        .type = ELEMENT_TEXTBOX,
        .alignment = NVG_ALIGN_TOP | NVG_ALIGN_RIGHT});

    plane = (SceneObject *)CreatePlane((vec3s){0.0f, 0.0f, 0.0f});
    cube = (SceneObject *)CreateCube((vec3s){10.0f, -10.0f, 10.0f});
    cube->transforms->scale = (vec3s){7.0f, 7.0f, 7.0f};
    return engine;
}

int cleanup(void) {
    printf("\n");
    nvgDeleteGL3(engine->vgContext);

    destroyUI();
    freeShaders();

    RemoveTextures();
    RemoveCameras();
    RemoveSceneObjects();

    MapFreeMemory(engine->textures);
    ListFreeMemory(engine->cameras);
    ListFreeMemory(engine->sceneObjects);
    ListFreeMemory(engine->shaders);
    ListFreeMemory(menu->elements);

    free(menu);

    if (timerManager->totalTimers > 0) {
        for (int i = 0; i < timerManager->totalTimers; i++) {
            free(timerManager->timers[i]);
        }
    }

    free(timerManager->timers);
    free(timerManager);

    glfwDestroyWindow(engine->window);
    glfwTerminate();

    if (engine->skybox != NULL) {
        engine->skybox->free(engine->skybox);
    }

    free(engine);
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
        engine->fps = frameCount;

        frameCount = 0;
        lastTime = currentTime;
    }
}

void render(void) {
    if (!engine->vSync) {
        glfwSwapInterval(0);
    }

    glClearColor(BACKGROUND_COLOR);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (engine->antiAliasing) {
        glBindFramebuffer(GL_FRAMEBUFFER, antiAlias->frameBufferID);
        glClearColor(BACKGROUND_COLOR);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
    }

    if (engine->wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (engine->skybox != NULL) {
        engine->skybox->draw(engine->skybox);
    }

    foreach (SceneObject *object, engine->sceneObjects) {
        if (!ObjectExists(object)) continue;
        if (object->type == OBJECT_FRAMEBUFFER_QUAD) continue;

        object->draw(object);
    }

    if (menu != NULL) {
        DrawElement(button, NULL);

        DrawElement(fpstextBox, lambda(void, (void), {
                        sprintf(fpstextBox->text, "%.1f", engine->fps);
                    }));

        DrawElement(coordinatestextBox, lambda(void, (void), {
                        sprintf(coordinatestextBox->text, "X: %.2f, Y: %.2f, Z: %.2f", camera->position.x, camera->position.y, camera->position.z);
                    }));
    }

    if (engine->antiAliasing) {
        antiAlias->drawBuffer(antiAlias);
    }

    glfwSwapBuffers(engine->window);
    glfwPollEvents();
}