#include "../include/stack.h"

int
stacked(stack *stack)
{
    return stack->sp;
}

void
stackPush(stack *stack, const uint16_t *value)
{
    if (stack->sp < STACK_LEVELS) {
        stack->s[stack->sp] = *value;
        stack->sp++;
    }
}

void
stackPop(stack *stack, uint16_t *poppedValue)
{
    if (stack->sp > 0) {
        stack->sp--;
        *poppedValue = stack->s[stack->sp];
    }
}
