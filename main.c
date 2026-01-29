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
#include <stdio.h>
#include <stdlib.h>

#include "cJSON/cJSON.h"

static inline void LOG(char* category, char* tip) {
    FILE* logf = fopen("ononokiShow.log", "a");
    fprintf(logf, "%s: %s\n", category, tip);
    fclose(logf);
}

struct {
    cJSON* json;

    int window_height;
    int window_width;
    int window_position_x;
    int window_position_y;
    int window_resizable;

    char* image_path;

    int image_count;

    float opacity;
} config;

void config_ctor() {
    char buff[2048];

    // read config file
    FILE* config_file = fopen("config.json", "r");
    fseek(config_file, 0, SEEK_END);
    int file_size = ftell(config_file);

    rewind(config_file);
    fread(buff, 1, file_size, config_file);
    buff[file_size] = '\0';

    fclose(config_file);

    config.json = cJSON_Parse((const char*)buff);
    config.window_height = cJSON_GetObjectItem(config.json, "window_height")->valueint;
    config.window_width = cJSON_GetObjectItem(config.json, "window_width")->valueint;
    config.window_position_x = cJSON_GetObjectItem(config.json, "window_position_x")->valueint;
    config.window_position_y = cJSON_GetObjectItem(config.json, "window_position_y")->valueint;
    config.window_resizable = cJSON_GetObjectItem(config.json, "window_resizable")->valueint;
    config.image_path = cJSON_GetObjectItem(config.json, "image_path")->valuestring;
    config.image_count = cJSON_GetObjectItem(config.json, "image_count")->valueint;
    config.opacity = cJSON_GetObjectItem(config.json, "opacity")->valuedouble;
}

void config_dtor() {
    cJSON_Delete(config.json);
}

struct {
    SDL_Texture**    texture;
    SDL_Rect*        texture_rect;

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
        "~", config.window_position_x, config.window_position_y,
        config.window_width, config.window_height,
        SDL_WINDOW_BORDERLESS | SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_INPUT_FOCUS);

    SDL_SetWindowOpacity(global.window, config.opacity);

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
    global.texture      = (SDL_Texture**)malloc(sizeof(SDL_Texture*) * config.image_count);
    global.texture_rect = (SDL_Rect*)malloc(sizeof(SDL_Rect) * config.image_count);
    char buffer[80];
    for (int i = 0; i < config.image_count; i++) {
        sprintf(buffer, "%s/%d.png", config.image_path, i);
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
    free(global.texture);
    free(global.texture_rect);

    SDL_DestroyRenderer(global.renderer);
    SDL_DestroyWindow(global.window);
    SDL_Quit();
}

int shoud_run() {
    return !global.quit_flag;
}

void render() {
    SDL_RenderClear(global.renderer);
    if (config.window_resizable)
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
    case SDL_SCANCODE_DELETE:
        global.image_index = (global.image_index+1) % config.image_count;
        break;
    case SDL_SCANCODE_ESCAPE:
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
    config_ctor();
    global_ctor();

    // mainloop
    while (shoud_run()) {
        event();
        render();

        // release cpu clip.
        SDL_Delay(60);
    }

    // quit
    global_dtor();
    config_dtor();
    return 0;
}

#endif
