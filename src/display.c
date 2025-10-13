#include <SDL.h>

#include "../include/display.h"

void resetDisplay(display *display)
{
    if (display->pixelDrawn == NULL)
        return;

    for (int y = 0; y < display->pixelHeight; y++)
        for (int x = 0; x < display->pixelWidth; x++)
            display->pixelDrawn[y * display->pixelWidth + x] = false;
    display->dirty = true;
}

void createPixels(display *display)
{
    if (display->pixels != NULL)
        free(display->pixels);

    if (display->pixelDrawn != NULL)
        free(display->pixelDrawn);

    display->pixelWidth = display->width / SCALE;
    display->pixelHeight = display->height / SCALE;

    display->pixels = calloc(
        display->pixelHeight * display->pixelWidth,
        sizeof(SDL_Rect));

    if (display->pixels == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for pixels\n"
        );
        return;
    }

    for (int y = 0; y < display->pixelHeight; y++) {
        for (int x = 0; x < display->pixelWidth; x++) {
            display->pixels[y * display->pixelWidth + x].x = x * SCALE;
            display->pixels[y * display->pixelWidth + x].y = y * SCALE;
            display->pixels[y * display->pixelWidth + x].w = SCALE;
            display->pixels[y * display->pixelWidth + x].h = SCALE;
        }
    }

    display->pixelDrawn = calloc(
        display->pixelHeight * display->pixelWidth,
        sizeof(bool));

    if (display->pixelDrawn == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for pixelDrawn\n"
        );
        free(display->pixels);
        display->pixels = NULL;
        return;
    }
}

int initDisplay(display *display, const char *iconPath)
{
    if (
        SDL_InitSubSystem(
            SDL_INIT_TIMER
            |
            SDL_INIT_VIDEO
            |
            SDL_INIT_EVENTS
        ) != EXIT_SUCCESS) {
        return EXIT_FAILURE;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to initialize SDL_image: %s\n",
            IMG_GetError()
        );
        return EXIT_FAILURE;
    }

    display->window = SDL_CreateWindow(
        "teal8",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        CHIP8_WIDTH * SCALE,
        CHIP8_HEIGHT * SCALE,
        0
    );
    if (display->window == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to create window: %s\n",
            SDL_GetError()
        );
        return EXIT_FAILURE;
    }

    SDL_GetWindowSize(display->window, &display->width, &display->height);

    display->renderer = SDL_CreateRenderer(
        display->window,
        -1,
        SDL_RENDERER_ACCELERATED
    );
    if (display->renderer == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to create renderer: %s\n",
            SDL_GetError()
        );
        SDL_DestroyWindow(display->window);
        return EXIT_FAILURE;
    }

    SDL_Surface *iconSurface = IMG_Load(iconPath);
    if (iconSurface == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to load icon: %s\n",
            IMG_GetError()
        );
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        return EXIT_FAILURE;
    }
    SDL_SetWindowIcon(display->window, iconSurface);
    SDL_FreeSurface(iconSurface);

    display->pixels = NULL;
    display->pixelDrawn = NULL;

    createPixels(display);

    resetDisplay(display);

    display->poweredOn = true;
    display->reset = false;
    display->dirty = true;

    return EXIT_SUCCESS;
}

void handleEvent(display *display, SDL_Event *event)
{
    switch (event->type) {
        /* handle key presses */
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    display->poweredOn = false;
                    break;
                case SDL_SCANCODE_SPACE: // restart the rom
                    display->reset = true;
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

        /* quit gracefully */
        case SDL_QUIT:
            display->poweredOn = false;
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
    if (display->pixelDrawn == NULL || display->pixels == NULL)
        return EXIT_FAILURE;

    if (SDL_SetRenderDrawColor(display->renderer, 255, 255, 255, 255) != EXIT_SUCCESS)
        return EXIT_FAILURE;

    for (int y = 0; y < display->pixelHeight; y++)
        for (int x = 0; x < display->pixelWidth; x++)
            if (
                display->pixelDrawn[y * display->pixelWidth + x]
                &&
                SDL_RenderFillRect(
                    display->renderer,
                    &display->pixels[y * display->pixelWidth + x]
                )
                !=
                EXIT_SUCCESS
            )
                return EXIT_FAILURE;

    return EXIT_SUCCESS;
}

void clearKeys(bool *keys)
{
    for (int i = 0x0; i <= 0xF; i++)
        keys[i] = false;
}
