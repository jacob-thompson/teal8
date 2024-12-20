#include "../include/emulator.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    uint16_t rate = DEFAULT_INSTRUCTION_RATE;
    if (argc < 2 || argc > 3) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "usage: %s <rom> <rate>\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc == 3) {
        rate = atoi(argv[2]);
        rate = roundRate(rate);
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "opening rom: %s\n", argv[1]);

    FILE *rom = getRom(argv[1]);
    if (rom == NULL) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error opening rom: %s\n", argv[1]);
        return EXIT_FAILURE;
    } else {
        SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "rom opened successfully\n");
    }

    emulator chip8;
    initializeEmulator(&chip8, rom);
    if (initDisplay(&chip8.display) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error creating SDL display: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "running %s at %d Hz\n", argv[1], rate);

    fclose(rom); // the rom is written to memory, so we can close it now

    uint32_t ticks;
    uint16_t opcode;

    // main loop
    while (chip8.display.powered_on) {

        // update timers
        ticks = SDL_GetTicks();

        if (ticks - chip8.lastUpdate >= 1000 / rate) {
            chip8.lastUpdate = ticks;
        } else if (ticks < chip8.lastUpdate) {
            chip8.lastUpdate = ticks;
        } else {
            continue;
        }

        if (ticks - chip8.timers.lastUpdate >= 1000 / TIMER_RATE) {
            if (chip8.timers.delay > 0) {
                chip8.timers.delay--;
            }
            if (chip8.timers.sound > 0) {
                SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "beep\n");
                chip8.timers.sound--;
            }
            chip8.timers.lastUpdate = ticks;
        } else if (ticks < chip8.timers.lastUpdate) {
            chip8.timers.lastUpdate = ticks;
        }

        // events
        clearKeys(chip8.display.keyUp);

        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(&chip8.display, &event);

        // fetch, decode, and execute opcode
        opcode = fetchOpcode(&chip8);
        //printf("opcode: %04x\n", opcode);

        chip8.pc += 2;

        decodeOpcode(&chip8, opcode);

        // draw the frame
        if (drawBackground(&chip8.display) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error drawing background: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }

        if (drawPixels(&chip8.display) != 0) {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error drawing pixels: %s\n", SDL_GetError());
            return EXIT_FAILURE;
        }

        SDL_RenderPresent(chip8.display.renderer);

    }

    // cleanup
    SDL_DestroyRenderer(chip8.display.renderer);
    SDL_DestroyWindow(chip8.display.window);
    SDL_Quit();

    return EXIT_SUCCESS;
}
