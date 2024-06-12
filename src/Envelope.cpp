#include "Envelope.h"

EnvelopeSettings envelope_settings;

void envelope_initialize()
{
    envelope_settings.attackRate = 50.0 / SAMPLE_RATE;
    envelope_settings.decayRate = 30.0 / SAMPLE_RATE;
    envelope_settings.releaseRate = 30.0 / SAMPLE_RATE;
    envelope_settings.sustainLevel = 0.8;
}

void envelope_note_on(Envelope* envelope)
{
    envelope->state = ADSR_ATTACK;
}

void envelope_note_off(Envelope* envelope)
{
    envelope->state = ADSR_RELEASE;
}
