#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "OrganOscillator.h"

class Instrument
{
public:
    EnvelopeAdsr envelope;
    Oscillator *oscillator;

    Instrument(Oscillator *oscillator, EnvelopeAdsr envelope)
    {
        this->oscillator = oscillator;
        this->envelope = envelope;
    }

    ~Instrument()
    {
        delete oscillator;
    }
};

#endif
