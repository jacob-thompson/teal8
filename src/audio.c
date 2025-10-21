#include <time.h>
#include <math.h>

#include <SDL.h>

#include "../include/audio.h"

#define AUDIO_TONE_FREQ     440
#define AUDIO_VOLUME        1000
#define AUDIO_SAMPLE_RATE   44100
#define AUDIO_BUFFER_SIZE   512

void
audioCallback(void *userdata, Uint8 *stream, int len)
{
    audio           *aud            = (audio *)userdata;
    Sint16          *samples        = (Sint16 *)stream;
    const double    phaseIncrement  = 2 * M_PI * AUDIO_TONE_FREQ / AUDIO_SAMPLE_RATE;
    const int       sampleCount     = len / sizeof(Sint16);

    for (int i = 0; i < sampleCount; ++i) {
        /* square wave for beep tone */
        samples[i] = sin(aud->phase) > 0 ? AUDIO_VOLUME : -AUDIO_VOLUME;
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
    audio->spec.freq        = AUDIO_SAMPLE_RATE;
    audio->spec.format      = AUDIO_S16SYS;
    audio->spec.channels    = 1;
    audio->spec.samples     = AUDIO_BUFFER_SIZE;
    audio->spec.callback    = audioCallback;
    audio->spec.userdata    = audio;
    audio->deviceId         = SDL_OpenAudioDevice(NULL, 0, &audio->spec, NULL, 0);

    if (audio->deviceId == 0) {
        audio->poweredOn = SDL_FALSE;
        return -1;
    }

    audio->poweredOn        = SDL_TRUE;
    audio->playing          = SDL_FALSE;
    audio->phase            = 0.0;

    return 0;
}
