#ifndef NOTE_H
#define NOTE_H

#include <cstdint>
#include "EnvelopeADSR.h"

struct Note
{
    uint8_t midiNote;
    EnvelopeAdsr envelope;
    float phaseAccumulator[DRAWBARS_COUNT];
};


#endif