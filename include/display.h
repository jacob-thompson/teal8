#include <stdbool.h>

#include <SDL.h>

#define KEY_COUNT 16
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

/**
    * The display struct.
    * Contains the SDL window, renderer, and the pixels to be drawn.
*/
typedef struct display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool powered_on;
    bool key_pressed[KEY_COUNT];
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
