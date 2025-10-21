#if defined(__APPLE__)
#include <mach-o/dyld.h>
#endif

#include <sys/syslimits.h>

#include <SDL.h>

#include "../include/emulator.h"
#include "../include/file.h"

#define TIMER_DECREMENT_INTERVAL (1000 / 60)

int
main(int argc, char **argv)
{
    char *binPath, *iconPath;
    uint32_t size;

    /* get resource path in order to set the window icon */
#if defined(__APPLE__) // macOS

    _NSGetExecutablePath(NULL, &size); // get the size needed
    binPath = malloc(sizeof(char) * PATH_MAX);
    if (binPath == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for executable path\n"
        );
        return -1;
    }

    if (_NSGetExecutablePath(binPath, &size) == 0) {
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "path = %s\n",
            binPath
        );
    }

    iconPath = malloc(sizeof(char) * PATH_MAX);
    if (iconPath == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for resource path\n"
        );
        return -1;
    }

    int len, charsToTrim;
    len         = strlen(binPath);
    charsToTrim = 10; // /bin/teal8 is 10 characters

    if (len >= charsToTrim) {
        binPath[len - charsToTrim] = '\0'; // trim
    } else {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "executable path is unexpectedly short\n"
        );
        free(binPath);
        free(iconPath);
        return -1;
    }

    /* construct the path to the icon */
    snprintf(iconPath, PATH_MAX, "%s/resources/icon.png", binPath);

#elif defined(__linux__)

    binPath = malloc(sizeof(char) * PATH_MAX);
    if (binPath == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for executable path\n"
        );
        return -1;
    }

    ssize_t count = readlink("/proc/self/exe", binPath, PATH_MAX);
    if (count != -1) {
        binPath[count] = '\0'; // null-terminate the string
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "path = %s\n",
            binPath
        );
    } else {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to read /proc/self/exe\n"
        );
        free(binPath);
        return -1;
    }

    iconPath = malloc(sizeof(char) * PATH_MAX);
    if (iconPath == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for resource path\n"
        );
        return -1;
    }

    int len, charsToTrim;
    len         = strlen(binPath);
    charsToTrim = 10; // /bin/teal8 is 10 characters

    if (len >= charsToTrim) {
        binPath[len - charsToTrim] = '\0'; // trim
    } else {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "executable path is unexpectedly short\n"
        );
        free(binPath);
        free(iconPath);
        return -1;
    }

    /* construct the path to the icon */
    snprintf(iconPath, PATH_MAX, "%s/resources/icon.png", binPath);

