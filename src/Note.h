#ifndef NOTE_H
#define NOTE_H

#include <cstdint>
#include "Envelope.h"

struct Note
{
    uint8_t midiNote = 0;
    EnvelopeAdsr envelope;
    float phaseAccumulator[DRAWBARS_COUNT] = {0.0};
};

#endif