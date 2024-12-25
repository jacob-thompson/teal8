#include "../include/emulator.h"

uint16_t roundRate(int rate)
{
    if (rate <= 0 || rate > DEFAULT_INSTRUCTION_RATE)
        return DEFAULT_INSTRUCTION_RATE;

    uint8_t significant = 0;
    uint16_t divisors[12] = {1, 2, 5, 10, 20, 25, 50, 100, 200, 250, 500, 1000};
    for (int i = 0; i < 12; i++)
        if (rate == divisors[i])
            return rate;
        else if (rate < divisors[i] && !significant)
            significant = i;

    uint16_t lowerDiff = rate - divisors[significant - 1];
    uint16_t upperDiff = divisors[significant] - rate;
    return lowerDiff < upperDiff ? divisors[significant - 1] : divisors[significant];
}

FILE *getRom(const char *rom)
{
    char *filename = malloc(sizeof(char) * 64);
    FILE *rom_file = NULL;

    sprintf(filename, "roms/%s.ch8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    sprintf(filename, "roms/%s", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    sprintf(filename, "%s", rom);
    if ( // test roms
        strcmp(filename, "beep") == 0 ||
        strcmp(filename, "corax+") == 0 ||
        strcmp(filename, "keypad") == 0 ||
        strcmp(filename, "flags") == 0 ||
        strcmp(filename, "quirks") == 0 ||
        strcmp(filename, "splash") == 0 ||
        strcmp(filename, "ibm_logo") == 0
    ) {
        sprintf(filename, "roms/test/%s.ch8", rom);
        rom_file = fopen(filename, "rb");
        if (rom_file != NULL) {
            free(filename);
            return rom_file;
        }
    }
    if (strcmp(filename, "octojamtitle") == 0) {
        int randomTitleRom = randomNumber(1, 10);
        sprintf(filename, "roms/octojam%dtitle.ch8", randomTitleRom);
        rom_file = fopen(filename, "rb");
        if (rom_file != NULL) {
            free(filename);
            return rom_file;
        }
    }

    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    free(filename);
    return NULL;
}

void writeFontToMemory(unsigned char *memory)
{
    uint8_t font[FONT_IN_BYTES] = {
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

void writeRomToMemory(emulator *chip8, FILE *rom)
{
    chip8->pc = PROGRAM_START_ADDRESS;
    while (fread(&chip8->memory[chip8->pc], 1, 1, rom) == 1)
        chip8->pc++;
    chip8->pc = PROGRAM_START_ADDRESS;
}

void initializeEmulator(emulator *chip8, FILE *rom)
{
    writeFontToMemory(chip8->memory);
    writeRomToMemory(chip8, rom);

    clearKeys(chip8->display.keyDown);
    clearKeys(chip8->display.keyUp);

    chip8->lastUpdate = 0;
    chip8->timers.lastUpdate = 0;

    chip8->specType = CHIP8;
}

void printMemory(emulator *chip8)
{
    for (int i = 0; i < MEMORY_IN_BYTES; i++) {
        if (i % 16 == 0)
            fprintf(stdout, "\n");
        fprintf(stdout, "%02X ", chip8->memory[i]);
    }
    fprintf(stdout, "\n");
}

int randomNumber(int min, int max)
{
    if (min == max)
        return min;

    // swap the numbers if min is greater than max
    if (min > max) {
        min ^= max;
        max ^= min;
        min ^= max;
    }

    int range = max - min + 1;

    // chop off the high end of rand()
    // so that the entire range is evenly divided by range
    int x;
    while (1) {
        x = rand();
        if (x < RAND_MAX / range * range)
            break;
    }

    return x % range + min;
}

uint16_t fetchOpcode(emulator *chip8)
{
    return (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc + 1];
}

void decodeOpcode(emulator *chip8, unsigned short opcode)
{
    uint8_t minuend, subtrahend, operand, addend;

    uint8_t x = (opcode & 0x0F00) >> 8;
    uint8_t y = (opcode & 0x00F0) >> 4;
    uint8_t n = opcode & 0x000F;
    uint8_t nn = opcode & 0x00FF;
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode >> 12) {
        case 0x0:
            switch (y) {
                case 0xC:
                    // scroll the display N lines down
                    chip8->specType = SCHIP;
                    break;
            }
            switch (opcode & 0x00FF) {
                case 0xE0:
                    // clear the display
                    resetDisplay(&chip8->display);
                    break;
                case 0xEE:
                    // return from subroutine
                    stackPop(&chip8->stack, &chip8->pc);
                    break;
                case 0xFB:
                    // scroll the display 4 pixels to the right
                    chip8->specType = SCHIP;
                    break;
                case 0xFC:
                    // scroll the display 4 pixels to the left
                    chip8->specType = SCHIP;
                    break;
                case 0xFD:
                    // exit the interpreter
                    chip8->display.poweredOn = false;
                    break;
                case 0xFE:
                    // set the CHIP-8 display mode to 64x32
                    chip8->specType = SCHIP;
                    break;
                case 0xFF:
                    // set the CHIP-8 display mode to 128x64
                    chip8->specType = SCHIP;
                    break;
                default:
                    // call RCA 1802 program at address NNN
                    // this is not implemented on modern interpreters
                    break;
            }
            break;
        case 0x1:
            // jump to address NNN
            chip8->pc = nnn;
            break;
        case 0x2:
            // call subroutine at address NNN
            stackPush(&chip8->stack, &chip8->pc);
            chip8->pc = nnn;
            break;
        case 0x3:
            // skip next instruction if Vx == NN
            if (chip8->v[x] == nn)
                chip8->pc += 2;
            break;
        case 0x4:
            // skip next instruction if Vx != NN
            if (chip8->v[x] != nn)
                chip8->pc += 2;
            break;
        case 0x5:
            // skip next instruction if Vx == Vy
            if (chip8->v[x] == chip8->v[y])
                chip8->pc += 2;
            break;
        case 0x6:
            // set Vx to NN
            chip8->v[x] = nn;
            break;
        case 0x7:
            // add NN to Vx
            chip8->v[x] += nn;
            break;
        case 0x8:
            switch (opcode & 0x000F) {
                case 0x0:
                    // set Vx to Vy
                    chip8->v[x] = chip8->v[y];
                    break;
                case 0x1:
                    // set Vx to Vx OR Vy
                    // reset VF to 0
                    chip8->v[x] |= chip8->v[y];
                    chip8->v[0xF] = 0;
                    break;
                case 0x2:
                    // set Vx to Vx AND Vy
                    // reset VF to 0
                    chip8->v[x] &= chip8->v[y];
                    chip8->v[0xF] = 0;
                    break;
                case 0x3:
                    // set Vx to Vx XOR Vy
                    // reset VF to 0
                    chip8->v[x] ^= chip8->v[y];
                    chip8->v[0xF] = 0;
                    break;
                case 0x4:
                    // add Vy to Vx
                    // set VF to 1 if there is a carry
                    operand = chip8->v[x];
                    addend = chip8->v[y];
                    chip8->v[x] += chip8->v[y];
                    if (operand > 0xFF - addend)
                        chip8->v[0xF] = 1;
                    else
                        chip8->v[0xF] = 0;
                    break;
                case 0x5:
                    // subtract Vy from Vx
                    // set VF to 0 if there is a borrow
                    minuend = chip8->v[x];
                    subtrahend = chip8->v[y];
                    chip8->v[x] -= chip8->v[y];
                    if (minuend < subtrahend)
                        chip8->v[0xF] = 0;
                    else
                        chip8->v[0xF] = 1;
                    break;
                case 0x6:
                    // shift Vx right by 1
                    // set VF to the least significant bit of Vx before the shift
                    operand = chip8->v[x];
                    chip8->v[x] = chip8->v[y];
                    chip8->v[x] >>= 1;
                    chip8->v[0xF] = operand & 0x01;
                    break;
                case 0x7:
                    // set Vx to Vy - Vx
                    // set VF to 0 if there is a borrow
                    minuend = chip8->v[y];
                    subtrahend = chip8->v[x];
                    chip8->v[x] = minuend - subtrahend;
                    if (minuend < subtrahend)
                        chip8->v[0xF] = 0;
                    else
                        chip8->v[0xF] = 1;
                    break;
                case 0xE:
                    // shift Vx left by 1
                    // set VF to the least significant bit of Vx before the shift
                    operand = chip8->v[x];
                    chip8->v[x] = chip8->v[y];
                    chip8->v[x] <<= 1;
                    chip8->v[0xF] = operand >> 7;
                    break;
            }
            break;
        case 0x9:
            // skip next instruction if Vx != Vy
            if (chip8->v[x] != chip8->v[y])
                chip8->pc += 2;
            break;
        case 0xA:
            // set I to address NNN
            chip8->ix = nnn;
            break;
        case 0xB:
            // jump to address NNN + V0
            chip8->pc = nnn + chip8->v[0];
            break;
        case 0xC:
            // set Vx to a random number AND NN
            chip8->v[x] = randomNumber(0, 255) & nn;
            break;
        case 0xD: // DXYN
            // draw a sprite at position Vx, Vy
            // sprite is 0xN pixels tall
            // on/off based on value in I
            // set VF to 1 if any set pixels are changed to unset, 0 otherwise
            while (true) // wait for vertical blank interrupt
                if (chip8->display.lastUpdate + (1000 / 60) < SDL_GetTicks())
                    break;

            uint8_t spriteX = chip8->v[x] % CHIP8_WIDTH;
            uint8_t spriteY = chip8->v[y] % CHIP8_HEIGHT;
            uint8_t spriteHeight = n;

            chip8->v[0xF] = 0;

            for (int yline = 0; yline < spriteHeight; yline++) {
                if (spriteY + yline >= CHIP8_HEIGHT)
                    continue; // clip vertically

                uint8_t pixel = chip8->memory[chip8->ix + yline];
                for (int xline = 0; xline < 8; xline++) {
                    if ((pixel & (0x80 >> xline)) != 0) {
                        if (spriteX + xline >= CHIP8_WIDTH)
                            continue; // clip horizontally

                        if (chip8->display.pixelDrawn[(spriteY + yline) * CHIP8_WIDTH + (spriteX + xline)])
                            chip8->v[0xF] = 1;

                        chip8->display.pixelDrawn[(spriteY + yline) * CHIP8_WIDTH + (spriteX + xline)] ^= true;
                    }
                }
            }
            chip8->display.lastUpdate = SDL_GetTicks();
            break;
        case 0xE:
            switch (opcode & 0x00FF) {
                case 0x9E:
                    // skip next instruction if key with the value of Vx is pressed
                    if (chip8->display.keyDown[chip8->v[x]])
                        chip8->pc += 2;
                    break;
                case 0xA1:
                    // skip next instruction if key with the value of Vx is not pressed
                    if (!chip8->display.keyDown[chip8->v[x]])
                        chip8->pc += 2;
                    break;
            }
            break;
        case 0xF:
            switch (opcode & 0x00FF) {
                case 0x07:
                    // set Vx to the value of the delay timer
                    chip8->v[x] = chip8->timers.delay;
                    break;
                case 0x0A:
                    // wait for a key press
                    // store the value of the key in Vx
                    chip8->pc -= 2;
                    for (int i = 0x0; i <= 0xF; i++)
                        if (chip8->display.keyUp[i]) {
                            chip8->v[x] = i;
                            chip8->pc += 2;
                            break;
                        }

                    break;
                case 0x15:
                    // set the delay timer to Vx
                    chip8->timers.delay = chip8->v[x];
                    break;
                case 0x18:
                    // set the sound timer to Vx
                    chip8->timers.sound = chip8->v[x];
                    break;
                case 0x1E:
                    // add Vx to I
                    chip8->ix += chip8->v[x];
                    break;
                case 0x29:
                    // set I to the location of the sprite for the character in Vx
                    chip8->ix = chip8->v[x] * 5;
                    break;
                case 0x33:
                    // store the binary-coded base-10 representation of Vx in memory locations I, I+1, and I+2
                    chip8->memory[chip8->ix] = chip8->v[x] / 100;
                    chip8->memory[chip8->ix + 1] = (chip8->v[x] / 10) % 10;
                    chip8->memory[chip8->ix + 2] = chip8->v[x] % 10;
                    break;
                case 0x55:
                    // store V0 to Vx in memory starting at address I
                    for (int i = 0; i <= x; i++)
                        chip8->memory[chip8->ix + i] = chip8->v[i];
                    chip8->ix += x + 1;
                    break;
                case 0x65:
                    // fill V0 to Vx with values from memory starting at address I
                    for (int i = 0; i <= x; i++)
                        chip8->v[i] = chip8->memory[chip8->ix + i];
                    chip8->ix += x + 1;
            }
    }
}
