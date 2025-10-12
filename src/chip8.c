#include "../include/emulator.h"
#include "../include/file.h"

int main(int argc, char **argv)
{
    srand(time(NULL));

    //SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    uint16_t rate = DEFAULT_INSTRUCTION_RATE;
    bool mute = false; // mute audio (-m or --mute)
    bool force = false; // force load rom regardless of validity (-f or --force)

    /* args */
    if (argc < 2 || argc > 5) {
        printUsage(argv[0], SDL_LOG_PRIORITY_ERROR);
        return EXIT_FAILURE;
    } else if (argc == 2 &&
              (strcmp("-h", argv[1]) == 0 ||
               strcmp("--help", argv[1]) == 0)) {
        printUsage(argv[0], SDL_LOG_PRIORITY_INFO);
        return EXIT_SUCCESS;
    } else if (argc == 2 &&
              (strcmp("-v", argv[1]) == 0 ||
               strcmp("--version", argv[1]) == 0)) {
        printVersion(argv[0]);
        return EXIT_SUCCESS;
    } else if (argc == 3) {
        /* check for valid flags and rate */
        if (isNumber(argv[2])) {
            rate = atoi(argv[2]);
        } else if (strcmp("-m", argv[2]) == 0 || strcmp("--mute", argv[2]) == 0) {
            mute = true;
        } else if (strcmp("-f", argv[2]) == 0 || strcmp("--force", argv[2]) == 0) {
            force = true;
        } else {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "invalid argument: %s\n",
                argv[2]
            );
            return EXIT_FAILURE;
        }
    } else if (argc == 4) {
        /* check for valid flags and rate */
        if (isNumber(argv[2]) &&
            (strcmp("-m", argv[3]) == 0 || strcmp("--mute", argv[3]) == 0)) {
            rate = atoi(argv[2]);
            mute = true;
        } else if (isNumber(argv[2]) &&
                  (strcmp("-f", argv[3]) == 0 || strcmp("--force", argv[3]) == 0)) {
            rate = atoi(argv[2]);
            force = true;
        } else if ((strcmp("-m", argv[2]) == 0 || strcmp("--mute", argv[2]) == 0) &&
                   (strcmp("-f", argv[3]) == 0 || strcmp("--force", argv[3]) == 0)) {
            mute = true;
            force = true;
        } else if ((strcmp("-f", argv[2]) == 0 || strcmp("--force", argv[2]) == 0) &&
                   (strcmp("-m", argv[3]) == 0 || strcmp("--mute", argv[3]) == 0)) {
            mute = true;
            force = true;
        } else {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "invalid arguments: %s %s\n",
                argv[2],
                argv[3]
            );
            return EXIT_FAILURE;
        }
    } else if (argc == 5) {
        if (isNumber(argv[2]) &&
          ((strcmp("-m", argv[3]) == 0 || strcmp("--mute", argv[3]) == 0) &&
           (strcmp("-f", argv[4]) == 0 || strcmp("--force", argv[4]) == 0))) {
            rate = atoi(argv[2]);
            mute = true;
            force = true;
        } else if (isNumber(argv[2]) &&
                 ((strcmp("-f", argv[3]) == 0 || strcmp("--force", argv[3]) == 0) &&
                  (strcmp("-m", argv[4]) == 0 || strcmp("--mute", argv[4]) == 0))) {
            rate = atoi(argv[2]);
            mute = true;
            force = true;
        } else {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "invalid arguments: %s %s\n",
                argv[3],
                argv[4]
            );
            return EXIT_FAILURE;
        }
    }

    FILE *rom = getRom(argv[1]);
    struct stat st;

    if (!force && !isFileValid(argv[1], rom, &st)) {
        return EXIT_FAILURE; // error has already been logged
    } else if (force) {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "force loading %s\n",
            argv[1]
        );
    } else {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "loading %s\n",
            argv[1]
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
        argv[1],
        rate
    );

    fclose(rom); // the rom is written to memory, so we can close it now

    uint32_t ticks;
    uint16_t opcode;
    uint16_t timerDelay = 1000 / TIMER_RATE;
    double msPerInstruction = 1000.0 / rate;
    double nextInstructionTime = SDL_GetTicks();

    /* main loop */
    while (chip8.display.poweredOn) {

        /* update timers */
        ticks = SDL_GetTicks();

        /* check if it is time to execute the next instruction */
        if ((double)ticks < nextInstructionTime) {
            /* not time yet, but still handle events */
            clearKeys(chip8.display.keyUp);

            SDL_Event event;
            while (SDL_PollEvent(&event))
                handleEvent(&chip8.display, &event);

            if (chip8.display.reset) {
                FILE *resetRom = getRom(argv[1]);
                if (resetRom != NULL) {
                    initializeEmulator(&chip8, resetRom);
                    fclose(resetRom);
                }
                resetDisplay(&chip8.display);
                chip8.display.reset = false;
                nextInstructionTime = SDL_GetTicks();
            }

            continue;
        }

        /* schedule next instruction */
        nextInstructionTime += msPerInstruction;

        /* update timers */
        if (ticks - chip8.timers.lastUpdate >= timerDelay) {
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
            FILE *resetRom = getRom(argv[1]);
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
