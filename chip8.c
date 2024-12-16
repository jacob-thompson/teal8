#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL.h>
#include <SDL_timer.h>
#include <SDL_image.h>

#define MEMORY_IN_BYTES 4096
#define FONT_IN_BYTES 80
#define FONT_START_ADDRESS 0x050
#define FONT_END_ADDRESS 0x09F
#define STACK_SIZE 16
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 320

typedef struct {
    unsigned char memory[MEMORY_IN_BYTES]; // 4KB memory
    unsigned char stack[STACK_SIZE]; // 16 16-bit registers
    unsigned short ix; // 16-bit register
    unsigned short pc; // program counter
    unsigned char sp; // stack pointer
    int8_t dtimer; // delay timer
    int8_t stimer; // sound timer
} chip8;

void write_font_to_memory(unsigned char* memory)
{
    unsigned char font[FONT_IN_BYTES] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };

    for (int loc = FONT_START_ADDRESS; loc <= FONT_END_ADDRESS; loc++)
        memory[loc] = font[loc - FONT_START_ADDRESS];
}

int main(void)
{
    chip8 chip8;

    write_font_to_memory(chip8.memory);

    // initialize SDL
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error initializing SDL: %s\n", SDL_GetError());
        return 1;
    }

    // create a window
    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (window == NULL) {
        fprintf(stderr, "error creating window: %s\n", SDL_GetError());
        return 1;
    }

    // create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        fprintf(stderr, "error creating renderer: %s\n", SDL_GetError());
        return 1;
    }

    // create grid of pixels
    bool pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            pixels[y][x] = false;
        }
    }
    pixels[4][4] = true;

    // main loop
    bool running = true;
    while (running) {

        // handle events
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // draw background
        if (SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255) != 0) {
            fprintf(stderr, "error setting render draw color: %s\n", SDL_GetError());
            return 1;
        }
        if (SDL_RenderClear(renderer) != 0) {
            fprintf(stderr, "error clearing renderer: %s\n", SDL_GetError());
            return 1;
        }

        // draw pixels
        if (SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255) != 0) {
            fprintf(stderr, "error setting render draw color: %s\n", SDL_GetError());
            return 1;
        }
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (pixels[y / 10][x / 10] && SDL_RenderDrawPoint(renderer, x, y) != 0) {
                    fprintf(stderr, "error drawing point: %s\n", SDL_GetError());
                    return 1;
                }
            }
        }

        // present the renderer
        SDL_RenderPresent(renderer);

        // 60 frames per second
        SDL_Delay(1000 / 60);
    }

    // clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
