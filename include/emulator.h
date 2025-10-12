#ifndef EMULATOR_H
#define EMULATOR_H

#include <getopt.h>

#include <SDL_log.h>

#include "../include/audio.h"
#include "../include/display.h"
#include "../include/stack.h"
#include "../include/timers.h"

#define VERSION "1.0.0"

#define MEMORY_IN_BYTES 4096 // 4KB memory

#define FONT_START_ADDRESS 0x000 // 0 decimal
#define FONT_BYTES 0x50 // 80 decimal

#define PROGRAM_START_ADDRESS 0x200 // 512 decimal

#define DEFAULT_INSTRUCTION_RATE 1000 // 1000 instructions per second

#define TIMER_INTERVAL (1000 / 60) // ~16.67ms for 60Hz timer update
#define VBLANK_INTERVAL (1000 / 60)  // ~16.67ms for 60Hz vertical blank

#define REGISTERS 16 // 16 8-bit registers (V0 to VF)
#define LAST_REGISTER 0xF // index of the last register (VF)

#define CHIP8 100
#define SCHIP 101

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

typedef struct emulator {
    uint8_t memory[MEMORY_IN_BYTES]; // 4KB memory
    uint8_t v[REGISTERS]; // 16 8-bit registers
    uint8_t specType; // chip8 or schip
    uint16_t ix; // 16-bit index register
    uint16_t pc; // program counter
    uint32_t lastUpdate; // last update tick
    timers timers; // delay & sound timers
    stack stack; // stack & stack pointer
    display display; // display structure
    audio sound; // sound structure
    bool muted; // is the sound muted?
} emulator;

/*
 * Print the version of the program.
 * @param programName the name of the program
 */
void printVersion(const char *programName);

/*
 * Print the usage of the program.
 * @param programName the name of the program
 * @param priority the log priority to use
 */
void printUsage(const char *programName, SDL_LogPriority priority);

/*
 * Check if a string is a number.
 * @param num the string to check
 * @return true if the string is a number
 * @return false if the string is not a number
 */
bool isNumber(const char num[]);

/*
 * Get the rom file.
 * @param rom the rom file
 * @return the rom file
 * @return NULL if the rom file could not be opened
 */
FILE *getRom(const char *rom);

/*
 * Write the font to the memory of the emulator.
 * font data is written into memory between 0x050 and 0x09F.
 * @param memory the memory of the emulator
 */
void writeFontToMemory(unsigned char *memory);

/*
 * Write the rom to the memory of the emulator.
 * The program counter is set to 0x200.
 * @param chip8 the emulator
 * @param rom the rom file
 */
void writeRomToMemory(emulator *chip8, FILE *rom);

/*
 * Initialize the emulator.
 * @param chip8 the emulator
 * @param rom the rom file
 */
void initializeEmulator(emulator *chip8, FILE *rom);

/*
 * Print the memory of the emulator.
 * For debugging purposes.
 * @param chip8 the emulator
 */
//void printMemory(emulator *chip8);

/*
 * Generate a random number between min and max.
 * @param min the minimum number
 * @param max the maximum number
 * @return the random number
 */
int randomNumber(int min, int max);

/*
 * Fetch the opcode from the memory of the emulator.
 * The opcode is 2 bytes long.
 * @param chip8 the emulator
 * @return the opcode
 */
uint16_t fetchOpcode(emulator *chip8);

/*
 * Decode the opcode.
 * @param chip8 the emulator
 * @param opcode the opcode
 */
void decodeAndExecuteOpcode(emulator *chip8, unsigned short opcode);

#endif /* EMULATOR_H */
