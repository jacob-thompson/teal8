#include <stdint.h>
#include <stdlib.h>

#include "../include/display.h"
#include "../include/stack.h"
#include "../include/timers.h"

#define MEMORY_IN_BYTES 4096
#define FONT_IN_BYTES 80

#define FONT_START_ADDRESS 0x050
#define FONT_END_ADDRESS 0x09F
#define PROGRAM_START_ADDRESS 0x200

#define REGISTERS 16

/**
    * The emulator struct.
    * This struct contains the memory, registers, index register, program counter, timers, stack, and display.
*/
typedef struct {
    uint8_t memory[MEMORY_IN_BYTES]; // 4KB memory
    uint8_t v[REGISTERS]; // 16 8-bit registers
    unsigned short ix; // 16-bit index register
    unsigned short pc; // program counter
    timers timers; // delay and sound timers
    stack stack; // stack
    display display; // display
} emulator;

/**
    * Get the rom file.
    * @param rom the rom file
    * @return the rom file
    * @return NULL if the rom file could not be opened
    * @return NULL if the rom file is NULL
    * @return NULL if the rom file is not a valid chip8 rom
*/
FILE *getRom(const char *rom);

/**
    * Initialize the emulator.
    * @param chip8 the emulator
*/
void initializeEmulator(emulator *chip8, FILE *rom);

/**
    * Write the font to the memory of the emulator.
    * font data is written into memory between 0x050 and 0x09F.
    * @param memory the memory of the emulator
*/
void writeFontToMemory(unsigned char *memory);

/**
    * Write the rom to the memory of the emulator.
    * The program counter is set to 0x200.
    * @param chip8 the emulator
    * @param rom the rom file
*/
void writeRomToMemory(emulator *chip8, FILE *rom);

/**
    * Print the memory of the emulator.
    * For debugging purposes.
    * @param chip8 the emulator
*/
void printMemory(emulator *chip8);

/**
    * Generate a random number between min and max.
    * @param min the minimum number
    * @param max the maximum number
    * @return the random number
*/
int randomNumber(int min, int max);

/**
    * Fetch the opcode from the memory of the emulator.
    * The opcode is 2 bytes long.
    * @param chip8 the emulator
    * @return the opcode
*/
unsigned short fetchOpcode(emulator *chip8);

/**
    * Decode the opcode.
    * @param chip8 the emulator
    * @param opcode the opcode
*/
void decodeOpcode(emulator *chip8, unsigned short opcode);
