#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include "RingBuffer.h"
#include "Parameter.h"
#include "WaveTables.h"

#define ROTARY_SPEAKER_FAST (6.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_SLOW (2.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_OFF 0.0

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
        lfo_update();

        float lfoValue = lfo_get_value();
        int maxDelay = BASE_DELAY_SEC * sampleRate;

        float delay = lfoValue * depth * maxDelay;
        delay += additionalDelay;

        float value = buffer.getHermiteAt(delay);

        buffer.write_margined(input);

        // Tremolo
        value *= (lfoValue / 2.0 + 1.0);

        return value;
    }

    void lfo_update()
    {
        lfo_phase += lfo_phase_increment.current_value;

        if (lfo_phase  >= LUT_SIZE)
            lfo_phase  -= LUT_SIZE;

        if (lfo_phase  < 0)
            lfo_phase  += LUT_SIZE;

        lfo_phase_increment.update();
    }

    float lfo_get_value()
    {
        return sine_table_lfo[(int)(lfo_phase)];
    }

    void organ_oscillator_set_vibrato_fast() {
        lfo_phase_increment.setValue(ROTARY_SPEAKER_FAST);
    }

    void organ_oscillator_set_vibrato_slow() {
        lfo_phase_increment.setValue(ROTARY_SPEAKER_SLOW);
    }

    void organ_oscillator_set_vibrato_off() {
        lfo_phase_increment.setValue(ROTARY_SPEAKER_OFF);
    }

private:
	float sampleRate;
	RingBuffer buffer;
	float depth;

    float lfo_phase;
    Parameter lfo_phase_increment
    {
        ROTARY_SPEAKER_FAST,
        ROTARY_SPEAKER_FAST,
        0.001,
        ROTARY_SPEAKER_FAST,
        0.0,
        0.01 / SAMPLE_RATE
    };

    static const int additionalDelay = 3;
};



extern RotarySpeaker rotarySpeaker;

#endif