#include "../include/stack.h"

int stacked(stack *stack)
{
    return stack->sp;
}

void stackPush(stack *stack, unsigned short *value)
{
    if (stack->sp < STACK_LEVELS) {
        stack->s[stack->sp] = *value;
        stack->sp++;
    }
}

void stackPop(stack *stack, unsigned short *poppedValue)
{
    if (stack->sp > 0) {
        stack->sp--;
        *poppedValue = stack->s[stack->sp];
    }
}
