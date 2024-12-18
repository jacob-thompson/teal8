#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MEMORY_IN_BYTES 4096
#define FONT_IN_BYTES 80

#define FONT_START_ADDRESS 0x050
#define FONT_END_ADDRESS 0x09F
#define PROGRAM_START_ADDRESS 0x200

#define STACK_SIZE 16
#define REGISTERS 16

/**
    * The timers struct.
    * Contains the delay timer and the sound timer.
    * The delay timer is used for timing the events of games.
    * The sound timer is used for sound effects.
*/
typedef struct {
    int8_t delay;
    int8_t sound;
} timers;

/**
    * The stack struct.
    * Contains the stack and the stack pointer.
    * The stack pointer points to the top of the stack.
*/
typedef struct {
    unsigned short s[STACK_SIZE]; // 16 16-bit registers
    unsigned short *sp; // stack pointer
} stack;

/**
    * The emulator struct.
    * Contains the memory, stack, registers, program counter, stack pointer, delay timer, and sound timer.
*/
typedef struct {
    uint8_t memory[MEMORY_IN_BYTES]; // 4KB memory
    uint8_t v[REGISTERS]; // 16 8-bit registers
    unsigned short ix; // 16-bit index register
    unsigned short pc; // program counter
    timers timers; // delay and sound timers
    stack stack; // stack
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
    * Get the number of stacked addresses.
    * @param stack the stack
    * @return the number of stacked addresses
*/
int stacked(stack *stack);

/**
    * Push a value onto the stack.
    * @param s the stack
    * @param value the value to push onto the stack
*/
void stackPush(stack *stack, unsigned short *value);

/**
    * Pop a value from the stack.
    * @param s the stack
    * @param poppedValue value to store popped address
*/
void stackPop(stack *stack, unsigned short *poppedValue);
