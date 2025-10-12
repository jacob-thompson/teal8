#include <stdint.h>

#define STACK_SIZE 16

typedef struct stack {
    uint16_t s[STACK_SIZE]; // 16 16-bit registers
    uint8_t sp; // stack pointer index (0x0-0xF)
} stack;

/*
 * Get the number of stacked addresses.
 * @param stack the stack
 * @return the number of stacked addresses
 */
int stacked(stack *stack);

/*
 * Push a value onto the stack.
 * @param stack the stack
 * @param value the value to push onto the stack
 */
void stackPush(stack *stack, unsigned short *value);

/*
 * Pop a value from the stack.
 * @param stack the stack
 * @param poppedValue value to store popped address
 */
void stackPop(stack *stack, unsigned short *poppedValue);
