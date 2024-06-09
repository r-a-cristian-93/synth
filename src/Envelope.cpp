#include "Envelope.h"

EnvelopeSettings envelope_settings;

void EnvelopeAdsr_Init() {
    envelope_settings.attackRate = 50.0 / SAMPLE_RATE;
    envelope_settings.decayRate = 30.0 / SAMPLE_RATE;
    envelope_settings.releaseRate = 30.0 / SAMPLE_RATE;
    envelope_settings.sustainLevel = 0.8;
}

float EnvelopeAdsr_GetAmplitude(EnvelopeAdsr* envelope)
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

void EnvelopeAdsr_NoteOn(EnvelopeAdsr* envelope)
{
    envelope->state = ADSR_ATTACK;
}

void EnvelopeAdsr_NoteOff(EnvelopeAdsr* envelope)
{
    envelope->state = ADSR_RELEASE;
}
