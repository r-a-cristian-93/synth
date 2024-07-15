#include "Envelope.h"

#ifndef SAMPLE_RATE
#define SAMPLE_RATE (44100)
#endif

#ifndef MAX_AMPLITUDE
#define MAX_AMPLITUDE (1.0)
#endif

#ifndef MIN_AMPLITUDE
#define MIN_AMPLITUDE (0.0)
#endif

EnvelopeSettings envelope_settings;

void envelope_initialize() {
    envelope_settings.attackRate = 50.0 / SAMPLE_RATE;
    envelope_settings.decayRate = 0.4 / SAMPLE_RATE;
    envelope_settings.releaseRate = 5.0 / SAMPLE_RATE;
    envelope_settings.sustainLevel = 0.0;
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
            envelope->amplitudeValue -= envelope_settings.decayRate;
            if (envelope->amplitudeValue <= envelope_settings.sustainLevel) {
                envelope->amplitudeValue = envelope_settings.sustainLevel;
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
