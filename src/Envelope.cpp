#include "Envelope.h"

EnvelopeSettings envelopeSettings {
    attackRate: 50.0 / SAMPLE_RATE,
    decayRate: 30.0 / SAMPLE_RATE,
    sustainLevel: 0.8,
    releaseRate: 30.0 / SAMPLE_RATE,
};

void Envelope::noteOn()
{
    state = ADSR_ATTACK;
}

void Envelope::noteOff()
{
    state = ADSR_RELEASE;
}
