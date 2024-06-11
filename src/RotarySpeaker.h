#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include "Config.h"
#include "RingBuffer.h"
#include "Parameter.h"
#include "WaveTables.h"

#define ROTARY_SPEAKER_FAST (6.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_SLOW (2.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_OFF 0.0

#define BASE_DELAY_SEC 0.002

extern float       rotarySpeaker_depth;
extern int         rotarySpeaker_additionalDelay;
extern RingBuffer  rotarySpeaker_ringBuffer;

extern float       rotarySpeaker_lfoPhase;
extern Parameter   rotarySpeaker_lfoPhaseIncrement;

void rotary_speaker_initialize();
void rotary_speaker_set_depth(float depth);
void rotary_speaker_set_velocity_fast();
void rotary_speaker_set_velocity_slow();
void rotary_speaker_set_velocity_off();

// inline
float rotary_speaker_process_sample(float input);
void rotary_speaker_lfo_advance();
void rotary_speaker_parameters_update();


__attribute__((always_inline)) inline
float rotary_speaker_process_sample(float input)
{
    float lfoValue = sine_table_lfo[(int)(rotarySpeaker_lfoPhase)];
    int maxDelay = BASE_DELAY_SEC * SAMPLE_RATE;

    float delay = lfoValue * rotarySpeaker_depth * maxDelay;
    delay += rotarySpeaker_additionalDelay;

    float output = rotarySpeaker_ringBuffer.getHermiteAt(delay);

    rotarySpeaker_ringBuffer.write_margined(input);

    // Tremolo
    output *= (lfoValue / 2.0 + 1.0);

    rotary_speaker_lfo_advance();

    return output;
}

__attribute__((always_inline)) inline
void rotary_speaker_lfo_advance()
{
    rotarySpeaker_lfoPhase += rotarySpeaker_lfoPhaseIncrement.current_value;

    if (rotarySpeaker_lfoPhase  >= LUT_SIZE)
        rotarySpeaker_lfoPhase  -= LUT_SIZE;

    if (rotarySpeaker_lfoPhase  < 0)
        rotarySpeaker_lfoPhase  += LUT_SIZE;
}

__attribute__((always_inline)) inline
void rotary_speaker_parameters_update()
{
    rotarySpeaker_lfoPhaseIncrement.update();
}

#endif