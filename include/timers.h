#include <stdint.h>

typedef struct timers {
    uint8_t delay;
    uint8_t sound;
    uint32_t lastUpdate;
} timers;
