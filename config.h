// (c) zmkjh, YouthfulCase 2026 - --

#ifndef ONONOKI_SHOW_CONFIG_H
#define ONONOKI_SHOW_CONFIG_H

// size of the show
#define WINDOW_HEIGHT   300
#define WINDOW_WIDTH    300
#define WINDOW_POS_X    0
#define WINDOW_POS_Y    0

// whether the show will resize with the image or not.
#define RESIZABLE       0

// image count
#define IMAGE_COUNT     5

// key to switch image
#define KEY_SWITCH SDL_SCANCODE_DELETE
// key to quit
#define KEY_QUIT SDL_SCANCODE_ESCAPE

// opacity of the show
#define OPACITY         0.9f

// path of debug log
#define LOG_ADDRESS     "debugLog.log"

// sleeping time each frame
#define FRAME_SLEEP     16

// useless because the show is borderless now
#define TITLE           "yo"

#endif
