#include <stdint.h>

/**
    * The timers struct.
    * Contains the delay timer and the sound timer.
    * The delay timer is used for timing the events of games.
    * The sound timer is used for sound effects.
*/
typedef struct {
    int8_t delay;
    int8_t sound;
} timers;
