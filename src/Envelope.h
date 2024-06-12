#ifndef ENVELOPE_ADSR_H
#define ENVELOPE_ADSR_H

#include "Config.h"

enum ADSR_STATE
{
    ADSR_IDLE = 0,
    ADSR_ATTACK,
    ADSR_DECAY,
    ADSR_SUSTAIN,
    ADSR_RELEASE
};

struct Envelope
{
    ADSR_STATE state = ADSR_ATTACK;
    float amplitudeValue = 0.0;
};

struct EnvelopeSettings
{
    float attackRate;
    float decayRate;
    float sustainLevel;
    float releaseRate;
};

extern EnvelopeSettings envelope_settings;

void envelope_initialize();
void envelope_note_on(Envelope *envelope);
void envelope_note_off(Envelope *envelope);

// Inline
float envelope_get_amplitude(Envelope *envelope);


__attribute__((always_inline)) inline
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

#endif