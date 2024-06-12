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
float envelope_get_amplitude(Envelope *envelope);
void envelope_note_on(Envelope *envelope);
void envelope_note_off(Envelope *envelope);

#endif