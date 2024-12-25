#include <stdbool.h>

#include <SDL.h>

#define KEY_COUNT 16

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define SCALE 10
#define CHIP8_WIDTH 64
#define CHIP8_HEIGHT 32
#define SCHIP_WIDTH CHIP8_WIDTH << 1
#define SCHIP_HEIGHT CHIP8_HEIGHT << 1

/**
    * The display struct.
    * Contains the SDL window, renderer, and the pixels to be drawn.
*/
typedef struct display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect *pixels;
    bool *pixelDrawn;
    bool poweredOn;
    bool keyDown[KEY_COUNT];
    bool keyUp[KEY_COUNT];
    uint32_t lastUpdate;
} display;

/**
    * Clear the pixels of the display.
    * @param display the display
*/
void resetDisplay(display *display);

/**
    * Initialize the display.
    * @param display the display
    * @return 0 on success, 1 on failure
*/
int initDisplay(display *display);

/**
    * Handle an event.
    * @param display the display
    * @param event the event
*/
void handleEvent(display *display, SDL_Event *event);

/**
    * Draw the background of the display.
    * @param display the display
    * @return 0 on success, 1 on failure
*/
int drawBackground(display *display);

/**
    * Draw the pixels of the display.
    * @param display the display
    * @return 0 on success, 1 on failure
*/
int drawPixels(display *display);

/**
    * Clear keypress events.
    * @param keys the keys to be cleared
*/
void clearKeys(bool *keys);
