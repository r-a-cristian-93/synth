#include "Lfo.h"
#define _USE_MATH_DEFINES
#include <math.h>

Lfo lfo;

void lfo_initialize(float sampleRate, float frequency)
{
	lfo.sampleRate = sampleRate;
	lfo.frequency = frequency;
}

void lfo_set_frequency(float frequency)
{
	lfo.frequency = frequency;
    lfo.phaseIncrement = 2 * M_PI * lfo.frequency / lfo.sampleRate;
}

void lfo_update()
{
    lfo.phase += lfo.phaseIncrement;

    while(lfo.phase > 2 * M_PI)
        lfo.phase -= 2 * M_PI;
}

float lfo_get_value()
{
    return sin(lfo.phase);
}