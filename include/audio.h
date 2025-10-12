#include <stdbool.h>

#include <SDL.h>

#define TONE_FREQ 440 // A440 tone
#define AMPLITUDE 1000 // volume of the tone
#define SAMPLE_RATE 44100 // samples per second
#define BUFFER_SIZE 512 // audio buffer size

/* Audio structure to hold audio state */
typedef struct audio {
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec spec;
    bool playing;
    bool poweredOn;
    double phase;
} audio;

/*
 * Audio callback function to generate a tone
 */
void audioCallback(void *userdata, Uint8 *stream, int len);

/*
 * Initialize the audio system
 */
int initAudio(audio *audio);
