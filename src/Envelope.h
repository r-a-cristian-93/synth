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
    float attackRate = 10.0 / SAMPLE_RATE;
    float decayRate = 2.0 / SAMPLE_RATE;
    float sustainLevel = 0.5;
    float releaseRate = 5.0 / SAMPLE_RATE;
};

extern EnvelopeSettings envelope_settings;

float EnvelopeAdsr_GetAmplitude(EnvelopeAdsr *envelope);
void EnvelopeAdsr_NoteOn(EnvelopeAdsr *envelope);
void EnvelopeAdsr_NoteOff(EnvelopeAdsr *envelope);

#endif