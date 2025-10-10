#include <stdbool.h>

#include <SDL.h>

#define TONE_FREQ 440 // A4 tone
#define AMPLITUDE 1000 // volume of the tone

typedef struct audio {
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec spec;
    bool poweredOn;
} audio;

void my_audio_callback(void *userdata, Uint8 *stream, int len);

int initAudio(audio *audio);
