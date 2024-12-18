#include "../include/emulator.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    if (argc != 2) {
        fprintf(stderr, "usage: %s <rom>\n", argv[0]);
        return 1;
    } else {
        fprintf(stdout, "opening rom: %s\n", argv[1]);
    }

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

    unsigned short opcode;

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
        if (chip8.timers.delay > 0)
            chip8.timers.delay--;
        if (chip8.timers.sound > 0)
            chip8.timers.sound--;

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

        SDL_Delay(1000 / 60); // 60 loops per second

    }

    // cleanup
    SDL_DestroyRenderer(chip8.display.renderer);
    SDL_DestroyWindow(chip8.display.window);
    SDL_Quit();

    return 0;
}
