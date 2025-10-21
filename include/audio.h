#ifndef AUDIO_H
#define AUDIO_H

#include <SDL_audio.h>

typedef struct audio {
    SDL_AudioDeviceID deviceId;         // audio device ID
    SDL_AudioSpec spec;                 // audio specification
    SDL_bool playing;                   // is audio playing?
    SDL_bool poweredOn;                 // power flag
    double phase;                       // phase for waveform generation
} audio;

/*
 * Audio callback function to generate a tone.
 * Parameter: audio structure
 * Parameter: audio buffer to fill
 * Parameter: length of the audio buffer in bytes
 */
void audioCallback(void *userdata, Uint8 *stream, int len);

/*
 * Initialize the audio system.
 * Parameter: audio structure
 * Return: 0 on success
 * Return: -1 on failure
 */
int initAudio(audio *audio);

#endif /* AUDIO_H */
