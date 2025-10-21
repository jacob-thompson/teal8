#include <stdio.h>
#include <string.h>

#include <SDL_log.h>
#include <SDL_timer.h>

#include "../include/emulator.h"

#define FONT_START_ADDRESS      0x00            // 0 decimal
#define AMOUNT_FONT_BYTES       0x50            // 80 decimal

#define PROGRAM_START_ADDRESS   0x200           // 512 decimal

#define VBLANK_INTERVAL         (1000 / 60)     // ~16.67ms for 60Hz

#define LAST_REGISTER_INDEX     0xF             // index of VF

void
printVersion(const char *programName)
{
    SDL_LogInfo(
        SDL_LOG_CATEGORY_APPLICATION,
        "%s version %s\n",
        programName,
        TEAL8VERSION
    );
}

void
printUsage(const char *programName, SDL_LogPriority priority)
{
    SDL_LogMessage(
        SDL_LOG_CATEGORY_APPLICATION,
        priority,
        "%s version %s\n"
        "usage:\t%s [-m|--mute] [-f|--force] [-i|--ips <number>] <rom>\n"
        "\t-m (--mute)\tmute audio\n"
        "\t-f (--force)\tforce load rom regardless of validity\n"
        "\t-i (--ips)\tinstructions per second (default: %d)\n"
        "\t<rom>\t\tchip8 rom path\n"
        "controls:\n"
        "\t1 2 3 4\n"
        "\tQ W E R\n"
        "\tA S D F\n"
        "\tZ X C V\n",
        programName,
        TEAL8VERSION,
        programName,
        DEFAULT_INSTRUCTION_RATE
    );
}

SDL_bool
isNumber(const char num[])
{
    /* check if string is empty */
    if (num == NULL || num[0] == '\0')
        return SDL_FALSE;

    /* check if each character is a numeral */
    for (int i = 0; num[i] != 0; ++i) {
        if (!isdigit(num[i]))
            return SDL_FALSE;
    }
    return SDL_TRUE;
}

FILE *
getRom(const char *rom)
{
    /*
     * allocate memory for the filename
     * and 4 additional bytes for appending
     * the file extension if necessary
     */
    char *filename = malloc(sizeof(char) * (strlen(rom) + 5));
    if (filename == NULL) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "failed to allocate memory for filename\n"
        );
        return NULL;
    }

    FILE *romFile = NULL; // file to be returned

    snprintf(filename, strlen(rom) + 1, "%s", rom);
    romFile = fopen(filename, "rb");
    if (romFile != NULL) {
        free(filename);
        return romFile;
    }

    /*
     * opening the file as given did not work,
     * so try appending the file extension
     */
    snprintf(filename, strlen(rom) + 5, "%s.ch8", rom);
    romFile = fopen(filename, "rb");
    if (romFile != NULL) {
        free(filename);
        return romFile;
    }

    /* nothing worked */
    free(filename);
    return romFile; // null
}

