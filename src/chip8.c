#include "../include/emulator.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    uint16_t rate = DEFAULT_INSTRUCTION_RATE;
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "usage: %s <rom> <rate> (default rate: %d)\n", argv[0], DEFAULT_INSTRUCTION_RATE);
        return 1;
    } else if (argc == 3) {
        rate = atoi(argv[2]);
        if (rate <= 0) {
            fprintf(stderr, "invalid rate: %d\n", rate);
            return 1;
        }
    }

    fprintf(stdout, "opening rom: %s\n", argv[1]);

    FILE *rom = getRom(argv[1]);
    if (rom == NULL) {
        fprintf(stderr, "error opening rom: %s\n", argv[1]);
        return 1;
    } else {
        fprintf(stdout, "rom opened successfully\n");
    }

    emulator chip8;
    initializeEmulator(&chip8, rom);
    if (initDisplay(&chip8.display) != 0) {
        fprintf(stderr, "error creating SDL display: %s\n", SDL_GetError());
        return 1;
    }

    uint32_t ticks = SDL_GetTicks();

    uint16_t opcode;

    // main loop
    while (chip8.display.powered_on) {
        clearKeys(chip8.display.key_pressed);

        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(&chip8.display, &event);

        opcode = fetchOpcode(&chip8);
        //printf("opcode: %04x\n", opcode);

        chip8.pc += 2;

        decodeOpcode(&chip8, opcode);

        // update timers
        // use SDL_GetTicks() to get the number of milliseconds since the program started
        // since the timers are decremented at a rate of 60Hz, we can use this to decrement the timers
        // the timers are decremented at a rate of 60Hz, so we need to decrement them every 1000 / 60 milliseconds
        //printf("ticks: %d\n", ticks);
        ticks = SDL_GetTicks();
        if (ticks - chip8.timers.lastUpdate >= 1000 / TIMER_RATE) {
            if (chip8.timers.delay > 0) {
                chip8.timers.delay--;
            }
            if (chip8.timers.sound > 0) {
                chip8.timers.sound--;
            }
            chip8.timers.lastUpdate = ticks;
        } else if (ticks < chip8.timers.lastUpdate) {
            chip8.timers.lastUpdate = ticks;
        }

        SDL_Delay(1000 / rate);

        // draw the frame
        if (drawBackground(&chip8.display) != 0) {
            fprintf(stderr, "error drawing background\n");
            return 1;
        }

        if (drawPixels(&chip8.display) != 0) {
            fprintf(stderr, "error drawing pixels\n");
            return 1;
        }

        SDL_RenderPresent(chip8.display.renderer);
    }

    // cleanup
    SDL_DestroyRenderer(chip8.display.renderer);
    SDL_DestroyWindow(chip8.display.window);
    SDL_Quit();

    return 0;
}
