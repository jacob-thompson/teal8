#ifndef TIMERS_H
#define TIMERS_H

#include <stdint.h>

typedef struct {
    uint8_t     delay;                  // delay timer
    uint8_t     sound;                  // sound timer
    uint32_t    lastUpdate;             // tick count at last update
} timers;

#endif /* TIMERS_H */
