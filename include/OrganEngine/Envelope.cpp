#include "Envelope.h"

EnvelopeSettings envelopeSettings {
    attackRate: (int16_t) (30 * MAX_AMPLITUDE / SAMPLE_RATE),
    decayRate: (int16_t) (10 * MAX_AMPLITUDE/ SAMPLE_RATE),
    sustainLevel: (int16_t) (0.8 * MAX_AMPLITUDE),
    releaseRate: (int16_t) (30 * MAX_AMPLITUDE/ SAMPLE_RATE),
};

void Envelope::noteOn()
{
    state = ADSR_ATTACK;
}

void Envelope::noteOff()
{
    state = ADSR_RELEASE;
}
