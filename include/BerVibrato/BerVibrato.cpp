#include "BerVibrato.h"

BerVibrato::BerVibrato()
: depth(0)
, sampleRate(0)
{
	setFrequency(VIBRATO_FREQUENCY_DEFAULT_HZ);
	setDepth(VIBRATO_DEPTH_DEFAULT_PERCENT / 100);
}

void BerVibrato::initialize(float sr)
{
	sampleRate = sr;
	lfo_initialize(sampleRate, VIBRATO_FREQUENCY_DEFAULT_HZ);
	buffer.resize(BASE_DELAY_SEC * sampleRate * 2);
}

void BerVibrato::setFrequency(float frequency)
{
	lfo_set_frequency(frequency);
}

void BerVibrato::setDepth(float dt)
{
	if (dt < 0)
		depth = 0;
	else if (dt > 1)
		depth = 1;
	else
		depth = dt;
}