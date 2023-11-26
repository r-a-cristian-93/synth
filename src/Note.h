#ifndef NOTE_H
#define NOTE_H

#include <cstdint>
#include "EnvelopeADSR.h"

struct Note
{
    uint8_t value = 0;
    EnvelopeADSR envelope;

    bool operator==(const Note &other) const
    {
        return value == other.value;
    }
};

#endif