#ifndef EMULATOR_H
#define EMULATOR_H

#include <getopt.h>

#include <SDL_log.h>

#include "../include/audio.h"
#include "../include/display.h"
#include "../include/stack.h"
#include "../include/timers.h"

#define AMOUNT_MEMORY_BYTES 0x1000

#define AMOUNT_REGISTERS    16

#define DEFAULT_IPS         1000

#define CHIP8               100
#define SCHIP               101

/* long options for getopt_long */
static struct option longOptions[] =
{
    {"force", no_argument, NULL, 'f'},
    {"mute", no_argument, NULL, 'm'},
    {"ips", required_argument, NULL, 'i'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'v'},
    {0, 0, 0, 0} // end of array
};

typedef struct {
    uint8_t     memory[AMOUNT_MEMORY_BYTES];    // 4KB memory
    uint8_t     v[AMOUNT_REGISTERS];            // 16 8-bit registers
    uint8_t     specType;                       // chip8 or schip
    uint16_t    i;                              // 16-bit address register
    uint16_t    pc;                             // program counter
    uint32_t    lastUpdate;                     // last update tick
    timers      timers;                         // delay & sound timers
    stack       stack;                          // stack & stack pointer
    display     display;                        // display structure
    audio       sound;                          // sound structure
    SDL_bool    muted;                          // is the sound muted?
} emulator;

/*
 * Print the version of the program.
 * Parameter: the name of the program
 */
void
printVersion(const char *programName);

/*
 * Print the usage of the program.
 * Parameter: the name of the program
 * Parameter: the log priority to use
 */
void
printUsage(const char *programName, const SDL_LogPriority priority);

/*
 * Check if a string is a number.
 * Parameter: the string to check
 * Return: true if the string is a number
 * Return: false if the string is not a number
 */
SDL_bool
isNumber(const char num[]);

/*
 * Get the rom file.
 * Parameter: string representing the path to the rom file
 * Return: the rom file
 * Return: NULL if the rom file could not be opened
 */
FILE *
getRom(const char *rom);

/*
 * Write the font to the memory of the emulator.
 * Font data is written into memory between 0x00 and 0x50.
 * Parameter: the memory of the emulator
 */
void
writeFontToMemory(uint8_t *memory);

/*
 * Write the rom to the memory of the emulator.
 * The program counter is set to 0x200.
 * Parameter: the emulator
 * Parameter: the rom file
 */
void
writeRomToMemory(emulator *chip8, FILE *rom);

/*
 * Initialize the emulator.
 * Parameter: the emulator
 * Parameter: the rom file
 */
void
initializeEmulator(emulator *chip8, FILE *rom);

/*
 * Print the memory of the emulator.
 * For debugging purposes.
 * Parameter: the emulator
 */
//void printMemory(emulator *chip8);

/*
 * Generate a random number between min and max.
 * Parameter: the minimum number
 * Parameter: the maximum number
 * Return: the random number
 */
int
randomNumber(int min, int max);

/*
 * Fetch the opcode from the memory of the emulator.
 * The opcode is 2 bytes long.
 * Parameter: the emulator
 * Return: the opcode
 */
uint16_t
fetchOpcode(emulator *chip8);

/*
 * Decode the opcode.
 * Parameter: the emulator
 * Parameter: the opcode
 */
void
decodeAndExecuteOpcode(emulator *chip8, const uint16_t opcode);

#endif /* EMULATOR_H */
