#include <SDL.h>

#include "../include/emulator.h"
#include "../include/file.h"

#define TIMER_DECREMENT_INTERVAL_MS (1000 / 60)

int
main(int argc, char **argv)
{

    srand(time(NULL));

    //SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_DEBUG);

    /* configurables */
    uint16_t rate = DEFAULT_IPS;
    int *opt = malloc(sizeof *opt);
    int *longOptIndex  = calloc(1, sizeof *longOptIndex);
    SDL_bool *mute = calloc(1, sizeof *mute); // audio is muted? (-m)
    SDL_bool *force = calloc(1, sizeof *force); // rom is force loaded? (-f)

    /* args */
    while (
        argc > 1
        &&
        (*opt = getopt_long(argc, argv,  "fmi:hv", longOptions, longOptIndex)) != -1
    ) {
        switch (*opt) {
            case 'f':   // force
                *force = SDL_TRUE;
                break;
            case 'm':   // mute
                *mute = SDL_TRUE;
                break;
            case 'i':   // ips
                if (isNumber(optarg)) rate = atoi(optarg);
                else {
                    SDL_LogError(
                        SDL_LOG_CATEGORY_APPLICATION,
                        "invalid IPS input\n"
                    );

                    free(opt);
                    free(longOptIndex);
                    free(mute);
                    free(force);

                    return -1;
                }

                if (rate <= 0) rate = DEFAULT_IPS;

                break;
            case 'h':   // help
                printUsage(argv[0], SDL_LOG_PRIORITY_INFO);

                return 0;
            case 'v':   // version
                printVersion(argv[0]);

                return 0;
            default:    // '?'
                printUsage(argv[0], SDL_LOG_PRIORITY_ERROR);

                free(opt);
                free(longOptIndex);
                free(mute);
                free(force);

                return -1;
        }
    }

    free(opt);
    free(longOptIndex);

    /* ensure that a ROM argument was given */
    if (optind >= argc) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "expected ROM argument after options\n"
        );

        free(mute);
        free(force);

        return -1;
    }

    const char *inputFile = argv[optind];
    FILE *rom = getRom(inputFile);
    struct stat st;

    if (!force && !isRomValid(inputFile, rom, &st)) {
        if (rom != NULL) fclose(rom);

        free(mute);
        free(force);

        return -1;      // error has already been logged
    } else if (force) {
        if (rom == NULL) {
            SDL_LogError(
                SDL_LOG_CATEGORY_APPLICATION,
                "failed to open ROM file: %s\n",
                inputFile
            );

            free(mute);
            free(force);

            return -1;
        }

        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "force loading %s\n",
            inputFile
        );
    } else SDL_LogDebug(
        SDL_LOG_CATEGORY_APPLICATION,
        "loading %s\n",
        inputFile
    );

    free(force);

    emulator chip8;
    initializeEmulator(&chip8, rom);
    chip8.muted = *mute;
    free(mute);

    if (chip8.muted)
        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "emulator initialized with %s audio\n",
            chip8.muted ? "muted" : "unmuted"
        );

#if defined(__APPLE__)

    char *bin = getExecutablePathMACOS();

#elif defined(__linux__)

    char *bin = getExecutablePathLINUX();

#endif

    const char *iconPath = getWindowIconPath(bin);

    if (initDisplay(&chip8.display, iconPath) != 0) {
        free((void *)bin);
        free((void *)iconPath);

        return -1;      // error has already been logged
    } else {
        free((void *)bin);
        free((void *)iconPath);
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
    const double msPerInstruction = 1000.0 / rate;
    double nextInstructionTime = SDL_GetTicks();

    /* main loop */
    while (chip8.display.poweredOn) {

        /* timers */
        ticks = SDL_GetTicks();

        if ((double)ticks < nextInstructionTime) continue;

        nextInstructionTime += msPerInstruction;

        if (ticks - chip8.timers.lastUpdate >= TIMER_DECREMENT_INTERVAL_MS) {
            if (chip8.timers.delay > 0) chip8.timers.delay--;

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
                chip8.sound.phase = 0.0;
                SDL_PauseAudioDevice(chip8.sound.deviceId, 1);
            }

            chip8.timers.lastUpdate = ticks;
        } else if (ticks < chip8.timers.lastUpdate)
            chip8.timers.lastUpdate = ticks;

        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "timers updated\n"
        );

        /* events */
        clearKeys(chip8.display.keyUp);

        SDL_Event event;
        while (SDL_PollEvent(&event))
            handleEvent(&chip8.display, &event);

        SDL_LogDebug(
            SDL_LOG_CATEGORY_APPLICATION,
            "events handled\n"
        );

        if (chip8.display.reset) {
            FILE *rom = getRom(inputFile);

            if (rom != NULL) {
                initializeEmulator(&chip8, rom);
                fclose(rom);
            } else {
                SDL_LogError(
                    SDL_LOG_CATEGORY_APPLICATION,
                    "failed to reopen rom\n"
                );

                break;
            }

            resetDisplay(&chip8.display);
            chip8.display.reset = SDL_FALSE;
            nextInstructionTime = SDL_GetTicks();

            continue;
        }

        /* instructions */
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
            "%x successfully executed\n",
            opcode
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

    } // end of main loop

    cleanUp(&chip8);

    return 0;
}
