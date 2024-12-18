#include "../include/display.h"

void resetDisplay(display *display)
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            display->pixels[y][x] = false;
}

int initDisplay(display *display)
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

    resetDisplay(display);

    display->powered_on = true;

    return EXIT_SUCCESS;
}

void handleEvent(display *display, SDL_Event *event)
{
    switch (event->type) {
        // handle key presses
        case SDL_KEYDOWN:
            switch (event->key.keysym.sym) {
                case SDLK_ESCAPE:
                    display->powered_on = false;
                    break;
                case SDLK_1:
                    display->key_pressed[0x1] = true;
                    break;
                case SDLK_2:
                    display->key_pressed[0x2] = true;
                    break;
                case SDLK_3:
                    display->key_pressed[0x3] = true;
                    break;
                case SDLK_4:
                    display->key_pressed[0xC] = true;
                    break;
                case SDLK_q:
                    display->key_pressed[0x4] = true;
                    break;
                case SDLK_w:
                    display->key_pressed[0x5] = true;
                    break;
                case SDLK_e:
                    display->key_pressed[0x6] = true;
                    break;
                case SDLK_r:
                    display->key_pressed[0xD] = true;
                    break;
                case SDLK_a:
                    display->key_pressed[0x7] = true;
                    break;
                case SDLK_s:
                    display->key_pressed[0x8] = true;
                    break;
                case SDLK_d:
                    display->key_pressed[0x9] = true;
                    break;
                case SDLK_f:
                    display->key_pressed[0xE] = true;
                    break;
                case SDLK_z:
                    display->key_pressed[0xA] = true;
                    break;
                case SDLK_x:
                    display->key_pressed[0x0] = true;
                    break;
                case SDLK_c:
                    display->key_pressed[0xB] = true;
                    break;
                case SDLK_v:
                    display->key_pressed[0xF] = true;
            }
            break;

        // quit gracefully
        case SDL_QUIT:
            display->powered_on = false;
    }
}

int drawBackground(display *display)
{
    if (SDL_SetRenderDrawColor(display->renderer, 0, 0, 0, 255) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    if (SDL_RenderClear(display->renderer) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

int drawPixels(display *display)
{
    if (SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
            if (display->pixels[y / 10][x / 10] && SDL_RenderDrawPoint(display->renderer, x, y) != EXIT_SUCCESS)
                return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

void clearKeys(bool *keys)
{
    for (int i = 0x0; i <= 0xF; i++)
        keys[i] = false;
}
