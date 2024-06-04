#ifndef ORGAN_OSCILATOR_H
#define ORGAN_OSCILATOR_H

#include <cstdint>
#include "Parameter.h"
#include "Oscillator.h"

extern double note_frequency[MIDI_NOTES_COUNT][DRAWBARS_COUNT];

class OrganOscillator: public Oscillator
{
public:
    OrganOscillator();
    ~OrganOscillator() {}
    double generateSample(uint8_t midiNoteValue, uint32_t time);
    void updateParameters();

private:
    Parameter drawbar_amplitude[DRAWBARS_COUNT];
    Parameter vibrato_amplitude;
    Parameter vibrato_frequency;
};

#endif