#endif

    srand(time(NULL));

    //SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    /* data that may be configured by args */
    uint16_t    rate;
    int         opt, longIndex;
    SDL_bool    mute, force;

    /* defaults */
    rate        = DEFAULT_IPS;                  // 1000 instructions per second
    longIndex   = 0;                            // index for longOptions
    mute        = SDL_FALSE;                    // mute audio (-m or --mute)
    force       = SDL_FALSE;                    // force load rom (-f or --force)

    /* parsing args */
    while (
        argc > 1
        &&
        (opt = getopt_long(argc, argv,  "fmi:hv", longOptions, &longIndex)) != -1
    ) {
        switch (opt) {
            case 'f':   // force
                force = SDL_TRUE;
                break;
            case 'm':   // mute
                mute = SDL_TRUE;
                break;
            case 'i':   // ips
                if (isNumber(optarg)) {
                    rate = atoi(optarg);
                } else {
                    SDL_LogError(
                        SDL_LOG_CATEGORY_APPLICATION,
                        "invalid IPS input\n"
                    );
                    return -1;
                }

                /* validate the input before we start using it */
                if (rate <= 0) {
                    rate = DEFAULT_IPS;
                }
                break;
            case 'h':   // help
                printUsage(argv[0], SDL_LOG_PRIORITY_INFO);
                return 0;
                break;
            case 'v':   // version
                printVersion(argv[0]);
                return 0;
                break;
            default:    // '?'
                printUsage(argv[0], SDL_LOG_PRIORITY_ERROR);
                return -1;
                break;
        }
    }

    /* ensure that a ROM argument was given */
    if (optind >= argc) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "expected ROM argument after options\n"
        );
        return -1;
    }

    const char *inputFile   = argv[optind];
    FILE *rom               = getRom(inputFile);
    struct stat st;

    if (!force && !isRomValid(inputFile, rom, &st)) {
        if (rom != NULL) fclose(rom);
        return -1;      // error has already been logged
    } else if (force) {
        if (rom == NULL) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "failed to open ROM file: %s\n",
                inputFile
            );
            return -1;
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

    if (initDisplay(&chip8.display, iconPath) != 0) {
        return -1;      // error has already been logged
    }

    if (!chip8.muted && initAudio(&chip8.sound) != 0) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "error creating SDL audio: %s\n",
            SDL_GetError()
        );
        return -1;
    }

    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "running %s at %d IPS\n",
        inputFile,
        rate
    );

    fclose(rom);        // the rom is already written to memory

    uint32_t ticks;
    uint16_t opcode;
    const double msPerInstruction   = 1000.0 / rate;
    double nextInstructionTime      = SDL_GetTicks();

    /* main loop */
    while (chip8.display.poweredOn) {

        /* update timers */
        ticks = SDL_GetTicks();

        /* check if it is time to execute the next instruction */
        if ((double)ticks < nextInstructionTime)
            continue;

        /* schedule next instruction */
        nextInstructionTime += msPerInstruction;

        /*
         * handle timer updates at 60Hz
         * timers are decremented if they are greater than zero
         */
        if (ticks - chip8.timers.lastUpdate >= TIMER_DECREMENT_INTERVAL) {
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
                    chip8.sound.playing = SDL_TRUE;
                    SDL_PauseAudioDevice(chip8.sound.deviceId, 0);
                }
                chip8.timers.sound--;
            } else if (chip8.sound.playing && !chip8.muted) {
                /* stop audio playback and reset phase */
                chip8.sound.playing = SDL_FALSE;
                chip8.sound.phase   = 0.0;
                SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
            }
            chip8.timers.lastUpdate = ticks;
        } else if (ticks < chip8.timers.lastUpdate) {
            chip8.timers.lastUpdate = ticks;
        }

        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "timers updated\n"
        );

        /* handle events */
        clearKeys(chip8.display.keyUp);

        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(&chip8.display, &event);

        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "events handled\n"
        );

        if (chip8.display.reset) {
            FILE *resetRom = getRom(inputFile);
            if (resetRom != NULL) {
                initializeEmulator(&chip8, resetRom);
                fclose(resetRom);
            }
            resetDisplay(&chip8.display);
            chip8.display.reset = SDL_FALSE;
            nextInstructionTime = SDL_GetTicks();
            continue;
        }

        /* fetch, decode, and execute opcode */
        opcode = fetchOpcode(&chip8);
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "handling opcode %x\n",
            opcode
        );

        chip8.pc += 2; // increment program counter

        decodeAndExecuteOpcode(&chip8, opcode);
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "opcode successfully fetched, decoded, and executed\n"
        );

        /* draw the frame only if display has changed */
        if (chip8.display.dirty) {
            if (drawBackground(&chip8.display) != 0) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "error drawing background: %s\n",
                    SDL_GetError()
                );
                return -1;
            }

            if (drawPixels(&chip8.display) != 0) {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "error drawing pixels: %s\n",
                    SDL_GetError()
                );
                return -1;
            }

            SDL_RenderPresent(chip8.display.renderer);
            chip8.display.dirty = SDL_FALSE;
        }

    }

    /* cleanup */
    SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "cleaning up\n"
    );

    free(binPath);
    free(iconPath);

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
    IMG_Quit();
    SDL_Quit();

    return 0;
}
