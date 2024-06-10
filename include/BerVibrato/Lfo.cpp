#include "Lfo.h"
#define _USE_MATH_DEFINES
#include <math.h> /*sin*/

Lfo::Lfo()
	: index(0), sampleRate(0), frequency(0), phase(0)
{}

Lfo::~Lfo(void) {}

void Lfo::initialize(float sr, float freq)
{
	sampleRate = sr;
	frequency = freq;
}

void Lfo::setFrequency(float freq)
{
	frequency = freq;
    phaseIncrement = 2 * M_PI * frequency / sampleRate;
}

void Lfo::setPhase(float ph)
{
	phase = ph;
}

float Lfo::getValue()
{
    return sin(phase);
}
