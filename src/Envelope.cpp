#include "Envelope.h"

EnvelopeSettings envelope_settings;

float EnvelopeAdsr_GetAmplitude(EnvelopeAdsr* envelope)
{
    switch (envelope->state) {
        case ADSR_IDLE:
            break;
        case ADSR_ATTACK:
            envelope->amplitudeValue += envelope_settings.attackRate;
            if (envelope->amplitudeValue >= 1.0) {
                envelope->amplitudeValue = 1.0;
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
            if (envelope->amplitudeValue <= 0.0) {
                envelope->amplitudeValue = 0.0;
                envelope->state = ADSR_IDLE;
            }
    }

    return envelope->amplitudeValue;
}

void EnvelopeAdsr_NoteOn(EnvelopeAdsr* envelope)
{
    envelope->state = ADSR_ATTACK;
}

void EnvelopeAdsr_NoteOff(EnvelopeAdsr* envelope)
{
    envelope->state = ADSR_RELEASE;
}
