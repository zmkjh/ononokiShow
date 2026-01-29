// (c) zmkjh, YouthfulCase 2026 - --

#ifndef ONONOKI_SHOW_MAIN_C
#define ONONOKI_SHOW_MAIN_C

#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_log.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <stdlib.h>

#include "config.h"

static inline void LOG(char* category, char* tip) {
    FILE* logf = fopen(LOG_ADDRESS, "a");
    fprintf(logf, "%s: %s\n", category, tip);
    fclose(logf);
}

struct {
    SDL_Texture*    texture[IMAGE_COUNT];
    SDL_Rect        texture_rect[IMAGE_COUNT];

    int             quit_flag;

    SDL_Window*     window;
    SDL_Renderer*   renderer;
    int             image_index;
} global;

void global_quit() {
    global.quit_flag = 1;
}

void global_ctor() {
    global.quit_flag = 0;
    global.image_index = 0;

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        LOG("ERROR", "failed to init img api!");
        SDL_Quit();
        exit(0);
    }

    if (SDL_Init(SDL_INIT_EVENTS) != 0) {
        LOG("ERROR", "failed to init event api!");
        SDL_Quit();
        exit(0);
    }

    global.window = SDL_CreateWindow(
        TITLE, WINDOW_POS_X, WINDOW_POS_Y,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_INPUT_FOCUS);

    SDL_SetWindowOpacity(global.window, OPACITY);

    if (global.window == NULL) {
        LOG("ERROR", "SDL_CreateWindow Failed");
        SDL_Quit();
        exit(0);
    }

    global.renderer = SDL_CreateRenderer(
        global.window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (global.renderer == NULL) {
        SDL_DestroyWindow(global.window);
        LOG("ERROR", "SDL_CreateRenderer Failed!");
        SDL_Quit();
        exit(0);
    }

    // pre-load images
#define IMAGE_PATH_BUFF_SIZE 50
    char buffer[IMAGE_PATH_BUFF_SIZE];
    for (int i = 0; i < IMAGE_COUNT; i++) {
        sprintf(buffer, "Assets/%d.png", i);
        SDL_Surface* loaded_surface = IMG_Load((const char*)buffer);
        if (!loaded_surface) {
            LOG("ERROR", "fail to load image.");
            SDL_DestroyWindow(global.window);
            SDL_DestroyRenderer(global.renderer);
            SDL_Quit();
            exit(0);
        }
        global.texture[i] = SDL_CreateTextureFromSurface(global.renderer, loaded_surface);
        global.texture_rect[i] = (SDL_Rect){.w = loaded_surface->w, .h = loaded_surface->h};
        SDL_FreeSurface(loaded_surface);
    }
}

void global_dtor() {
    SDL_DestroyRenderer(global.renderer);
    SDL_DestroyWindow(global.window);
    SDL_Quit();
}

int shoud_run() {
    return !global.quit_flag;
}

void render() {
    SDL_RenderClear(global.renderer);
    if (RESIZABLE)
        SDL_SetWindowSize(global.window, global.texture_rect[global.image_index].w, global.texture_rect[global.image_index].h);

    SDL_Rect dst_rect = {0,0,0,0};
    SDL_GetWindowSize(global.window, &dst_rect.w, &dst_rect.h);

    SDL_RenderCopy(
        global.renderer, global.texture[global.image_index],
        &global.texture_rect[global.image_index],
        &dst_rect);
    SDL_RenderPresent(global.renderer);
}

void process_keycode(SDL_Scancode keycode) {
    switch (keycode) {
    case KEY_SWITCH:
        global.image_index = (global.image_index+1)%IMAGE_COUNT;
        break;
    case KEY_QUIT:
        global_quit();
        break;
    default:
        break;
    }
}

void event() {
    SDL_Event event;
    SDL_WaitEvent(&event);

    switch (event.type) {
    case SDL_QUIT:
        global_quit();
        break;
    case SDL_KEYDOWN:
        process_keycode(event.key.keysym.scancode);
        break;
    }
}

int main(int argc, char *argv[]) {
    // init
    global_ctor();

    // mainloop
    while (shoud_run()) {
        event();
        render();

        // release cpu clip.
        SDL_Delay(FRAME_SLEEP);
    }

    // quit
    global_dtor();
    return 0;
}

#endif
