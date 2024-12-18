#include "../include/emulator.h"

FILE *getRom(const char *rom)
{
    char *filename = malloc(sizeof(char) * 50);
    FILE *rom_file = NULL;

    sprintf(filename, "roms/%s.c8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    sprintf(filename, "roms/%s.ch8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    sprintf(filename, "roms/%s.chip8", rom);
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
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return rom_file;
    }

    return NULL;
}

void writeFontToMemory(unsigned char *memory)
{
    unsigned char font[FONT_IN_BYTES] = {
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

int stacked(stack *stack)
{
    int count = 0;
    for (int i = 0; stack->s[i] != '\0'; i++)
        count++;
    return count;
}

void stackPush(stack *stack, unsigned short *value)
{
    if (stacked(stack) < STACK_SIZE) {
        stack->s[stacked(stack)] = *value;
        stack->sp++;
    }
    else {
        fprintf(stderr, "Stack overflow\n");
    }
}

void stackPop(stack *stack, unsigned short *poppedValue)
{
    if (stacked(stack) > 0) {
        stack->sp--;
        *poppedValue = stack->s[stacked(stack) - 1];
        stack->s[stacked(stack)] = '\0';
    }
    else {
        fprintf(stderr, "Stack underflow\n");
    }
}
