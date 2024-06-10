#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include "Lfo.h"
#include "RingBuffer.h"

const float BASE_DELAY_SEC = 0.002; // 2 ms
const float VIBRATO_FREQUENCY_DEFAULT_HZ = 2;
const float VIBRATO_FREQUENCY_MAX_HZ = 14;
const float VIBRATO_DEPTH_DEFAULT_PERCENT = 50;

class RotarySpeaker
{
public:
	RotarySpeaker();

public:
	void initialize(float sampleRate);
    void process(float* input, float*output, int numberOfSamples);
    void setFrequency(float frequency);
    void setDepth(float depth);


public:
    INLINE float processOneSample(float input)
    {
        float lfoValue = (lfo_get_value() + 1.0) * 0.5;
        int maxDelay = BASE_DELAY_SEC * sampleRate;

        float delay = lfoValue * depth * maxDelay;
        delay += additionalDelay;

        float value = buffer.getHermiteAt(delay);

        buffer.write_margined(input);

        // Tremolo
        value *= (lfoValue / 2.0 + 1.0);

        return value;
    }

private:
	float sampleRate;
	Lfo lfo;
	RingBuffer buffer;
	float depth;

    static const int additionalDelay = 3;
};

#endif