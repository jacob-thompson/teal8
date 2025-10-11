#include <stdbool.h>

#include <SDL.h>

#define TONE_FREQ 440 // A4 tone
#define AMPLITUDE 1000 // volume of the tone
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096

typedef struct audio {
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec spec;
    bool playing;
    bool poweredOn;
    double phase;
} audio;

void my_audio_callback(void *userdata, Uint8 *stream, int len);

int initAudio(audio *audio);
