#include "Envelope.h"

EnvelopeSettings envelopeSettings = {
    attackRate: (int32_t) (100 * MAX_AMPLITUDE / SAMPLE_RATE),
    decayRate: (int32_t) (200 * MAX_AMPLITUDE/ SAMPLE_RATE),
    sustainLevel: (int32_t) (0.8 * MAX_AMPLITUDE),
    releaseRate: (int32_t) (100 * MAX_AMPLITUDE / SAMPLE_RATE),
};

void Envelope::noteOn()
{
    if (state == ADSR_IDLE)
    {
        state = ADSR_ATTACK;
    }
}

void Envelope::noteOff()
{
    if (state != ADSR_IDLE)
    {
        state = ADSR_RELEASE;
    }
}
