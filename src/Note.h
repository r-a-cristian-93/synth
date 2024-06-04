#ifndef NOTE_H
#define NOTE_H

#include <cstdint>
#include "EnvelopeADSR.h"
#include "Oscillator.h"
#include "Instrument.h"

class Note
{
public:
    EnvelopeAdsr envelope;
    Oscillator *oscillator;
    uint8_t midiNote;
    uint8_t velocity;

public:
    Note(Instrument *instrument, uint8_t midiNote, uint8_t velocity)
    {
        this->oscillator = instrument->oscillator;
        this->envelope = EnvelopeAdsr(g_time);
        this->midiNote = midiNote;
        this->velocity = velocity;
    }

    double generateSample(double time)
    {
        return oscillator->generateSample(midiNote, time) * envelope.getAmplitude(time);
    }

    bool operator==(const Note &other) const
    {
        return midiNote == other.midiNote;
    }
};

#endif