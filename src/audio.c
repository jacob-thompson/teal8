#include <time.h>
#include <math.h>

#include <SDL.h>

#include "../include/audio.h"

#define TONE_FREQ       440                 // A440 tone
#define AMPLITUDE       1000                // volume of the tone
#define SAMPLE_RATE     44100               // samples per second
#define BUFFER_SIZE     512                 // audio buffer size

void
audioCallback(void *userdata, Uint8 *stream, int len)
{
    audio           *aud            = (audio *)userdata;
    Sint16          *samples        = (Sint16 *)stream;
    const double    phaseIncrement  = 2.0 * M_PI * TONE_FREQ / SAMPLE_RATE;
    const int       sampleCount     = len / sizeof(Sint16);

    for (int i = 0; i < sampleCount; ++i) {
        /* square wave for beep tone */
        samples[i] = sin(aud->phase) > 0 ? AMPLITUDE : -AMPLITUDE;
        aud->phase += phaseIncrement;
        if (aud->phase >= 2.0 * M_PI) {
            aud->phase -= 2.0 * M_PI;
        }
    }
}

int
initAudio(audio *audio)
{
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        audio->poweredOn = SDL_FALSE;
        return -1;
    }

    SDL_zero(audio->spec);
    audio->spec.freq        = SAMPLE_RATE;
    audio->spec.format      = AUDIO_S16SYS;
    audio->spec.channels    = 1;
    audio->spec.samples     = BUFFER_SIZE;
    audio->spec.callback    = audioCallback;
    audio->spec.userdata    = audio;
    audio->deviceId         = SDL_OpenAudioDevice(NULL, 0, &audio->spec, NULL, 0);

    if (audio->deviceId == 0) {
        audio->poweredOn = SDL_FALSE;
        return -1;
    }

    audio->poweredOn    = SDL_TRUE;
    audio->playing      = SDL_FALSE;
    audio->phase        = 0.0;

    return 0;
}
