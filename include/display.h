#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL_events.h>

#define KEY_COUNT 16 // 16 keys (0x0 to 0xF)

#define EXIT_SUCCESS 0 // for consistency with stdlib.h
#define EXIT_FAILURE 1 // for consistency with stdlib.h

#define SCALE 10 // scale factor for the display
#define CHIP8_WIDTH 64 // width of the CHIP8 display in pixels
#define CHIP8_HEIGHT 32 // height of the CHIP8 display in pixels
#define SCHIP_WIDTH 128 // width of the SCHIP display in pixels
#define SCHIP_HEIGHT 64 // height of the SCHIP display in pixels

typedef struct display {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect *pixels;
    bool *pixelDrawn;
    bool poweredOn;
    bool reset;
    bool keyDown[KEY_COUNT];
    bool keyUp[KEY_COUNT];
    uint32_t lastUpdate;
    int width;
    int height;
    int pixelWidth;  // cached width / SCALE
    int pixelHeight; // cached height / SCALE
    bool dirty;      // track if display needs redraw
} display;

/*
 * Clear the pixels of the display.
 * @param display the display
 */
void resetDisplay(display *display);

/*
 * Create the pixels of the display.
 * @param display the display
 */
void createPixels(display *display);

/*
 * Initialize the display.
 * @param display the display
 * @return 0 on success, 1 on failure
 */
int initDisplay(display *display);

/*
 * Handle an event.
 * @param display the display
 * @param event the event
 */
void handleEvent(display *display, SDL_Event *event);

/*
 * Draw the background of the display.
 * @param display the display
 * @return 0 on success, 1 on failure
 */
int drawBackground(display *display);

/*
 * Draw the pixels of the display.
 * @param display the display
 * @return 0 on success, 1 on failure
 */
int drawPixels(display *display);

/*
 * Clear keypress events.
 * @param keys the keys to be cleared
*/
void clearKeys(bool *keys);

#endif /* DISPLAY_H */
