#include <stdio.h>

#include "../include/stack.h"

int stacked(stack *stack)
{
    int count = 0;
    for (int i = 0; stack->s[i] != '\0'; i++)
        count++;
    return count;
}

void stackPush(stack *stack, unsigned short *value)
{
    if (stacked(stack) < STACK_SIZE) {
        stack->s[stacked(stack)] = *value;
        stack->sp++;
    }
    else {
        fprintf(stderr, "Stack overflow\n");
    }
}

void stackPop(stack *stack, unsigned short *poppedValue)
{
    if (stacked(stack) > 0) {
        stack->sp--;
        *poppedValue = stack->s[stacked(stack) - 1];
        stack->s[stacked(stack)] = '\0';
    }
    else {
        fprintf(stderr, "Stack underflow\n");
    }
}
