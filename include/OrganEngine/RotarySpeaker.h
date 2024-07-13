#ifndef VIBRATO_PROCESSOR_H
#define VIBRATO_PROCESSOR_H

#include <OrganEngine/Config.h>
#include <OrganEngine/RingBuffer.h>
#include <OrganEngine/Parameter.h>
#include <OrganEngine/WaveTables.h>

#define BASE_DELAY_SEC (0.002)

extern int         rotarySpeaker_additionalDelay;
extern RingBuffer  rotarySpeaker_ringBuffer;

extern float       rotarySpeaker_lfoPhase;
extern float       rotarySpeaker_lfoPhaseIncrement;

void rotary_speaker_initialize();
void rotary_speaker_set_speed(uint8_t midiParam);

// inline
int16_t rotary_speaker_process_sample(int16_t input);
void rotary_speaker_lfo_advance();

__attribute__((always_inline)) inline
int16_t rotary_speaker_process_sample(int16_t input)
{
    const int16_t vibratoLfoValue = (float) (sine_table_lfo[(int)(rotarySpeaker_lfoPhase)]);
    const float delay = vibratoLfoValue * BASE_DELAY_SEC + rotarySpeaker_additionalDelay;

    // Vibrato
    int16_t output = rotarySpeaker_ringBuffer.getHermiteAt(delay);

    // Tremolo
    output = (output * rotary_table_lfo[(int)(rotarySpeaker_lfoPhase)]) >> 15;

    rotarySpeaker_ringBuffer.write_margined(input);
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
