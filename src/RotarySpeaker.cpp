#include "RotarySpeaker.h"

RotarySpeaker::RotarySpeaker()
: depth(0)
{
	setFrequency(VIBRATO_FREQUENCY_DEFAULT_HZ);
	setDepth(VIBRATO_DEPTH_DEFAULT_PERCENT / 100);
}

void RotarySpeaker::initialize()
{
	buffer.resize(BASE_DELAY_SEC * SAMPLE_RATE * 2);

	setFrequency(6.0);
    setDepth(0.1);
}

void RotarySpeaker::setFrequency(float frequency)
{
	// lfo_set_frequency(frequency);
}

void RotarySpeaker::setDepth(float dt)
{
	if (dt < 0)
		depth = 0;
	else if (dt > 1)
		depth = 1;
	else
		depth = dt;
}

RotarySpeaker rotarySpeaker;
