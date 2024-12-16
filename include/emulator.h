#include <stdint.h>

#define MEMORY_IN_BYTES 4096
#define FONT_IN_BYTES 80
#define FONT_START_ADDRESS 0x050
#define FONT_END_ADDRESS 0x09F
#define STACK_SIZE 16

typedef struct {
    unsigned char memory[MEMORY_IN_BYTES]; // 4KB memory
    unsigned char stack[STACK_SIZE]; // 16 16-bit registers
    unsigned short ix; // 16-bit register
    unsigned short pc; // program counter
    unsigned char sp; // stack pointer
    int8_t dtimer; // delay timer
    int8_t stimer; // sound timer
} chip8;

void write_font_to_memory(unsigned char* memory);
