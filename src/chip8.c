#include "../include/emulator.h"
#include "../include/file.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    //SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    uint16_t rate = DEFAULT_INSTRUCTION_RATE;
    bool mute = false;
    if (argc < 2 || argc > 4) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "usage: %s <rom> <rate>\n", argv[0]);
        return EXIT_FAILURE;
    } else if (argc == 3 && isNumber(argv[2])) {
        rate = atoi(argv[2]);
    } else if (argc == 3 && !isNumber(argv[2])) {
        if (strcmp("-m", argv[2]) == 0 || strcmp("--mute", argv[2]) == 0) {
            mute = true;
        }
    } else if (argc == 4) {
        if (strcmp("-m", argv[3]) == 0 || strcmp("--mute", argv[3]) == 0) {
            mute = true;
        }
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "opening %s\n", argv[1]);

    FILE *rom = getRom(argv[1]);
    struct stat st;

    if (!isFileValid(argv[1], rom, &st)) {
        return EXIT_FAILURE; // error has already been logged
    }

    emulator chip8;
    initializeEmulator(&chip8, rom);
    chip8.muted = mute;
    if (initDisplay(&chip8.display) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error creating SDL display: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if (!chip8.muted && initAudio(&chip8.sound) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error creating SDL audio: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "running %s at %d Hz\n", argv[1], rate);

    fclose(rom); // the rom is written to memory, so we can close it now

    uint32_t ticks;
    uint16_t opcode;
    uint16_t instructionDelay = 1000 / rate;  // Cache division
    uint16_t timerDelay = 1000 / TIMER_RATE;  // Cache division

    // main loop
    while (chip8.display.poweredOn) {

        // update timers
        ticks = SDL_GetTicks();

        if (ticks - chip8.lastUpdate >= instructionDelay) {
            chip8.lastUpdate = ticks;
        } else if (ticks < chip8.lastUpdate) {
            chip8.lastUpdate = ticks;
        } else {
            continue;
        }

        if (ticks - chip8.timers.lastUpdate >= timerDelay) {
            if (chip8.timers.delay > 0) {
                chip8.timers.delay--;
            }
            if (chip8.timers.sound > 0) {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "beep\n");
                if (!chip8.muted) {
                    // start audio playback
                    chip8.sound.playing = true;
                    SDL_PauseAudioDevice(chip8.sound.deviceId, 0);
                }
                chip8.timers.sound--;
            } else if (chip8.sound.playing && !chip8.muted) {
                // stop audio playback and reset phase
                chip8.sound.playing = false;
                chip8.sound.phase = 0.0;
                SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
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

        if (chip8.display.reset) {
            initializeEmulator(&chip8, getRom(argv[1]));
            resetDisplay(&chip8.display);
            chip8.display.reset = false;
            continue;
        }

        // fetch, decode, and execute opcode
        opcode = fetchOpcode(&chip8);

        //printf("opcode: %04x\n", opcode);
        //printf("v0: %02x\n", chip8.v[0]);
        //printf("v1: %02x\n", chip8.v[1]);
        //printf("v2: %02x\n", chip8.v[2]);
        //printf("v3: %02x\n", chip8.v[3]);
        //printf("v4: %02x\n", chip8.v[4]);
        //printf("v5: %02x\n", chip8.v[5]);
        //printf("v6: %02x\n", chip8.v[6]);
        //printf("v7: %02x\n", chip8.v[7]);
        //printf("v8: %02x\n", chip8.v[8]);
        //printf("v9: %02x\n", chip8.v[9]);
        //printf("va: %02x\n", chip8.v[10]);
        //printf("vb: %02x\n", chip8.v[11]);
        //printf("vc: %02x\n", chip8.v[12]);
        //printf("vd: %02x\n", chip8.v[13]);
        //printf("ve: %02x\n", chip8.v[14]);
        //printf("vf: %02x\n", chip8.v[15]);
        //printf("stacked values: %d\n", stacked(&chip8.stack));
        //printf(" ------- \n");

        chip8.pc += 2;

        decodeOpcode(&chip8, opcode);

        // draw the frame only if display has changed
        if (chip8.display.dirty) {
            if (drawBackground(&chip8.display) != 0) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error drawing background: %s\n", SDL_GetError());
                return EXIT_FAILURE;
            }

            if (drawPixels(&chip8.display) != 0) {
                SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error drawing pixels: %s\n", SDL_GetError());
                return EXIT_FAILURE;
            }

            SDL_RenderPresent(chip8.display.renderer);
            chip8.display.dirty = false;
        }

    }

    // cleanup
    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "cleaning up\n");

    if (chip8.sound.poweredOn) {
        SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "shutting down audio\n");
        if (chip8.sound.playing) {
            // stop audio playback and reset phase
            chip8.sound.phase = 0.0;
            SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
        }
        SDL_CloseAudioDevice(chip8.sound.deviceId);
    }

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "shutting down display\n");
    free(chip8.display.pixels);
    free(chip8.display.pixelDrawn);
    SDL_DestroyRenderer(chip8.display.renderer);
    SDL_DestroyWindow(chip8.display.window);

    SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "quitting SDL\n");
    SDL_Quit();

    return EXIT_SUCCESS;
}