void
writeFontToMemory(unsigned char *memory)
{
    const uint8_t font[AMOUNT_FONT_BYTES] = {
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

    for (int i = 0x0; i < AMOUNT_FONT_BYTES; i++)
        memory[FONT_START_ADDRESS + i] = font[i];
}

void
writeRomToMemory(emulator *chip8, FILE *rom)
{
    chip8->pc = PROGRAM_START_ADDRESS;

    while (
        chip8->pc < AMOUNT_MEMORY_BYTES
        &&
        fread(&chip8->memory[chip8->pc], 1, 1, rom) == 1
    ) {
        chip8->pc++;
    }

    if (chip8->pc >= AMOUNT_MEMORY_BYTES) {
        SDL_LogWarn(
            SDL_LOG_CATEGORY_APPLICATION,
            "ROM too large, truncated at %d bytes\n",
            AMOUNT_MEMORY_BYTES - PROGRAM_START_ADDRESS
        );
    }

    chip8->pc = PROGRAM_START_ADDRESS; // 0x200
}

void
initializeEmulator(emulator *chip8, FILE *rom)
{
    writeFontToMemory(chip8->memory);
    writeRomToMemory(chip8, rom);

    clearKeys(chip8->display.keyDown);
    clearKeys(chip8->display.keyUp);

    chip8->lastUpdate           = 0;
    chip8->timers.lastUpdate    = 0;

    /* ensure registers and stack are cleared */
    memset(chip8->v, 0, sizeof chip8->v);
    memset(chip8->stack.s, 0, sizeof chip8->stack.s);
    chip8->stack.sp = 0;

    chip8->specType = CHIP8;
}

/*
void
printMemory(emulator *chip8)
{
    for (int i = 0x0; i < AMOUNT_MEMORY_BYTES; i++) {
        if (i % 0x10 == 0)
            fprintf(stdout, "\n");
        fprintf(stdout, "%02X ", chip8->memory[i]);
    }
    fprintf(stdout, "\n");
}
*/

int
randomNumber(int min, int max)
{
    if (min == max)
        return min;

    /* swap the numbers if min is greater than max */
    if (min > max) {
        min ^= max;
        max ^= min;
        min ^= max;
    }

    const int range = max - min + 1;

    /*
     * chop off the high end of rand()
     * so that the entire range is evenly divided by range
     */
    int x;
    while (1) {
        x = rand();
        if (x < RAND_MAX / range * range) break;
    }

    return x % range + min;
}

uint16_t
fetchOpcode(emulator *chip8)
{
    if (chip8->pc >= AMOUNT_MEMORY_BYTES - 1) {
        SDL_LogError(
            SDL_LOG_CATEGORY_APPLICATION,
            "program counter out of bounds: 0x%04X\n",
            chip8->pc
        );
        return 0;
    }
    return (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
}

void
decodeAndExecuteOpcode(emulator *chip8, unsigned short opcode)
{
    uint8_t minuend, subtrahend, operand, addend;

    const uint8_t     x   =   (opcode & 0x0F00) >> 8;
    const uint8_t     y   =   (opcode & 0x00F0) >> 4;
    const uint8_t     n   =   opcode & 0x000F;
    const uint8_t     nn  =   opcode & 0x00FF;
    const uint16_t    nnn =   opcode & 0x0FFF;

    switch (opcode >> 12) {
        case 0x0:
            switch (y) {
                case 0xC:
                    /* scroll the display N lines down */
                    chip8->specType = SCHIP;
                    break;
            }
            switch (opcode & 0x00FF) {
                case 0xE0:
                    /* clear the display */
                    resetDisplay(&chip8->display);
                    break;
                case 0xEE:
                    /* return from subroutine */
                    stackPop(&chip8->stack, &chip8->pc);
                    break;
                case 0xFB:
                    /* scroll the display 4 pixels to the right */
                    chip8->specType = SCHIP;
                    break;
                case 0xFC:
                    /* scroll the display 4 pixels to the left */
                    chip8->specType = SCHIP;
                    break;
                case 0xFD:
                    /* exit the interpreter */
                    chip8->display.poweredOn = SDL_FALSE;
                    break;
                case 0xFE:
                    /* set the CHIP-8 display mode to 64x32 */
                    if (chip8->display.width == SCHIP_WIDTH * SCALE && chip8->display.height == SCHIP_HEIGHT * SCALE) {
                        SDL_SetWindowSize(chip8->display.window, CHIP8_WIDTH * SCALE, CHIP8_HEIGHT * SCALE);
                        SDL_GetWindowSize(chip8->display.window, &chip8->display.width, &chip8->display.height);
                        SDL_SetWindowPosition(chip8->display.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        createPixels(&chip8->display);
                        SDL_LogInfo(
                            SDL_LOG_CATEGORY_APPLICATION,
                            "display mode switched to lo-res\n"
                        );
                    }
                    chip8->specType = SCHIP;
                    break;
                case 0xFF:
                    /* set the CHIP-8 display mode to 128x64 */
                    if (chip8->display.width == CHIP8_WIDTH * SCALE && chip8->display.height == CHIP8_HEIGHT * SCALE) {
                        SDL_SetWindowSize(chip8->display.window, SCHIP_WIDTH * SCALE, SCHIP_HEIGHT * SCALE);
                        SDL_GetWindowSize(chip8->display.window, &chip8->display.width, &chip8->display.height);
                        SDL_SetWindowPosition(chip8->display.window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
                        createPixels(&chip8->display);
                        SDL_LogInfo(
                            SDL_LOG_CATEGORY_APPLICATION,
                            "display mode switched to hi-res\n"
                        );
                    }
                    chip8->specType = SCHIP;
                    break;
                default:
                    /* call RCA 1802 program at address NNN */
                    break;
            }
            break;
        case 0x1:
            /* jump to address NNN */
            chip8->pc = nnn;
            break;
        case 0x2:
            /* call subroutine at address NNN */
            stackPush(&chip8->stack, &chip8->pc);
            chip8->pc = nnn;
            break;
        case 0x3:
            /* skip next instruction if Vx == NN */
            if (chip8->v[x] == nn)
                chip8->pc += 2;
            break;
        case 0x4:
            /* skip next instruction if Vx != NN */
            if (chip8->v[x] != nn)
                chip8->pc += 2;
            break;
        case 0x5:
            /* skip next instruction if Vx == Vy */
            if (chip8->v[x] == chip8->v[y])
                chip8->pc += 2;
            break;
        case 0x6:
            /* set Vx to NN */
            chip8->v[x] = nn;
            break;
        case 0x7:
            /* add NN to Vx */
            chip8->v[x] += nn;
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0:
                    /* set Vx to Vy */
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1:
                    /*
                     * set Vx to Vx OR Vy
                     * reset VF to 0
                     */
                    chip8->v[x] |= chip8->v[y];
                    if (chip8->specType == CHIP8)
                        chip8->v[0xF] = 0;
                    break;
                case 0x2:
                    /*
                     * set Vx to Vx AND Vy;
                     * reset VF to 0
                     */
                    chip8->v[x] &= chip8->v[y];
                    if (chip8->specType == CHIP8)
                        chip8->v[0xF] = 0;
                    break;
                case 0x3:
                    /*
                     * set Vx to Vx XOR Vy;
                     * reset VF to 0
                     */
                    chip8->v[x] ^= chip8->v[y];
                    if (chip8->specType == CHIP8)
                        chip8->v[0xF] = 0;
                    break;
                case 0x4:
                    /*
                     * add Vy to Vx;
                     * set VF to 1 if there is a carry
                     */
                    operand = chip8->v[x];
                    addend  = chip8->v[y];
                    chip8->v[x] += chip8->v[y];
                    if (operand > 0xFF - addend)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                case 0x5:
                    /*
                     * subtract Vy from Vx;
                     * set VF to 0 if there is a borrow
                     */
                    minuend     = chip8->v[x];
                    subtrahend  = chip8->v[y];
                    chip8->v[x] -= chip8->v[y];
                    if (minuend < subtrahend)
                        chip8->v[0xF] = 0;
                    else
                        chip8->v[0xF] = 1;
                    break;
                case 0x6:
                    /*
                     * shift Vx right by 1;
                     * set VF to the least significant bit of Vx before the shift
                     */
                    operand = chip8->v[x];
                    if (chip8->specType == CHIP8)
                        chip8->v[x] = chip8->v[y];
                    chip8->v[x] >>= 1;
                    chip8->v[0xF] = operand & 0x01;
                    break;
                case 0x7:
                    /*
                     * set Vx to Vy - Vx;
                     * set VF to 0 if there is a borrow
                     */
                    minuend     = chip8->v[y];
                    subtrahend  = chip8->v[x];
                    chip8->v[x] = minuend - subtrahend;
                    if (minuend < subtrahend)
                        chip8->v[0xF] = 0;
                    else
                        chip8->v[0xF] = 1;
                    break;
                case 0xE:
                    /*
                     * shift Vx left by 1;
                     * set VF to the least significant bit of Vx before the shift
                     */
                    operand = chip8->v[x];
                    if (chip8->specType == CHIP8)
                        chip8->v[x] = chip8->v[y];
                    chip8->v[x] <<= 1;
                    chip8->v[0xF] = operand >> 7;
                    break;
            }
            break;
        case 0x9:
            /* skip next instruction if Vx != Vy */
            if (chip8->v[x] != chip8->v[y])
                chip8->pc += 2;
            break;
        case 0xA:
            /* set I to address NNN */
            chip8->i = nnn;
            break;
        case 0xB:
            /*
             * jump to address NNN + V0;
             * on SCHIP, jump to XNN + vX
             */
            if (chip8->specType == CHIP8)
                chip8->pc = nnn + chip8->v[0];
            else
                chip8->pc = nnn + chip8->v[x];
            break;
        case 0xC:
            /* set Vx to a random number AND NN */
            chip8->v[x] = randomNumber(0, 255) & nn;
            break;
        case 0xD: // DXYN
            /*
             * draw a sprite at position Vx, Vy;
             * the sprite is 0xN pixels tall;
             * on/off based on value in I;
             * set VF to 1 if any set pixels are changed to unset, 0 otherwise
             */
            while (1) { // wait for vertical blank interrupt
                if (chip8->display.lastUpdate + VBLANK_INTERVAL < SDL_GetTicks())
                    break;
            }

            const uint8_t sX    = chip8->v[x] % chip8->display.pixelWidth;
            const uint8_t sY    = chip8->v[y] % chip8->display.pixelHeight;
            const uint8_t sH    = n;

            chip8->v[0xF]       = 0;

            for (int yline = 0; yline < sH; yline++) {
                if (sY + yline >= chip8->display.pixelHeight)
                    continue; // clip vertically

                if (chip8->i + yline >= AMOUNT_MEMORY_BYTES)
                    break; // prevent buffer overflow

                const uint8_t pixel = chip8->memory[chip8->i + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (sX + xline >= chip8->display.pixelWidth)
                            continue; // clip horizontally

                        if (chip8->display.pixelDrawn[(sY + yline) * chip8->display.pixelWidth + (sX + xline)])
                            chip8->v[0xF] = 1;

                        chip8->display.pixelDrawn[(sY + yline) * chip8->display.pixelWidth + (sX + xline)] ^= SDL_TRUE;
                        chip8->display.dirty = SDL_TRUE;
                    }
                }
            }
            chip8->display.lastUpdate = SDL_GetTicks();
            break;
        case 0xE:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    /* skip next instruction if key with the value of Vx is pressed */
                    if (chip8->display.keyDown[chip8->v[x]])
                        chip8->pc += 2;
                    break;
                case 0xA1:
                    /* skip next instruction if key with the value of Vx is not pressed */
                    if (!chip8->display.keyDown[chip8->v[x]])
                        chip8->pc += 2;
                    break;
            }
            break;
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x07:
                    /* set Vx to the value of the delay timer */
                    chip8->v[x] = chip8->timers.delay;
                    break;
                case 0x0A:
                    /*
                     * wait for a key press
                     * and store the value of the key in Vx
                     */
                    chip8->pc -= 2;
                    for (int i = 0x0; i <= LAST_REGISTER_INDEX; i++)
                        if (chip8->display.keyUp[i]) {
                            chip8->v[x] = i;
                            chip8->pc += 2;
                            break;
                        }

                    break;
                case 0x15:
                    /* set the delay timer to Vx */
                    chip8->timers.delay = chip8->v[x];
                    break;
                case 0x18:
                    /* set the sound timer to Vx */
                    chip8->timers.sound = chip8->v[x];
                    break;
                case 0x1E:
                    /* add Vx to I */
                    chip8->i += chip8->v[x];
                    break;
                case 0x29:
                    /* set I to the location of the sprite for the character in Vx */
                    chip8->i = (chip8->v[x] & 0x0F) * 5;
                    break;
                case 0x33:
                    /*
                     * store the binary-coded base-10 representation of Vx
                     * in memory locations I, I+1, and I+2
                     */
                    if (chip8->i < AMOUNT_MEMORY_BYTES)
                        chip8->memory[chip8->i] = chip8->v[x] / 100;
                    if (chip8->i + 1 < AMOUNT_MEMORY_BYTES)
                        chip8->memory[chip8->i + 1] = (chip8->v[x] / 10) % 10;
                    if (chip8->i + 2 < AMOUNT_MEMORY_BYTES)
                        chip8->memory[chip8->i + 2] = chip8->v[x] % 10;
                    break;
                case 0x55:
                    /* store V0 to Vx in memory starting at address I */
                    for (int i = 0; i <= x; i++) {
                        if (chip8->i + i < AMOUNT_MEMORY_BYTES)
                            chip8->memory[chip8->i + i] = chip8->v[i];
                    }
                    if (chip8->specType == CHIP8)
                        chip8->i += x + 1;
                    break;
                case 0x65:
                    /* fill V0 to Vx with values from memory starting at address I */
                    for (int i = 0; i <= x; i++) {
                        if (chip8->i + i < AMOUNT_MEMORY_BYTES)
                            chip8->v[i] = chip8->memory[chip8->i + i];
                    }
                    if (chip8->specType == CHIP8)
                        chip8->i += x + 1;
                    break;
                case 0x75:
                    /* store V0 to Vx in the RPL user flags */
                    chip8->specType = SCHIP;
                    break;
                case 0x85:
                    /* fill V0 to Vx with values from the RPL user flags */
                    chip8->specType = SCHIP;
                    break;
            }
    }
}
