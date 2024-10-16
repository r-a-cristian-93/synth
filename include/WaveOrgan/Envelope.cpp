#include "Envelope.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100.0)
#endif

#ifndef MAX_AMPLITUDE
#define MAX_AMPLITUDE (1.0)
#endif

#ifndef MIN_AMPLITUDE
#define MIN_AMPLITUDE (0.0)
#endif

#define MIN_RELEASE_RATE (20.0)
#define MAX_RELEASE_RATE (0.3)

EnvelopeSettings envelope_settings;

void envelope_initialize() {
    envelope_settings.attackRate = 100.0 / SAMPLE_RATE;
    envelope_settings.releaseRate = MIN_RELEASE_RATE / SAMPLE_RATE;
}

float envelope_get_amplitude(Envelope* envelope)
{
    switch (envelope->state) {
        case ADSR_IDLE:
            break;
        case ADSR_ATTACK:
            envelope->amplitudeValue += envelope_settings.attackRate;
            if (envelope->amplitudeValue >= MAX_AMPLITUDE) {
                envelope->amplitudeValue = MAX_AMPLITUDE;
                envelope->state = ADSR_DECAY;
            }
            break;
        case ADSR_DECAY:
            envelope->amplitudeValue -= envelope_settings.releaseRate;
            if (envelope->amplitudeValue <= MIN_AMPLITUDE) {
                envelope->amplitudeValue = MIN_AMPLITUDE;
                envelope->state = ADSR_SUSTAIN;
            }
            break;
        case ADSR_SUSTAIN:
            break;
        case ADSR_RELEASE:
            envelope->amplitudeValue -= envelope_settings.releaseRate;
            if (envelope->amplitudeValue <= MIN_AMPLITUDE) {
                envelope->amplitudeValue = MIN_AMPLITUDE;
                envelope->state = ADSR_IDLE;
            }
            break;
        default:
            break;
    }

    return envelope->amplitudeValue;
}

void envelope_note_on(Envelope* envelope)
{
    envelope->state = ADSR_ATTACK;
}

void envelope_note_off(Envelope* envelope)
{
    envelope->state = ADSR_RELEASE;
}

#include <math.h>
#include <iostream>
void envelope_set_release_rate(uint8_t midiParam)
{
    envelope_settings.releaseRate = (((float)midiParam  / 127.0 * (MAX_RELEASE_RATE - MIN_RELEASE_RATE)) + MIN_RELEASE_RATE) /  SAMPLE_RATE;
}
