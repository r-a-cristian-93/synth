#ifndef LFO_H_
#define LFO_H_

#define _USE_MATH_DEFINES
#include <math.h>

class Lfo
{
public:
	Lfo();
	~Lfo();

	void initialize(float sampleRate, float freq);
	void setFrequency(float freq);
	void setPhase(float phase);

public:
    float getValue();

	inline void update()
	{
		phase += phaseIncrement;
		while(phase > 2 * M_PI)
			phase -= 2 * M_PI;
	}

private:
	float sampleRate;
	float frequency;
	float phase;
	float index;
	float phaseIncrement;
};

#endif