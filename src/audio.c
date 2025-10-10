#include <time.h>
#include <math.h>

#include "../include/audio.h"

void my_audio_callback(void *userdata, Uint8 *stream, int len) {
    static double phase = 0.0;
    double phase_inc = 2.0 * M_PI * TONE_FREQ / 44100.0;
    Sint16* samples = (Sint16*) stream;
    int sample_count = len / sizeof(Sint16);

    for (int i = 0; i < sample_count; ++i)
    {
        // Simple square wave for retro-computer tone
        samples[i] = (sin(phase) > 0 ? AMPLITUDE : -AMPLITUDE);
        phase += phase_inc;
        if (phase >= 2.0 * M_PI)
            phase -= 2.0 * M_PI;
    }
}

int initAudio(audio *audio) {
    if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
        audio->poweredOn = false;
        return -1;
    }

    SDL_zero(audio->spec);
    audio->spec.freq = 44100;
    audio->spec.format = AUDIO_S16SYS;
    audio->spec.channels = 1;
    audio->spec.samples = 4096;
    audio->spec.callback = my_audio_callback;
    audio->deviceId = SDL_OpenAudioDevice(NULL, 0, &audio->spec, NULL, 0);

    if (audio->deviceId == 0) {
        audio->poweredOn = false;
        return -1;
    }

    audio->poweredOn = true;
    return 0;
}
