#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <cstdint>

class Oscillator
{
public:
    virtual double generateSample(uint8_t midiNote, uint32_t time) = 0;
    virtual void updateParameters() = 0;
    virtual ~Oscillator() = default;
};
#endif