#include <stdint.h>
#include <stdio.h>

#define MEMORY_IN_BYTES 4096
#define FONT_IN_BYTES 80

#define FONT_START_ADDRESS 0x050
#define FONT_END_ADDRESS 0x09F
#define PROGRAM_START_ADDRESS 0x200

#define STACK_SIZE 16

/**
    * The emulator struct.
    * Contains the memory, stack, registers, program counter, stack pointer, delay timer, and sound timer.
*/
typedef struct {
    unsigned char memory[MEMORY_IN_BYTES]; // 4KB memory
    unsigned char stack[STACK_SIZE]; // 16 16-bit registers
    unsigned short ix; // 16-bit register
    unsigned short pc; // program counter
    unsigned char sp; // stack pointer
    int8_t dtimer; // delay timer
    int8_t stimer; // sound timer
} emulator;

/**
    * Write the font to the memory of the emulator.
    * font data is written into memory between 0x050 and 0x09F.
    * @param memory the memory of the emulator
*/
void write_font_to_memory(unsigned char* memory);

/**
    * Write the rom to the memory of the emulator.
    * The program counter is set to 0x200.
    * @param chip8 the emulator
    * @param rom the rom file
*/
void write_rom_to_memory(emulator* chip8, FILE* rom);

/**
    * Print the memory of the emulator.
    * For debugging purposes.
    * @param chip8 the emulator
*/
void print_memory(emulator* chip8);
