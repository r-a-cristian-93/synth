#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include <OrganEngine/Config.h>
#include <OrganEngine/RingBuffer.h>
#include <OrganEngine/Parameter.h>
#include <OrganEngine/WaveTables.h>

#define ROTARY_SPEAKER_FAST (6.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_SLOW (2.0 * PHASE_LUT_SCALE_FACTOR)
#define ROTARY_SPEAKER_OFF 0.0

#define BASE_DELAY_SEC 0.002

extern float       rotarySpeaker_depth;
extern int         rotarySpeaker_additionalDelay;
extern RingBuffer  rotarySpeaker_ringBuffer;

extern float       rotarySpeaker_lfoPhase;
extern float       rotarySpeaker_lfoPhaseIncrement;

void rotary_speaker_initialize();
void rotary_speaker_set_depth(uint8_t midiParam);
void rotary_speaker_set_speed(uint8_t midiParam);

// inline
int16_t rotary_speaker_process_sample(int16_t input);
void rotary_speaker_lfo_advance();

__attribute__((always_inline)) inline
int16_t rotary_speaker_process_sample(int16_t input)
{
    float lfoValue = (float) (sine_table_lfo[(int)(rotarySpeaker_lfoPhase)]) / (MAX_AMPLITUDE);
    int maxDelay = BASE_DELAY_SEC * SAMPLE_RATE;

    float delay = lfoValue * rotarySpeaker_depth * maxDelay;
    delay += rotarySpeaker_additionalDelay;

    int16_t output = rotarySpeaker_ringBuffer.getHermiteAt(delay);

    rotarySpeaker_ringBuffer.write_margined(input);

    // Tremolo
    // >> 4         bring lfo to range 0x0 - 0x7FF
    // + 0x7800     bring lfo to range 0x7800 - 0x7FFF
    // results 6.2% amplitude modulation
    output = (output * ((sine_table_lfo[(int)(rotarySpeaker_lfoPhase)] >> 4) + 0x7FFF)) >> 15;

    // Variably scale tremolo amplitude
    // output = (int32_t)(output * (rotarySpeaker_depth * sine_table_lfo[(int)(rotarySpeaker_lfoPhase)] + (1-rotarySpeaker_depth) * 0x7FFF)) >> 15;

    rotary_speaker_lfo_advance();

    return output;
}

__attribute__((always_inline)) inline
void rotary_speaker_lfo_advance()
{
    rotarySpeaker_lfoPhase += rotarySpeaker_lfoPhaseIncrement;


    if (rotarySpeaker_lfoPhase  >= LUT_SIZE)
        rotarySpeaker_lfoPhase  -= LUT_SIZE;

    if (rotarySpeaker_lfoPhase  < 0)
        rotarySpeaker_lfoPhase  += LUT_SIZE;
}

#endif
