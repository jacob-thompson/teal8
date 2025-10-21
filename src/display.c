#include <SDL.h>

#include "../include/display.h"

#define BLACK_PIXEL_COLOR       0, 0, 0, 255            // RGBA for black pixel
#define WHITE_PIXEL_COLOR       255, 255, 255, 255      // RGBA for white pixel

void
resetDisplay(display *display)
{
    if (display->pixelDrawn == NULL)
        return;

    memset(
        display->pixelDrawn,
        SDL_FALSE,
        display->pixelHeight
        *
        display->pixelWidth
        *
        sizeof *display->pixelDrawn
    );

    display->dirty = SDL_TRUE;
}

void
createPixels(display *display)
{
    if (display->pixels != NULL)
        free(display->pixels);

    if (display->pixelDrawn != NULL)
        free(display->pixelDrawn);

    display->pixelWidth     =   display->width / SCALE;
    display->pixelHeight    =   display->height / SCALE;

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
        sizeof(SDL_bool));

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

int
initDisplay(display *display, const char *iconPath)
{
    if (
        SDL_InitSubSystem(
            SDL_INIT_TIMER
            |
            SDL_INIT_VIDEO
            |
            SDL_INIT_EVENTS
        ) != 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to initialize SDL (display): %s\n",
            SDL_GetError()
        );
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to initialize SDL_image: %s\n",
            IMG_GetError()
        );
        return -1;
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
        return -1;
    }

    SDL_GetWindowSize(
        display->window,
        &display->width,
        &display->height
    );

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
        return -1;
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
        return -1;
    }
    SDL_SetWindowIcon(display->window, iconSurface);
    SDL_FreeSurface(iconSurface);

    display->pixels     = NULL;
    display->pixelDrawn = NULL;

    createPixels(display);

    resetDisplay(display);

    display->poweredOn  = SDL_TRUE;
    display->reset      = SDL_FALSE;
    display->dirty      = SDL_TRUE;

    display->lastUpdate = 0;

    return 0;
}

void
handleEvent(display *display, SDL_Event *event)
{
    switch (event->type) {
        /* handle key presses */
        case SDL_KEYUP:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_ESCAPE:
                    display->poweredOn = SDL_FALSE;
                    break;
                case SDL_SCANCODE_SPACE: // restart the rom
                    display->reset = SDL_TRUE;
                    break;
                case SDL_SCANCODE_1:
                    display->keyDown[0x1] = SDL_FALSE;
                    display->keyUp[0x1] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_2:
                    display->keyDown[0x2] = SDL_FALSE;
                    display->keyUp[0x2] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_3:
                    display->keyDown[0x3] = SDL_FALSE;
                    display->keyUp[0x3] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_4:
                    display->keyDown[0xC] = SDL_FALSE;
                    display->keyUp[0xC] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_Q:
                    display->keyDown[0x4] = SDL_FALSE;
                    display->keyUp[0x4] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_W:
                    display->keyDown[0x5] = SDL_FALSE;
                    display->keyUp[0x5] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_E:
                    display->keyDown[0x6] = SDL_FALSE;
                    display->keyUp[0x6] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_R:
                    display->keyDown[0xD] = SDL_FALSE;
                    display->keyUp[0xD] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_A:
                    display->keyDown[0x7] = SDL_FALSE;
                    display->keyUp[0x7] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_S:
                    display->keyDown[0x8] = SDL_FALSE;
                    display->keyUp[0x8] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_D:
                    display->keyDown[0x9] = SDL_FALSE;
                    display->keyUp[0x9] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_F:
                    display->keyDown[0xE] = SDL_FALSE;
                    display->keyUp[0xE] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_Z:
                    display->keyDown[0xA] = SDL_FALSE;
                    display->keyUp[0xA] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_X:
                    display->keyDown[0x0] = SDL_FALSE;
                    display->keyUp[0x0] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_C:
                    display->keyDown[0xB] = SDL_FALSE;
                    display->keyUp[0xB] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_V:
                    display->keyDown[0xF] = SDL_FALSE;
                    display->keyUp[0xF] = SDL_TRUE;
                    break;
                default:
                    break;
            }
            break;

        case SDL_KEYDOWN:
            switch (event->key.keysym.scancode) {
                case SDL_SCANCODE_1:
                    display->keyDown[0x1] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_2:
                    display->keyDown[0x2] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_3:
                    display->keyDown[0x3] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_4:
                    display->keyDown[0xC] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_Q:
                    display->keyDown[0x4] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_W:
                    display->keyDown[0x5] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_E:
                    display->keyDown[0x6] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_R:
                    display->keyDown[0xD] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_A:
                    display->keyDown[0x7] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_S:
                    display->keyDown[0x8] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_D:
                    display->keyDown[0x9] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_F:
                    display->keyDown[0xE] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_Z:
                    display->keyDown[0xA] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_X:
                    display->keyDown[0x0] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_C:
                    display->keyDown[0xB] = SDL_TRUE;
                    break;
                case SDL_SCANCODE_V:
                    display->keyDown[0xF] = SDL_TRUE;
                    break;
                default:
                    break;
            }
            break;

        /* quit gracefully */
        case SDL_QUIT:
            display->poweredOn = SDL_FALSE;
    }
}

int
drawBackground(display *display)
{
    if (
        SDL_SetRenderDrawColor(display->renderer, BLACK_PIXEL_COLOR)
        != 0
        ||
        SDL_RenderClear(display->renderer)
        != 0
    )
        return -1;

    if (SDL_RenderClear(display->renderer) != 0)
        return -1;

    return 0;
}

int
drawPixels(display *display)
{
    if (
        display->pixelDrawn == NULL
        ||
        display->pixels == NULL
        ||
        SDL_SetRenderDrawColor(display->renderer, WHITE_PIXEL_COLOR)
        != 0
    )
        return -1;

    for (int y = 0; y < display->pixelHeight; y++)
        for (int x = 0; x < display->pixelWidth; x++)
            if (
                display->pixelDrawn[y * display->pixelWidth + x]
                &&
                SDL_RenderFillRect(
                    display->renderer,
                    &display->pixels[y * display->pixelWidth + x]
                )
                != 0
            )
                return -1;

    return 0;
}

void
clearKeys(SDL_bool *keys)
{
    memset(keys, SDL_FALSE, KEY_COUNT * sizeof *keys);
}
