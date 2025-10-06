#include <stdint.h>

#define STACK_SIZE 16

/**
    * The stack struct.
    * Contains the stack and the stack pointer.
    * The stack pointer points to the top of the stack.
*/
typedef struct {
    uint16_t s[STACK_SIZE]; // 16 16-bit registers
    uint8_t sp; // stack pointer index (0-15)
} stack;

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
