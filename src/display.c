#include "../include/display.h"

void reset_display(display *display)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            display->pixels[y][x] = false;
}

int init_display(display *display)
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    display->window = SDL_CreateWindow(
        "CHIP-8 Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0
    );
    if (display->window == NULL)
        return EXIT_FAILURE;

    display->renderer = SDL_CreateRenderer(display->window, -1, SDL_RENDERER_ACCELERATED);
    if (display->renderer == NULL)
        return EXIT_FAILURE;

    reset_display(display);

    display->powered_on = true;

    return EXIT_SUCCESS;
}

int draw_background(display *display)
{
    if (SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    if (SDL_RenderClear(display->renderer) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int draw_pixels(display *display)
{
    if (SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            if (display->pixels[y / 10][x / 10] && SDL_RenderDrawPoint(display->renderer, x, y) != EXIT_SUCCESS)
                return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
