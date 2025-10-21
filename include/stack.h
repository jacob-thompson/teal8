#ifndef STACK_H
#define STACK_H

#include <stdint.h>

#define STACK_LEVELS 16             // 16 levels of stack

typedef struct {
    uint16_t    s[STACK_LEVELS];    // 16 addresses
    uint8_t     sp;                 // stack pointer index (0x0-0xF)
} stack;

/*
 * Get the number of stacked addresses.
 * Parameter: the stack
 * Return: the number of stacked addresses
 */
int
stacked(stack *stack);

/*
 * Push a value onto the stack.
 * Parameter: the stack
 * Parameter: the value to push onto the stack
 */
void
stackPush(stack *stack, const unsigned short *value);

/*
 * Pop a value from the stack.
 * Parameter: the stack
 * Parameter: the value to be used to store popped address
 */
void
stackPop(stack *stack, unsigned short *poppedValue);

#endif /* STACK_H */
