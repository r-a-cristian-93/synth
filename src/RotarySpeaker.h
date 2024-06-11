#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include "Config.h"
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
	void initialize();
    void setFrequency(float frequency);
    void setDepth(float depth);


public:
    INLINE float processOneSample(float input)
    {
        lfo_update();

        float lfoValue = lfo_get_value();
        int maxDelay = BASE_DELAY_SEC * SAMPLE_RATE;

        float delay = lfoValue * rotarySpeaker_depth * maxDelay;
        delay += rotarySpeaker_additionalDelay;

        float value = rotarySpeaker_ringBuffer.getHermiteAt(delay);

        rotarySpeaker_ringBuffer.write_margined(input);

        // Tremolo
        value *= (lfoValue / 2.0 + 1.0);

        return value;
    }

    void lfo_update()
    {
        rotarySpeaker_lfoPhase += rotarySpeaker_lfoPhaseIncrement.current_value;

        if (rotarySpeaker_lfoPhase  >= LUT_SIZE)
            rotarySpeaker_lfoPhase  -= LUT_SIZE;

        if (rotarySpeaker_lfoPhase  < 0)
            rotarySpeaker_lfoPhase  += LUT_SIZE;

        rotarySpeaker_lfoPhaseIncrement.update();
    }

    float lfo_get_value()
    {
        return sine_table_lfo[(int)(rotarySpeaker_lfoPhase)];
    }

    void organ_oscillator_set_vibrato_fast() {
        rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_FAST);
    }

    void organ_oscillator_set_vibrato_slow() {
        rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_SLOW);
    }

    void organ_oscillator_set_vibrato_off() {
        rotarySpeaker_lfoPhaseIncrement.setValue(ROTARY_SPEAKER_OFF);
    }

private:
	RingBuffer rotarySpeaker_ringBuffer;
	float rotarySpeaker_depth;

    float rotarySpeaker_lfoPhase;
    Parameter rotarySpeaker_lfoPhaseIncrement
    {
        ROTARY_SPEAKER_FAST,
        ROTARY_SPEAKER_FAST,
        0.001,
        ROTARY_SPEAKER_FAST,
        0.0,
        0.01 / SAMPLE_RATE
    };

    static const int rotarySpeaker_additionalDelay = 3;
};



extern RotarySpeaker rotarySpeaker;

#endif