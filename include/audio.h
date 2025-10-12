#ifndef AUDIO_H
#define AUDIO_H

#include <stdbool.h>

#include <SDL_audio.h>

#define TONE_FREQ 440 // A440 tone
#define AMPLITUDE 1000 // volume of the tone
#define SAMPLE_RATE 44100 // samples per second
#define BUFFER_SIZE 512 // audio buffer size

typedef struct audio {
    SDL_AudioDeviceID deviceId;
    SDL_AudioSpec spec;
    bool playing;
    bool poweredOn;
    double phase;
} audio;

/*
 * Audio callback function to generate a tone
 * @param userdata Pointer to audio structure
 * @param stream Pointer to the audio buffer to fill
 * @param len Length of the audio buffer
 */
void audioCallback(void *userdata, Uint8 *stream, int len);

/*
 * Initialize the audio system
 * @param audio Pointer to audio structure
 * @return 0 on success, -1 on failure
 */
int initAudio(audio *audio);

#endif /* AUDIO_H */
