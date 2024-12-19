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
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    display->powered_on = false;
                    break;
                case SDL_SCANCODE_1:
                    display->keyDown[0x1] = false;
                    display->keyUp[0x1] = true;
                    break;
                case SDL_SCANCODE_2:
                    display->keyDown[0x2] = false;
                    display->keyUp[0x2] = true;
                    break;
                case SDL_SCANCODE_3:
                    display->keyDown[0x3] = false;
                    display->keyUp[0x3] = true;
                    break;
                case SDL_SCANCODE_4:
                    display->keyDown[0xC] = false;
                    display->keyUp[0xC] = true;
                    break;
                case SDL_SCANCODE_Q:
                    display->keyDown[0x4] = false;
                    display->keyUp[0x4] = true;
                    break;
                case SDL_SCANCODE_W:
                    display->keyDown[0x5] = false;
                    display->keyUp[0x5] = true;
                    break;
                case SDL_SCANCODE_E:
                    display->keyDown[0x6] = false;
                    display->keyUp[0x6] = true;
                    break;
                case SDL_SCANCODE_R:
                    display->keyDown[0xD] = false;
                    display->keyUp[0xD] = true;
                    break;
                case SDL_SCANCODE_A:
                    display->keyDown[0x7] = false;
                    display->keyUp[0x7] = true;
                    break;
                case SDL_SCANCODE_S:
                    display->keyDown[0x8] = false;
                    display->keyUp[0x8] = true;
                    break;
                case SDL_SCANCODE_D:
                    display->keyDown[0x9] = false;
                    display->keyUp[0x9] = true;
                    break;
                case SDL_SCANCODE_F:
                    display->keyDown[0xE] = false;
                    display->keyUp[0xE] = true;
                    break;
                case SDL_SCANCODE_Z:
                    display->keyDown[0xA] = false;
                    display->keyUp[0xA] = true;
                    break;
                case SDL_SCANCODE_X:
                    display->keyDown[0x0] = false;
                    display->keyUp[0x0] = true;
                    break;
                case SDL_SCANCODE_C:
                    display->keyDown[0xB] = false;
                    display->keyUp[0xB] = true;
                    break;
                case SDL_SCANCODE_V:
                    display->keyDown[0xF] = false;
                    display->keyUp[0xF] = true;
                    break;
                default:
                    break;
            }
            break;

        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_1:
                    display->keyDown[0x1] = true;
                    break;
                case SDL_SCANCODE_2:
                    display->keyDown[0x2] = true;
                    break;
                case SDL_SCANCODE_3:
                    display->keyDown[0x3] = true;
                    break;
                case SDL_SCANCODE_4:
                    display->keyDown[0xC] = true;
                    break;
                case SDL_SCANCODE_Q:
                    display->keyDown[0x4] = true;
                    break;
                case SDL_SCANCODE_W:
                    display->keyDown[0x5] = true;
                    break;
                case SDL_SCANCODE_E:
                    display->keyDown[0x6] = true;
                    break;
                case SDL_SCANCODE_R:
                    display->keyDown[0xD] = true;
                    break;
                case SDL_SCANCODE_A:
                    display->keyDown[0x7] = true;
                    break;
                case SDL_SCANCODE_S:
                    display->keyDown[0x8] = true;
                    break;
                case SDL_SCANCODE_D:
                    display->keyDown[0x9] = true;
                    break;
                case SDL_SCANCODE_F:
                    display->keyDown[0xE] = true;
                    break;
                case SDL_SCANCODE_Z:
                    display->keyDown[0xA] = true;
                    break;
                case SDL_SCANCODE_X:
                    display->keyDown[0x0] = true;
                    break;
                case SDL_SCANCODE_C:
                    display->keyDown[0xB] = true;
                    break;
                case SDL_SCANCODE_V:
                    display->keyDown[0xF] = true;
                    break;
                default:
                    break;
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
