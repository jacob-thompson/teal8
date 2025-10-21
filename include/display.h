#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL_video.h>
#include <SDL_render.h>
#include <SDL_rect.h>
#include <SDL_events.h>
#include <SDL_image.h>

#define AMOUNT_KEYS     16                      // 16 keys (0x0 to 0xF)

#define SCALE           10                      // scale factor for the display

#define CHIP8_WIDTH     64                      // CHIP8 display width in px
#define CHIP8_HEIGHT    32                      // CHIP8 display height in px

#define SCHIP_WIDTH     128                     // SCHIP display width in px
#define SCHIP_HEIGHT    64                      // SCHIP display height in px

typedef struct {
    SDL_Window          *window;                // window for the display
    SDL_Renderer        *renderer;              // renderer for the display
    SDL_Rect            *pixels;                // rectangles for each pixel
    SDL_bool            *pixelDrawn;            // which pixels are drawn?
    SDL_bool            poweredOn;              // power flag
    SDL_bool            reset;                  // reset flag
    SDL_bool            keyDown[AMOUNT_KEYS];     // which keys are pressed?
    SDL_bool            keyUp[AMOUNT_KEYS];       // which keys are released?
    SDL_bool            dirty;                  // does display need redrawing?
    uint32_t            lastUpdate;             // tick count at last update
    int                 width;                  // current width
    int                 height;                 // current height
    int                 pixelWidth;             // cached width / SCALE
    int                 pixelHeight;            // cached height / SCALE
} display;

/*
 * Clear the pixels of the display.
 * Parameter: the display structure
 */
void
resetDisplay(display *display);

/*
 * Create the pixels of the display.
 * Parameter: the display structure
 */
void
createPixels(display *display);

/*
 * Initialize the display.
 * Parameter: the display structure
 * Parameter: string representing the path to the icon file
 * Return: 0 on success
 * Return: 1 on failure
 */
int
initDisplay(display *display, const char *iconPath);

/*
 * Handle an event.
 * Parameter: the display structure
 * Parameter: the event to handle
 */
void
handleEvent(display *display, SDL_Event *event);

/*
 * Draw the background of the display.
 * Parameter: the display structure
 * Return: 0 on success
 * Return: 1 on failure
 */
int
drawBackground(display *display);

/*
 * Draw the pixels of the display.
 * Parameter: the display structure
 * Return: 0 on success
 * Return: 1 on failure
 */
int
drawPixels(display *display);

/*
 * Clear keypress events.
 * Sets all key states to SDL_FALSE.
 * Parameter: the list of key states to be cleared
*/
void
clearKeys(SDL_bool *keys);

#endif /* DISPLAY_H */
