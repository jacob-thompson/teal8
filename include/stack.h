#ifndef STACK_H
#define STACK_H

#include <stdint.h>

#define STACK_LEVELS 16

typedef struct {
    uint16_t    s[STACK_LEVELS];    // 16 addresses
    uint8_t     sp;                 // stack pointer index (0x0-0xF)
} stack;

/*
 * Get the number of stacked addresses.
 *
 * Parameter:
 * the stack
 *
 * Return:
 * the number of stacked addresses
 */
int
stacked(stack *stack);

/*
 * Push a value onto the stack.
 *
 * Parameters:
 * the stack,
 * the value to push onto the stack
 */
void
stackPush(stack *stack, const uint16_t *value);

/*
 * Pop a value from the stack.
 *
 * Parameters:
 * the stack,
 * the value to be used to store popped address
 */
void
stackPop(stack *stack, uint16_t *poppedValue);

#endif /* STACK_H */
