#include "../include/emulator.h"

FILE *verified_rom(FILE *rom)
{
    // a valid chip8 rom starts with 0x00E0
    unsigned char buffer[2];
    fread(buffer, 1, 2, rom);
    if (buffer[0] != 0x00 || buffer[1] != 0xE0)
        return NULL;
    rewind(rom);

    return rom;
}

FILE *get_rom(const char *rom)
{
    char *filename = malloc(sizeof(char) * 50);
    FILE *rom_file = NULL;

    sprintf(filename, "roms/%s.c8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return verified_rom(rom_file);
    }

    sprintf(filename, "roms/%s.ch8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return verified_rom(rom_file);
    }

    sprintf(filename, "roms/%s.chip8", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return verified_rom(rom_file);
    }

    sprintf(filename, "roms/%s", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return verified_rom(rom_file);
    }

    sprintf(filename, "%s", rom);
    rom_file = fopen(filename, "rb");
    if (rom_file != NULL) {
        free(filename);
        return verified_rom(rom_file);
    }

    return NULL;
}

void write_font_to_memory(unsigned char *memory)
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

void write_rom_to_memory(emulator *chip8, FILE *rom)
{
    chip8->pc = PROGRAM_START_ADDRESS;
    while (fread(&chip8->memory[chip8->pc], 1, 1, rom) == 1)
        chip8->pc++;
    chip8->pc = PROGRAM_START_ADDRESS;
}

void print_memory(emulator *chip8)
{
    for (int i = 0; i < MEMORY_IN_BYTES; i++) {
        if (i % 16 == 0)
            printf("\n");
        printf("%02X ", chip8->memory[i]);
    }
    printf("\n");
}
