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

struct EnvelopeAdsr
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

void EnvelopeAdsr_Init();
float EnvelopeAdsr_GetAmplitude(EnvelopeAdsr *envelope);
void EnvelopeAdsr_NoteOn(EnvelopeAdsr *envelope);
void EnvelopeAdsr_NoteOff(EnvelopeAdsr *envelope);

#endif