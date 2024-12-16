#include <stdbool.h>

#include <SDL.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct display {
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
    bool powered_on;
} display;

int init_display(display* display);

int draw_background(display* chip8_display);

int draw_pixels(display* chip8_display);
