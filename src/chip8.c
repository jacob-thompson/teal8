#include <SDL.h>

#include "../include/emulator.h"
#include "../include/file.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    //SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    /* data that may be configured by args */
    uint16_t rate;
    int opt, longIndex;
    bool mute, force;

    /* defaults */
    rate = DEFAULT_INSTRUCTION_RATE; // 1000 instructions per second
    longIndex = 0; // used as the index for longOptions
    mute = false; // mute audio (-m or --mute)
    force = false; // force load rom regardless of validity (-f or --force)

    /* parsing args */
    while (argc > 1 &&
          (opt = getopt_long(argc, argv,  "fmi:hv", longOptions, &longIndex)) != -1) {
        switch (opt) {
            case 'f': // force
                force = true;
                break;
            case 'm': // mute
                mute = true;
                break;
            case 'i': // ips
                if (isNumber(optarg)) {
                    rate = atoi(optarg);
                } else {
                    SDL_LogError(
                        SDL_LOG_CATEGORY_APPLICATION,
                        "invalid IPS input\n"
                    );
                    return EXIT_FAILURE;
                }

                /* validate the input before we start using it */
                if (rate <= 0) {
                    rate = DEFAULT_INSTRUCTION_RATE;
                }
                break;
            case 'h': // help
                printUsage(argv[0], SDL_LOG_PRIORITY_INFO);
                return EXIT_SUCCESS;
                break;
            case 'v': // version
                printVersion(argv[0]);
                return EXIT_SUCCESS;
                break;
            default: // '?'
                printUsage(argv[0], SDL_LOG_PRIORITY_ERROR);
                return EXIT_FAILURE;
                break;
        }
    }

    /* ensure that a ROM argument was given */
    if (optind >= argc) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "expected ROM argument after options\n"
        );
        return EXIT_FAILURE;
    }

    const char *inputFile = argv[optind];
    FILE *rom = getRom(inputFile);
    struct stat st;

    if (!force && !isFileValid(inputFile, rom, &st)) {
        if (rom != NULL) fclose(rom);
        return EXIT_FAILURE; // error has already been logged
    } else if (force) {
        if (rom == NULL) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "failed to open ROM file: %s\n",
                inputFile
            );
            return EXIT_FAILURE;
        }
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "force loading %s\n",
            inputFile
        );
    } else {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "loading %s\n",
            inputFile
        );
    }

    emulator chip8;
    initializeEmulator(&chip8, rom);
    chip8.muted = mute;

    if (chip8.muted) {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "emulator initialized with %s audio\n",
            chip8.muted ? "muted" : "unmuted"
        );
    }

    if (initDisplay(&chip8.display) != 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "error creating SDL display: %s\n",
            SDL_GetError()
        );
        return EXIT_FAILURE;
    }

    if (!chip8.muted && initAudio(&chip8.sound) != 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "error creating SDL audio: %s\n",
            SDL_GetError()
        );
        return EXIT_FAILURE;
    }

    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "running %s at %d IPS\n",
        inputFile,
        rate
    );

    fclose(rom); // the rom is written to memory, so we can close it now

    uint32_t ticks;
    uint16_t opcode;
    double msPerInstruction = 1000.0 / rate;
    double nextInstructionTime = SDL_GetTicks();

    /* main loop */
    while (chip8.display.poweredOn) {

        /* update timers */
        ticks = SDL_GetTicks();

        /* check if it is time to execute the next instruction */
        if ((double)ticks < nextInstructionTime) {
            continue; // not time yet
        }

        /* schedule next instruction */
        nextInstructionTime += msPerInstruction;

        /*
         * handle timer updates at 60Hz
         * timers are decremented if they are greater than zero
         */
        if (ticks - chip8.timers.lastUpdate >= TIMER_INTERVAL) {
            if (chip8.timers.delay > 0) {
                chip8.timers.delay--;
            }
            if (chip8.timers.sound > 0) {
                SDL_LogDebug(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "beep\n"
                );
                if (!chip8.muted) {
                    /* start audio playback */
                    chip8.sound.playing = true;
                    SDL_PauseAudioDevice(chip8.sound.deviceId, 0);
                }
                chip8.timers.sound--;
            } else if (chip8.sound.playing && !chip8.muted) {
                /* stop audio playback and reset phase */
                chip8.sound.playing = false;
                chip8.sound.phase = 0.0;
                SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
            }
            chip8.timers.lastUpdate = ticks;
        } else if (ticks < chip8.timers.lastUpdate) {
            chip8.timers.lastUpdate = ticks;
        }

        /* handle events */
        clearKeys(chip8.display.keyUp);

        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(&chip8.display, &event);

        if (chip8.display.reset) {
            FILE *resetRom = getRom(inputFile);
            if (resetRom != NULL) {
                initializeEmulator(&chip8, resetRom);
                fclose(resetRom);
            }
            resetDisplay(&chip8.display);
            chip8.display.reset = false;
            nextInstructionTime = SDL_GetTicks();
            continue;
        }

        /* fetch, decode, and execute opcode */
        opcode = fetchOpcode(&chip8);
        chip8.pc += 2; // increment program counter
        decodeAndExecuteOpcode(&chip8, opcode);

        /* draw the frame only if display has changed */
        if (chip8.display.dirty) {
            if (drawBackground(&chip8.display) != 0) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "error drawing background: %s\n",
                    SDL_GetError()
                );
                return EXIT_FAILURE;
            }

            if (drawPixels(&chip8.display) != 0) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "error drawing pixels: %s\n",
                    SDL_GetError()
                );
                return EXIT_FAILURE;
            }

            SDL_RenderPresent(chip8.display.renderer);
            chip8.display.dirty = false;
        }

    }

    /* cleanup */
    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "cleaning up\n"
    );

    if (chip8.sound.poweredOn) {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "shutting down audio\n"
        );
        if (chip8.sound.playing) {
            /* stop audio playback */
            SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
        }
        SDL_CloseAudioDevice(chip8.sound.deviceId);
    }

    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "shutting down display\n"
    );
    free(chip8.display.pixels);
    free(chip8.display.pixelDrawn);
    SDL_DestroyRenderer(chip8.display.renderer);
    SDL_DestroyWindow(chip8.display.window);

    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "quitting SDL\n"
    );
    SDL_Quit();

    return EXIT_SUCCESS;
}
